#include "xaplPropertiesDialog.h"
#include "xaplConstants.h"
#include "xaplUtilities.h"
#include <wx/file.h>
#include <wx/filename.h>
#include "../../lib/include/taglib/tag.h"
#include "../../lib/include/taglib/fileref.h"

#ifdef _DEBUG
#include <crtdbg.h>
#define DEBUG_NEW new(_NORMAL_BLOCK ,__FILE__, __LINE__)
#else
#define DEBUG_NEW new
#endif
#ifdef _DEBUG
#define new DEBUG_NEW
#endif 

xaplPropertiesDialog::xaplPropertiesDialog( wxWindow *parent, xaplPlaylistItem *plItem ) 
	: wxDialog( parent, wxID_ANY, wxT("Properties"), wxDefaultPosition, wxDefaultSize, 
	wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER ), m_plItem( plItem )
{
	wxASSERT_MSG( plItem != NULL, wxT("Cannot initialize xaplPropertiesDialog without a valid playlist item pointer.") );
	
	Initialize();
}

xaplPropertiesDialog::~xaplPropertiesDialog( void )
{
	delete m_lblRenameFile;
	delete m_lblChangeM3uTitle;
	
	delete m_txtFileName;
	delete m_txtM3uTitle;
	delete m_txtData;

	delete m_btnOk;
	delete m_btnCancel;
}

void xaplPropertiesDialog::Initialize( void )
{	
	// Instantiate the controls
	// Labels
	m_lblRenameFile = new wxStaticText( this, wxID_ANY, wxT( "Rename File: " ), wxDefaultPosition, wxDefaultSize );
	m_lblChangeM3uTitle = new wxStaticText( this, wxID_ANY, wxT( "Extended M3U Title:" ), wxDefaultPosition, wxDefaultSize );
	wxFont temp = m_lblChangeM3uTitle->GetFont();

	// Controls
	m_txtFileName = new wxTextCtrl( this, wxID_ANY );
	m_txtM3uTitle = new wxTextCtrl( this, wxID_ANY );
	m_txtData = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 380, 320 ), wxTE_MULTILINE | wxTE_READONLY | wxBORDER_NONE );
	m_txtData->SetBackgroundColour( m_lblRenameFile->GetBackgroundColour() );

	m_btnOk = new wxButton( this, wxID_OK, wxEmptyString );
	m_btnCancel = new wxButton( this, wxID_CANCEL, wxEmptyString );

	// Layout
	wxGridBagSizer *gridbag = new wxGridBagSizer();

	gridbag->Add( m_lblRenameFile, wxGBPosition( 0, 0 ), wxGBSpan( 1, 1 ), wxALIGN_CENTER_VERTICAL, 5 );
	gridbag->Add( m_txtFileName, wxGBPosition( 0, 1 ), wxGBSpan( 1, 2 ), wxGROW | wxLEFT | wxALIGN_CENTER_VERTICAL, 5 );

	gridbag->Add( m_lblChangeM3uTitle, wxGBPosition( 1, 0 ), wxGBSpan( 1, 1 ), wxTOP | wxALIGN_CENTER_VERTICAL, 5 );
	gridbag->Add( m_txtM3uTitle, wxGBPosition( 1, 1 ), wxGBSpan( 1, 2 ), wxGROW | wxTOP | wxLEFT | wxALIGN_CENTER_VERTICAL, 5 );

	wxStaticBoxSizer *detailsSizer = new wxStaticBoxSizer( wxVERTICAL, this, wxT("Details") );
	detailsSizer->Add( m_txtData, 1, wxGROW | wxLEFT, 5 );
	gridbag->Add( detailsSizer, wxGBPosition( 2, 0 ), wxGBSpan( 1, 2 ), wxGROW | wxTOP | wxBOTTOM | wxALIGN_CENTER_VERTICAL, 5 );
	
	wxFlexGridSizer *buttonSizer = new wxFlexGridSizer( 2, 1 );
	buttonSizer->Add( m_btnOk, 0 );
	buttonSizer->Add( m_btnCancel, 0, wxLEFT, 5 );
	gridbag->Add( buttonSizer, wxGBPosition( 3, 0 ), wxGBSpan( 1, 2 ), wxALIGN_RIGHT, 5 );

	gridbag->AddGrowableCol( 1 );
	gridbag->AddGrowableRow( 2 );

	// 5px border all around
	wxGridSizer *gridSizer = new wxGridSizer(1, 1, 0, 0);
	gridSizer->Add( gridbag, 0, wxGROW | wxALL, 5 );

	gridSizer->Layout();
	gridSizer->SetSizeHints( this );
	
	SetSizer( gridSizer );
}

int xaplPropertiesDialog::ShowModal( void )
{
	if ( !LoadData() ) 
	{
		m_plItem->ok = false;
		return -1;
	}

	return wxDialog::ShowModal();
}

bool xaplPropertiesDialog::LoadData( void )
{
	wxLogNull noLog;
	wxFile fd( m_plItem->path );
	if ( !fd.IsOpened() )
		return false;
	
	// Simple data
	SetTitle( wxString::Format( wxT("'%s' Properties"), m_plItem->fileName.c_str() ) );
	m_txtFileName->SetValue( m_plItem->fileName );
	m_txtFileName->SetSelection( 0, 0 );
	m_txtM3uTitle->SetValue( m_plItem->m3uTitle );
	
	// Complex data
	m_txtData->AppendText( m_plItem->path.Mid( 0, m_plItem->path.Last(DIR_SEP) + 1 ) );
	m_txtData->AppendText( wxT("\n\n") );

	m_txtData->AppendText( wxT("Size:\t\t") );
	m_txtData->AppendText( xaplUtilities::FormatThousands( wxString::Format( wxT("%i"), fd.Length() ) ) );
	m_txtData->AppendText( wxT("  bytes\n\n") );
	fd.Close();

	char str[2048];
	strcpy(str, (const char *)m_plItem->path.mb_str(wxConvUTF8)); 
	TagLib::FileName fn( str );
	TagLib::FileRef file( fn );
	if ( !file.isNull() && file.tag() )
	{
		TagLib::Tag *tag = file.tag();

		m_txtData->AppendText( wxT("Title:\t\t") );
		m_txtData->AppendText( wxString(tag->title().toCString(true), wxConvLibc).Trim().Trim(false) );
		m_txtData->AppendText( wxT("\n") );
		
		m_txtData->AppendText( wxT("Artist:\t\t") );
		m_txtData->AppendText( wxString(tag->artist().toCString(true), wxConvLibc).Trim().Trim(false) );
		m_txtData->AppendText( wxT("\n") );

		m_txtData->AppendText( wxT("Album:\t\t") );
		m_txtData->AppendText( wxString(tag->album().toCString(true), wxConvLibc).Trim().Trim(false) );
		m_txtData->AppendText( wxT("\n") );

		m_txtData->AppendText( wxT("Year:\t\t") );
		m_txtData->AppendText( wxString::Format( wxT("%i"), (int) tag->year() ) );
		m_txtData->AppendText( wxT("\n") );

		m_txtData->AppendText( wxT("Comments:\t") );
		m_txtData->AppendText( wxString(tag->comment().toCString(true), wxConvLibc).Trim().Trim(false) );
		m_txtData->AppendText( wxT("\n") );

		m_txtData->AppendText( wxT("Track #:\t\t") );
		m_txtData->AppendText( wxString::Format( wxT("%i"), (int) tag->track() ) );
		m_txtData->AppendText( wxT("\n") );

		m_txtData->AppendText( wxT("Genre:\t\t") );
		m_txtData->AppendText( wxString(tag->genre().toCString(true), wxConvLibc).Trim().Trim(false) );
		m_txtData->AppendText( wxT("\n\n") );

		if ( file.audioProperties() ) 
		{
			TagLib::AudioProperties *properties = file.audioProperties();

			int seconds = properties->length() % 60;
			int minutes = ( properties->length() - seconds ) / 60;

			m_txtData->AppendText( wxT("Length:\t\t") );
			m_txtData->AppendText( wxString::Format( wxT("%i:%02i"), minutes, seconds ) );
			m_txtData->AppendText( wxT("\n") );

			m_txtData->AppendText( wxT("Bitrate:\t\t") );
			m_txtData->AppendText( xaplUtilities::FormatThousands( wxString::Format( wxT("%i"), properties->bitrate() ) ) );
			m_txtData->AppendText( wxT(" kbps\n") );

			m_txtData->AppendText( wxT("Sample Rate:\t") );
			m_txtData->AppendText( xaplUtilities::FormatThousands( wxString::Format( wxT("%i"), properties->sampleRate() ) ) );
			m_txtData->AppendText( wxT("\n") );

			m_txtData->AppendText( wxT("Channels:\t") );
			m_txtData->AppendText( wxString::Format( wxT("%i"), properties->channels() ) );
			m_txtData->AppendText( wxT("\n") );

			return true;
		}
	}

	return false;
}

int xaplPropertiesDialog::CommitChanges( void )
{
	int ret = 1;

	// File name
	wxString newFileName = m_txtFileName->GetValue().Trim().Trim(false);
	
	if ( newFileName != m_plItem->fileName )
	{
		wxFileName plPath( m_plItem->path );
		int flag = newFileName.StartsWith( DIR_SEP_STR ) ? wxPATH_GET_VOLUME : wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR;
		wxString newFilePath = plPath.GetPath( flag ).Append( newFileName );

		if ( ::wxRenameFile( m_plItem->path, newFilePath ) )
		{
			m_plItem->path = newFilePath.c_str();
			m_plItem->fileName = newFileName.c_str();
			ret = 2;
		}
		else
		{
			ret = 0;
		}
	}

	// Extended M3U Title
	wxString newM3uTitle = m_txtM3uTitle->GetValue().Trim().Trim(false);

	if ( newM3uTitle != m_plItem->m3uTitle )
	{
		m_plItem->m3uTitle = newM3uTitle.c_str();
		ret = 2;
	}

	return ret;
}
