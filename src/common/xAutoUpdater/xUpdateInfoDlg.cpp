#include "xUpdateInfoDlg.h"

#ifdef _DEBUG
#include <crtdbg.h>
#define DEBUG_NEW new(_NORMAL_BLOCK ,__FILE__, __LINE__)
#else
#define DEBUG_NEW new
#endif
#ifdef _DEBUG
#define new DEBUG_NEW
#endif 

BEGIN_EVENT_TABLE( xUpdateInfoDlg, wxDialog )
	EVT_SIZE( xUpdateInfoDlg::OnSize )
	EVT_BUTTON( Button_InstallLater, xUpdateInfoDlg::OnButtonInstallLater )
	EVT_BUTTON( Button_InstallNow, xUpdateInfoDlg::OnButtonInstallNow )
END_EVENT_TABLE()

xUpdateInfoDlg::xUpdateInfoDlg( const wxString &appIconPath, const wxString &appName, 
		const wxString &installedVersion, const wxString &newVersion, 
		const wxString &updateInfo )
		: wxDialog( NULL, wxID_ANY, wxT("Software Update"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER )
{
	Initialize( appIconPath, appName, installedVersion, newVersion, updateInfo );
}

xUpdateInfoDlg::~xUpdateInfoDlg( void )
{
}

void xUpdateInfoDlg::Initialize( const wxString &appIconPath, const wxString &appName, 
		const wxString &installedVersion, const wxString &newVersion, 
		const wxString &updateInfo )
{
	// Frame icon
	#ifdef __WINDOWS__
	  wxIcon appIco = wxIcon( wxICON( UPDICON ) );
	#else
	  wxIcon appIco = wxIcon( updater_xpm );
	#endif
	
	if ( appIco.Ok() )
		SetIcon( appIco );

	// Create controls
	{
		wxLogNull logNull;

		#ifdef __WINDOWS__
		   wxIcon appImage( appIconPath, wxBITMAP_TYPE_ICO, 32, 32 );
		#else
		   wxIcon appImage( appIconPath, wxBITMAP_TYPE_XPM, 48, 48 );
		#endif

		if ( appImage.Ok() )
			m_appIcon = new wxStaticBitmap( this, wxID_ANY, wxBitmap( appImage ) );
		else
			m_appIcon = new wxStaticBitmap( this, wxID_ANY, wxBitmap( appIco ) );
	}
	
	#ifdef __WINDOWS__
	  wxSize shortInfoSize( 300, 28 );
	#else
	  wxSize shortInfoSize( 300, 35 );
	#endif

	wxFont font = wxSystemSettings::GetFont( wxSYS_DEFAULT_GUI_FONT );
	font.SetWeight( wxBOLD );
	int pointSize = font.GetPointSize();
	font.SetPointSize( pointSize + 3 );

	m_updateAvailable = new wxStaticText( this, wxID_ANY, wxString::Format( wxT("A new version of %s is available!"), appName.c_str() ) );
	m_updateAvailable->SetFont( font );
	m_updateShortInfo = new wxTextCtrl( this, wxID_ANY, wxString::Format( wxT("%s %s is now available. You currently have version %s installed. Would you like to update now?"), appName.c_str(), newVersion.c_str(), installedVersion.c_str() ), wxDefaultPosition, shortInfoSize, wxTE_READONLY | wxTE_MULTILINE | wxBORDER_NONE );
	m_updateShortInfo->SetBackgroundColour( this->GetBackgroundColour() );
	m_updateShortInfo->SetScrollbar( wxVERTICAL, 0, 0, 0 );
	m_updateShortInfo->Connect( wxEVT_LEFT_DOWN, wxMouseEventHandler(xUpdateInfoDlg::OnShortDescMouseDown) );
	m_releaseNotes = new wxStaticText( this, wxID_ANY, wxT("Release Notes:") );
	font.SetPointSize( pointSize );
	m_releaseNotes->SetFont( font );

	m_updateFullInfo = new wxHtmlWindow( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_THEME );
	m_updateFullInfo->SetPage( updateInfo );
	m_updateFullInfo->SetBorders( 1 );
	m_updateFullInfo->SetMinSize( wxSize(200, 130) );

	m_installLater = new wxButton( this, Button_InstallLater, wxT("Install &Later") );
	m_installNow = new wxButton( this, Button_InstallNow, wxT("Install &Now") );
	m_installNow->SetFocus();

	SetAffirmativeId( Button_InstallNow );
	SetEscapeId( Button_InstallLater );

	// Lay out controls
	m_gridbag = new wxGridBagSizer;
	m_gridbag->Add( m_appIcon, wxGBPosition(0, 0), wxDefaultSpan, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL | wxALL, 5 );
	m_gridbag->Add( m_updateAvailable, wxGBPosition(0, 1), wxDefaultSpan, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL | wxRIGHT, 5 );
	m_gridbag->Add( m_updateShortInfo, wxGBPosition(1, 1), wxDefaultSpan, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL | wxRIGHT, 5 );
	
	m_gridbag->Add( m_releaseNotes, wxGBPosition(3, 1), wxDefaultSpan, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL | wxRIGHT | wxBOTTOM, 5 );
	m_gridbag->Add( m_updateFullInfo, wxGBPosition(4, 1), wxDefaultSpan, wxGROW | wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL | wxRIGHT, 5 );

	wxFlexGridSizer *buttonSizer = new wxFlexGridSizer( 1, 2, 0, 0 );
	buttonSizer->Add( m_installLater, 0, wxTOP | wxBOTTOM, 5 );
	buttonSizer->Add( m_installNow, 0, wxALL, 5 );

	m_gridbag->Add( buttonSizer, wxGBPosition(5, 1), wxDefaultSpan, wxALIGN_RIGHT );
	
	m_gridbag->AddGrowableCol( 1 );
	m_gridbag->AddGrowableRow( 4 );
	m_gridbag->SetEmptyCellSize( wxSize( 10, 10 ) );
	m_gridbag->Fit( this );
	m_gridbag->SetSizeHints( this );

	SetSizer( m_gridbag );
} 

void xUpdateInfoDlg::OnSize( wxSizeEvent &evt )
{
	m_updateShortInfo->SetMinSize( wxSize(evt.GetSize().x - 65, m_updateShortInfo->GetMinSize().y) );
	wxDialog::OnSize( evt );
}

// Don't let the user select on the emulated multi-line label
void xUpdateInfoDlg::OnShortDescMouseDown( wxMouseEvent &evt )
{
	return;
}

void xUpdateInfoDlg::OnButtonInstallLater( wxCommandEvent &WXUNUSED(evt) )
{
	EndModal( wxNO );
}

void xUpdateInfoDlg::OnButtonInstallNow( wxCommandEvent &WXUNUSED(evt) )
{
	EndModal( wxYES );
}
