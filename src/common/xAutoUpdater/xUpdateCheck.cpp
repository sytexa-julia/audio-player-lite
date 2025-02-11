#include "xUpdateCheck.h"

#ifdef _DEBUG
#include <crtdbg.h>
#define DEBUG_NEW new(_NORMAL_BLOCK ,__FILE__, __LINE__)
#else
#define DEBUG_NEW new
#endif
#ifdef _DEBUG
#define new DEBUG_NEW
#endif 

xUpdateCheck::xUpdateCheck( const wxString &productName, const wxString &localVersion, const wxString &broadcastPage, const wxString &installLocation, const wxString &mainExe )
	: m_productName( productName ), 
	  m_localVersion( localVersion ),
	  m_broadcastPage( broadcastPage ), 
	  m_installLocation( installLocation ),
	  m_mainExecutable( mainExe ),
	  m_updateMode( UpdateMode_LaunchBrowser )
{
	wxASSERT( !productName.empty() && !localVersion.empty() && !broadcastPage.empty() && !installLocation.empty() );
}

xUpdateCheck::xUpdateCheck( const wxString &productName, const wxString &localVersion, const wxString &broadcastPage, const wxString &installLocation, const wxString &mainExe, int updateMode )
	: m_productName( productName ), 
	  m_localVersion( localVersion ),
	  m_broadcastPage( broadcastPage ), 
	  m_installLocation( installLocation ),
	  m_mainExecutable( mainExe ),
	  m_updateMode( updateMode )
{
	wxASSERT( !productName.empty() && !localVersion.empty() && !broadcastPage.empty() && !installLocation.empty() );

	if ( updateMode < 0 || updateMode > 2 )
		m_updateMode = UpdateMode_LaunchBrowser;
}

xUpdateCheck::~xUpdateCheck( void )
{}

// Returns true if the URL is valid. A valid URL looks like
// http://myserver.com/SomeDir/BroadcastPage.ext
// The http:// is required
bool xUpdateCheck::ParseBroadcastUrl( const wxString &url )
{
	// http:// is required
	wxString working = url.Lower().Trim().Trim( false );
	if ( working.Left(6).Cmp( wxT("http://") ) != 0 )
		return false;

	return true;
}

xBroadcastInfo *xUpdateCheck::ParseBroadcastData( const wxString &data )
{
	wxString homepage, latestVersion, updateType, archiveLoc, archiveFormat, description;
	wxString working = data;
	size_t start, end;

	// Find correct entry for this product
	start = working.find( wxString::Format( wxT("<product name=\"%s\" platform=\"%s\">"), m_productName, OS_NAME ).c_str() );
	end = working.find( wxT("</product>"), start );
	if ( start != wxString::npos && end != wxString::npos )
	{
		size_t offset = 29 + m_productName.Length() + OS_NAME.Length();
		working = working.substr( start + offset, end - start - offset );

		// Get home page link
		start = working.find( wxT("<homepage>") );
		end = working.find( wxT("</homepage>"), start );
		if ( start != wxString::npos && end != wxString::npos )
			homepage = working.substr( start + 10, end - start - 10 );

		// Get latest version number
		start = working.find( wxT("<latestVersion number=\"") );
		end = working.find( wxT("\">"), start );
		if ( start != wxString::npos && end != wxString::npos )
		{
			latestVersion = working.substr( start + 23, end - start - 23 );
			latestVersion = latestVersion.Trim().Trim( false ).Lower();
		}

		// Get update type info
		start = working.find( wxT("<updateType>") );
		end = working.find( wxT("</updateType>"), start );
		if ( start != wxString::npos && end != wxString::npos )
		{
			updateType = working.substr( start + 12, end - start - 12 );
			updateType = updateType.Trim().Trim( false );
		}

		// Get update archive info
		start = working.find( wxT("<updateArchive>") );
		end = working.find( wxT("</updateArchive>"), start );
		if ( start != wxString::npos && end != wxString::npos )
		{
			wxString archiveData = working.substr( start + 15, end - start - 15 );
			
			// Archive location
			start = archiveData.find( wxT("<location>") );
			end = archiveData.find( wxT("</location>"), start );
			if ( start != wxString::npos && end != wxString::npos )
			{
				archiveLoc = archiveData.substr( start + 10, end - start - 10 );
				archiveLoc = archiveLoc.Trim().Trim( false );
			}

			// Archive format
			start = archiveData.find( wxT("<type>") );
			end = archiveData.find( wxT("</type>"), start );
			if ( start != wxString::npos && end != wxString::npos )
			{
				archiveFormat = archiveData.substr( start + 6, end - start - 6 );
				archiveFormat = archiveFormat.Trim().Trim( false ).Lower();
			}
		}

		// Get update description
		start = working.find( wxT("<updateDescription>") );
		end = working.find( wxT("</updateDescription>"), start );
		if ( start != wxString::npos && end != wxString::npos )
		{
			description = working.substr( start + 19, end - start - 19 );
			description = description.Trim().Trim( false );
		}
	}

	if ( !latestVersion.empty() && !homepage.empty() )
	{
		xBroadcastInfo *info = new xBroadcastInfo;
		info->productHomepage = homepage;
		info->latestVersion = latestVersion;
		info->updateType = updateType;
		info->updateArchiveLocation = archiveLoc;
		info->updateArchiveFormat = archiveFormat;
		info->updateDescription = description;

		return info;
	}

	return NULL;
}

void xUpdateCheck::SetUpdateMode( int mode )
{
	wxASSERT( mode >= 0 && mode <= 2 );
	m_updateMode = mode;
}

int  xUpdateCheck::GetUpdateMode( void )
{
	return m_updateMode;
}

bool xUpdateCheck::PerformCheck( void )
{
	// Initialize connection
	wxHTTP http;
	bool result = false;

	// Parse broadcast page location
	wxString url = m_broadcastPage.Right( m_broadcastPage.Length() - 7 );
	wxString path;
	size_t breaker = url.find( wxT("/") );
	if ( breaker == wxNOT_FOUND )
	{
		path = wxT("/");
	}
	else
	{
		path = url.SubString( breaker, url.Length() );
		url = url.Left( breaker );
	}

    http.SetHeader( wxT( "Accept" ), wxT( "text/*" ) ); 
    http.SetHeader( wxT( "User-Agent" ), m_productName ); 

	// Try to connect
    if ( http.Connect( url ) ) 
    { 
		// Try to read data
		wxInputStream *stream = http.GetInputStream( path ); 
		wxProtocolError err = http.GetError(); 

		// If no error occurred, read the data
		if ( err == wxPROTO_NOERR ) 
		{ 
			wxString res;
			wxStringOutputStream strOutputStream( &res );
			stream->Read( strOutputStream );
			
			xBroadcastInfo *info = ParseBroadcastData( res );
			if ( info != NULL )
			{
				result = true;

				if ( m_localVersion == info->latestVersion )
					wxMessageBox( wxString::Format( wxT("You are running the latest version of %s."), m_productName ).c_str(), m_productName );
				else
				{
					switch ( m_updateMode )
					{
					case UpdateMode_AutoInstall:
						{
							wxString mainExePath = wxString::Format( wxT("%s%s%s"), m_installLocation.c_str(), DIR_SEP, m_mainExecutable.c_str() );
							xUpdateInfoDlg *updateInfoDlg = new xUpdateInfoDlg( mainExePath, m_productName, m_localVersion, info->latestVersion, info->updateDescription );
							if ( updateInfoDlg->ShowModal() == wxYES )
							{
								xAutoUpdater *autoUpdater = new xAutoUpdater( info->updateArchiveLocation, m_productName, m_installLocation, m_mainExecutable, info->latestVersion );
								autoUpdater->ShowModal();
								delete autoUpdater;
							}
							delete updateInfoDlg;
						}
						break;
					case UpdateMode_LaunchBrowser:
						{
							int goDownloadNow = wxMessageBox( wxString::Format( wxT( "A newer version of %s is available. Would you like to visit the download page now?" ), m_productName ).c_str(), m_productName, wxYES_NO );
							if ( goDownloadNow == wxYES )
							{ 
								if ( !wxLaunchDefaultBrowser( info->productHomepage ) )
									wxMessageBox( wxString::Format( wxT("Your web browser could not be opened to the download page. You can visit the website yourself at %s."), info->productHomepage ).c_str(), m_productName );
							}
						}
						break;
					case UpdateMode_NotifyOnly:
					default:
						wxMessageBox( wxString::Format( wxT( "A newer version of %s is available. Please visit %s to download the update." ), m_productName, info->productHomepage ).c_str(), m_productName );
						break;
					}
				}

				delete info;
			}
			else
			{
				wxMessageBox( wxT( "The update service returned a poorly formatted update descriptor. You can visit the product's website to check if a newer version is available." ), m_productName, wxICON_ERROR );
			}
		}

		wxDELETE( stream );
    }
	else
	{
		wxMessageBox( wxT( "An error occurred while trying to connect to the update service. The service may be offline, or you may not be connected to the Internet." ), m_productName, wxICON_ERROR );
	}

	return result;
}
