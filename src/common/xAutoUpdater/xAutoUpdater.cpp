#include "xAutoUpdater.h"

#ifdef _DEBUG
#include <crtdbg.h>
#define DEBUG_NEW new(_NORMAL_BLOCK ,__FILE__, __LINE__)
#else
#define DEBUG_NEW new
#endif
#ifdef _DEBUG
#define new DEBUG_NEW
#endif 

/****** cURL Callback Functions ******/
size_t WriteFunc( void *ptr, size_t size, size_t nmemb, FILE *stream )
{
	return fwrite( ptr, size, nmemb, stream );
}

int ProgressCallback( void *clientp, double dltotal, double dlnow, double ultotal, double ulnow )
{
	xAutoUpdaterDownloadThread *thread = static_cast<xAutoUpdaterDownloadThread *>( clientp );
	if ( thread != NULL && ( !thread->IsAlive() || thread->TestDestroy() || thread->IsPaused() ) )
		return -1;

	int value = (int) (dlnow * 100.0 / dltotal);
	if ( thread != NULL )
	{
		wxCommandEvent evt;
		evt.SetId( 127 );
		evt.SetInt( value );
		evt.SetEventType( wxEVT_UPDATE_PROGRESS );
		wxPostEvent( thread->GetUpdater(), evt );
	}

	return 0;
}

/****** xAutoUpdaterDownloadThread Implementation ******/
xAutoUpdaterDownloadThread::xAutoUpdaterDownloadThread( xAutoUpdater *updater )
	: wxThread(), 
	  m_updater( updater )
{
	wxASSERT( updater != NULL );
}

xAutoUpdaterDownloadThread::~xAutoUpdaterDownloadThread( void )
{}

void *xAutoUpdaterDownloadThread::Entry( void )
{
	FILE *outFile;
	CURL *curl;
	CURLcode res;

	size_t length = strlen( GetUpdater()->GetTempArchive().mb_str(wxConvUTF8) );
	char *buffer = (char *) malloc( length + 1 );
#ifdef __WINDOWS__
	strcpy_s( buffer, length + 1, GetUpdater()->GetTempArchive().mb_str(wxConvUTF8) );
	if ( fopen_s( &outFile, buffer, "wb" ) != 0 )
	{
		free(buffer);

		wxCommandEvent evt;
		evt.SetId( 127 );
		evt.SetEventType( wxEVT_NOTIFY_CURL_ERROR );
		wxPostEvent( GetUpdater(), evt );

		return NULL;
	}
#else
	strcpy( buffer, GetUpdater()->GetTempArchive().mb_str(wxConvUTF8) );
	outFile = fopen( buffer, "w" );
#endif
	
	curl = curl_easy_init();
	if ( outFile != NULL && curl != NULL )
	{
		// Check if file exists
		curl_easy_setopt( curl, CURLOPT_URL, GetUpdater()->GetUpdateArchive().mb_str() );
		curl_easy_setopt( curl, CURLOPT_NOBODY, 1 );
		curl_easy_perform( curl );
		
		long response;
		res = curl_easy_getinfo( curl, CURLINFO_RESPONSE_CODE, &response );

		// All HTTP error codes
		if ( res != CURLE_OK || response >= 400 )
		{
			curl_easy_cleanup( curl );
			fclose( outFile );
			free(buffer);

			wxCommandEvent evt;
			evt.SetId( 127 );
			evt.SetEventType( wxEVT_NOTIFY_FILE_NOT_FOUND );
			wxPostEvent( GetUpdater(), evt );

			return NULL;
		}

		// Fetch the file
		curl_easy_reset( curl );
		curl_easy_setopt( curl, CURLOPT_URL, GetUpdater()->GetUpdateArchive().mb_str() );
		curl_easy_setopt( curl, CURLOPT_WRITEDATA, outFile );
		curl_easy_setopt( curl, CURLOPT_WRITEFUNCTION, WriteFunc );
		curl_easy_setopt( curl, CURLOPT_NOPROGRESS, 0L );
		curl_easy_setopt( curl, CURLOPT_PROGRESSFUNCTION, ProgressCallback );
		curl_easy_setopt( curl, CURLOPT_PROGRESSDATA, this );
		curl_easy_setopt( curl, CURLOPT_VERBOSE, 1 );

		res = curl_easy_perform( curl );

		// cURL error? Notify main thread and exit
		if ( res != CURLE_OK )
		{
			curl_easy_cleanup( curl );
			fclose( outFile );
			free(buffer);

			wxCommandEvent evt;
			evt.SetId( 127 );
			evt.SetEventType( wxEVT_NOTIFY_CURL_ERROR );
			wxPostEvent( GetUpdater(), evt );

			return NULL;
		}

		curl_easy_cleanup( curl );
		fclose( outFile );
	}

	free(buffer);
	
	// Send transfer complete message
	wxCommandEvent evt;
	evt.SetId( 127 );
	evt.SetEventType( wxEVT_NOTIFY_TRANSFER_COMPLETE );
	wxPostEvent( GetUpdater(), evt );

	return NULL;
}

xAutoUpdater *xAutoUpdaterDownloadThread::GetUpdater( void )
{
	return m_updater;
}


/****** xAutoUpdaterWorkerThread Implementation ******/
xAutoUpdaterWorkerThread::xAutoUpdaterWorkerThread( xAutoUpdater *updater )
	: wxThread(), 
	  m_updater( updater )
{
	wxASSERT( updater != NULL );
}

xAutoUpdaterWorkerThread::~xAutoUpdaterWorkerThread( void )
{}

void *xAutoUpdaterWorkerThread::Entry( void )
{
	PerformUpdate();
	CleanUp();

	wxCommandEvent evt;
	evt.SetId( 127 );
	evt.SetEventType( wxEVT_NOTIFY_UPDATE_SUCCESS );
	wxPostEvent( GetUpdater(), evt );

	return NULL;
}

bool xAutoUpdaterWorkerThread::PerformUpdate( void )
{
	// Create temp directory for extracting files
	wxString extractTempDir = wxString::Format( wxT("%stemp_%s%s"), 
										GetUpdater()->GetTempDirectory().c_str(),
										GetUpdater()->GetArchiveName().c_str(),
										DIR_SEP );

	// Clean up any crap left over from previous updates.
	if ( wxDirExists( extractTempDir.c_str() ) )
	{
		// Remove files
		wxArrayString fileList;
		wxDir::GetAllFiles( extractTempDir, &fileList );

		for ( size_t i = 0; i < fileList.GetCount(); ++i )
		{
			wxRemoveFile( fileList[i] );
		}

		// Remove directories
		wxDir dir( extractTempDir );
		wxString subDir;
		bool cont = dir.GetFirst( &subDir, wxEmptyString, wxDIR_DIRS ); 
		while ( cont )
		{ 
			wxRmdir( extractTempDir + subDir );
			cont = dir.GetNext( &subDir ); 
		} 

		// Remove root dir
		wxRmdir( extractTempDir.c_str() );

		if ( !ProgressPulse() ) return NULL;
	}

	if ( !wxMkdir( extractTempDir.c_str() ) )
		return false;

	// Extract files
	ExtractZipFile( GetUpdater()->GetTempArchive(), extractTempDir );

	// Close app -- target app must close when sent the string message CLOSE over IPC
	// or the file copy probably won't work
	{
		wxLogNull logNull;
		wxClient *client = new wxClient;
		wxConnectionBase* connection = client->MakeConnection( wxT("localhost"), GetUpdater()->GetProductName().Lower(), GetUpdater()->GetProductName().Lower() );
		if ( connection )
		{
			connection->Execute( wxT("CLOSE") );
			connection->Disconnect();
			delete connection;
		}
		delete client;

		// Wait for it to close
		for ( size_t i = 0; i < 20; ++i )
		{
			wxThread::Sleep( 250 );
			if ( !ProgressPulse() ) return NULL;
		}
	}

	// Create new directories
	{
		wxLogNull logNull;
		wxArrayString unprocessed;
		xDirMover dirMover( extractTempDir, GetUpdater()->GetInstallLocation(), unprocessed );
		wxDir dir( extractTempDir );
		dir.Traverse( dirMover );

		if ( !ProgressPulse() ) return NULL;
	}

	// Update registry uninstall info (Windows)
#ifdef __WINDOWS__
	wxRegKey *key = new wxRegKey( wxString::Format( wxT("HKEY_LOCAL_MACHINE\\Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\%s"), GetUpdater()->GetProductName() ) );
	if ( key->Exists() )
	{
		key->SetValue( wxT("DisplayName"), wxString::Format( wxT("%s %s"), GetUpdater()->GetProductName(), GetUpdater()->GetNewVersion() ) );
		key->SetValue( wxT("DisplayVersion"), GetUpdater()->GetNewVersion() );
		key->Close();

		if ( !ProgressPulse() ) return NULL;
	}
	delete key;
#endif

	return true;
}

bool xAutoUpdaterWorkerThread::CleanUp( void )
{
	wxString temp = wxString::Format( wxT("%stemp_%s"), 
										GetUpdater()->GetTempDirectory().c_str(),
										GetUpdater()->GetArchiveName().c_str() );
	return wxRmdir( temp.c_str() ) && wxRemoveFile( GetUpdater()->GetTempArchive() ) ;
}

bool xAutoUpdaterWorkerThread::ExtractZipFile( const wxString &file, const wxString &destination )
{
	bool ret = true; 

    wxFileSystem fs; 
    auto_ptr<wxZipEntry> entry( new wxZipEntry ); 
    do 
	{     
        wxFileInputStream in( file ); 
        if (!in) 
        { 
            wxLogError( wxT("Can not open file '") + file + wxT("'.") ); 
            ret = false; 
            break; 
        } 
        wxZipInputStream zip(in); 

        while ( entry.reset( zip.GetNextEntry() ), entry.get() != NULL ) 
        { 
            // access meta-data 
            wxString name = entry->GetName(); 
            name = destination + name; 

            // read 'zip' to access the entry's data 
            if ( entry->IsDir() ) 
            { 
                int perm = entry->GetMode(); 
                wxFileName::Mkdir(name, perm, wxPATH_MKDIR_FULL); 
            } 
            else // t is a file 
            { 
                zip.OpenEntry(*entry.get()); 
                if (!zip.CanRead()) 
                { 
                    wxLogError( wxT("Can not read zip entry '") + entry->GetName() + wxT("'.") ); 
                    ret = false; 
                    break; 
                } 
                wxFileOutputStream file( name ); 
                if ( !file ) 
                { 
                     wxLogError( wxT("Can not create file '") + name + wxT("'.") ); 
                     ret = false; 
                     break; 
                } 
                zip.Read( file ); 
            } 

			if ( !ProgressPulse() ) return false;
        } 
    } while( false ); 
	
	return ret; 
}

bool xAutoUpdaterWorkerThread::ProgressPulse( void )
{
	wxCommandEvent evt;
	evt.SetId( 127 );
	evt.SetEventType( wxEVT_UPDATE_PROGRESS );
	wxPostEvent( GetUpdater(), evt );
	
	return IsAlive() && !TestDestroy();
}

xAutoUpdater *xAutoUpdaterWorkerThread::GetUpdater( void )
{
	return m_updater;
}	


/****** xAutoUpdater Implementation ******/
DEFINE_EVENT_TYPE(wxEVT_UPDATE_PROGRESS)
DEFINE_EVENT_TYPE(wxEVT_NOTIFY_FILE_NOT_FOUND)
DEFINE_EVENT_TYPE(wxEVT_NOTIFY_CURL_ERROR)
DEFINE_EVENT_TYPE(wxEVT_NOTIFY_TRANSFER_COMPLETE)
DEFINE_EVENT_TYPE(wxEVT_NOTIFY_UPDATE_SUCCESS)
DEFINE_EVENT_TYPE(wxEVT_NOTIFY_UPDATE_FAILURE)

BEGIN_EVENT_TABLE( xAutoUpdater, wxDialog )
	EVT_BUTTON( 125, xAutoUpdater::OnButtonClicked )
	EVT_BUTTON( 126, xAutoUpdater::OnCancelClicked )
	EVT_UPDATE_PROGRESS( 127, xAutoUpdater::OnUpdateProgress )
	EVT_NOTIFY_FILE_NOT_FOUND( 127, xAutoUpdater::OnFileNotFound )
	EVT_NOTIFY_CURL_ERROR( 127, xAutoUpdater::OnCURLError )
	EVT_NOTIFY_TRANSFER_COMPLETE( 127, xAutoUpdater::OnTransferComplete )
	EVT_NOTIFY_UPDATE_SUCCESS(127, xAutoUpdater::OnUpdateSuccess)
	EVT_NOTIFY_UPDATE_FAILURE(127, xAutoUpdater::OnUpdateFailure)
END_EVENT_TABLE()

xAutoUpdater::xAutoUpdater( const wxString &updateArchive, const wxString &productName, const wxString &installLocation, const wxString &mainExe, const wxString &newVersion )
	: wxDialog( NULL, wxID_ANY, wxString::Format( wxT("%s Updater"), productName.c_str() ), wxDefaultPosition, wxDefaultSize, wxCAPTION | wxSYSTEM_MENU ),
	  m_updateArchive( updateArchive ),
	  m_productName( productName ),
	  m_installLocation( installLocation ),
	  m_mainExecutable( mainExe ),
	  m_newVersion( newVersion ),
	  m_transferComplete( false ),
	  m_cancelled( false )
{
	Initialize( updateArchive );
}

xAutoUpdater::~xAutoUpdater(void)
{}

bool xAutoUpdater::Initialize( const wxString &updateArchive )
{
	if ( m_installLocation.Right( 1 ).Cmp( DIR_SEP ) != 0 )
		m_installLocation += DIR_SEP;

	m_tempDir = wxFileName::CreateTempFileName( wxT("xAutoUpdater") );
	if ( !m_tempDir.empty() )
	{
		wxString temp = m_tempDir;
		m_tempDir = m_tempDir.Left( m_tempDir.find_last_of( DIR_SEP ) + 1 );
		wxRemoveFile( temp );
	}

	m_archiveName = m_updateArchive.Right( m_updateArchive.Length() - m_updateArchive.find_last_of( wxT("/") ) - 1 );

	m_tempArchive = wxString::Format( wxT("%s%s"), m_tempDir, m_archiveName );
	if ( wxFileExists( m_tempArchive ) )
		wxRemoveFile( m_tempArchive );

	// GUI setup
	#ifdef __WINDOWS__
	  wxIcon appIco = wxIcon( wxICON( UPDICON ) );
	#else
	  wxIcon appIco = wxIcon( updater_xpm );
	#endif
	
	if ( appIco.Ok() )
		SetIcon( appIco );

	m_downloading = new wxStaticText( this, wxID_ANY, wxString::Format( wxT("Update data for %s is being downloaded. Please wait..."), GetProductName().c_str() ) );
	m_progressBar = new wxGauge( this, wxID_ANY, 100, wxDefaultPosition, wxDefaultSize, wxHORIZONTAL | wxGA_SMOOTH );
	m_gButton = new wxButton( this, 125, wxT("&Cancel") );
	m_cancel = new wxButton( this, 126, wxT("&Cancel") );
	m_cancel->Hide();

	SetEscapeId( 125 );

	m_gridbag = new wxGridBagSizer;
	m_gridbag->Add( m_downloading, wxGBPosition(0, 0), wxGBSpan(1, 1), wxALIGN_CENTER_VERTICAL | wxTOP | wxLEFT | wxRIGHT, 5 );
	m_gridbag->Add( m_progressBar, wxGBPosition(1, 0), wxGBSpan(1, 1), wxGROW | wxALIGN_CENTER_VERTICAL | wxTOP | wxLEFT | wxRIGHT, 5 );
	
	wxFlexGridSizer *gs = new wxFlexGridSizer( 1, 2, 0, 0 );
	gs->Add( m_cancel, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5 );
	gs->Add( m_gButton, 0, wxALIGN_CENTER_VERTICAL );
	m_gridbag->Add( gs, wxGBPosition(2, 0), wxGBSpan(1, 1), wxALIGN_CENTER_VERTICAL | wxALIGN_RIGHT | wxALL, 5 );

	m_gridbag->AddGrowableCol( 0 );
	m_gridbag->AddGrowableRow( 1 );
	m_gridbag->SetEmptyCellSize( wxSize( 10, 20 ) );
	m_gridbag->Fit( this );
	m_gridbag->SetSizeHints( this );

	SetSizer( m_gridbag );

	// Downlaoder thread setup
	curl_global_init(CURL_GLOBAL_ALL);

	m_downloader = new xAutoUpdaterDownloadThread( this );
	m_downloader->Create();

	m_updateWorker = new xAutoUpdaterWorkerThread( this );
	m_updateWorker->Create();

	return true;
}

int xAutoUpdater::ShowModal( void )
{
	m_downloader->Run();	
	return wxDialog::ShowModal();
}

void xAutoUpdater::OnButtonClicked( wxCommandEvent &WXUNUSED(evt) )
{
	if ( !m_transferComplete )
	{
		m_cancelled = true;
		m_downloader->Delete();
		EndModal( -1 ); // Canceled
	}
	else
	{
		m_downloading->SetLabel( wxT("Installing...") );
		m_gButton->Hide();
		m_cancel->Hide();
		m_progressBar->SetIndeterminateMode();
		m_updateWorker->Run();
	}
}

void xAutoUpdater::OnCancelClicked( wxCommandEvent &WXUNUSED(evt) )
{
	m_cancelled = true;
	EndModal( -1 );
}

void xAutoUpdater::OnUpdateProgress( wxCommandEvent &evt )
{
	if ( !m_transferComplete )
		m_progressBar->SetValue( evt.GetInt() );
	else
		m_progressBar->Pulse();
}

void xAutoUpdater::OnFileNotFound( wxCommandEvent &WXUNUSED(evt) )
{
	wxMessageBox( wxT("The update package could not be found on the server."), GetLabel(), wxICON_ERROR | wxOK );
}

void xAutoUpdater::OnCURLError( wxCommandEvent &WXUNUSED(evt) )
{
	if ( !m_cancelled )
		wxMessageBox( wxT("The update package could not be downloaded. Please try again later."), GetLabel(), wxICON_ERROR | wxOK );
}

void xAutoUpdater::OnTransferComplete( wxCommandEvent &WXUNUSED(evt) )
{
	m_downloading->SetLabel( wxT("Click 'Install and Relaunch' to finish the update.") );
	m_gButton->SetLabel( wxT("&Install and Relaunch") );
	m_cancel->Show();
	Layout();
	SetEscapeId( wxID_NONE );
	SetAffirmativeId( 125 );
	RequestUserAttention();
	m_transferComplete = true;
}

void xAutoUpdater::OnUpdateSuccess( wxCommandEvent &WXUNUSED(evt) )
{
	// Start the updated app
	wxExecute( GetInstallLocation() + GetMainExecutableName() );

	EndModal( 0 );
}

void xAutoUpdater::OnUpdateFailure( wxCommandEvent &WXUNUSED(evt) )
{
	wxMessageBox( wxT("The update could not be completed."), GetLabel(), wxICON_ERROR | wxOK );
	EndModal( -2 );
}

wxString &xAutoUpdater::GetNewVersion( void )
{
	return m_newVersion;
}

wxString &xAutoUpdater::GetProductName( void )
{
	return m_productName;
}

wxString &xAutoUpdater::GetInstallLocation( void )
{
	return m_installLocation;
}

wxString &xAutoUpdater::GetMainExecutableName( void )
{
	return m_mainExecutable;
}

wxString &xAutoUpdater::GetUpdateArchive( void )
{
	return m_updateArchive;
}

wxString &xAutoUpdater::GetTempArchive( void )
{
	return m_tempArchive;
}

wxString &xAutoUpdater::GetTempDirectory( void )
{
	return m_tempDir;
}

wxString &xAutoUpdater::GetArchiveName( void )
{
	return m_archiveName;
}
