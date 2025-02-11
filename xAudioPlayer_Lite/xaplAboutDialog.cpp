#include "xaplAboutDialog.h"

#include "wx/wx.h"
#include "wx/wxhtml.h"
#include "wx/gbsizer.h"
#include "wx/image.h"
#include "wx/mstream.h"

#include "xaplConstants.h"

#include "license.h"
#include "xaplAppIcon.h"

BEGIN_EVENT_TABLE( xaplAboutDialog, wxDialog )
	EVT_BUTTON( ID_Control_BtnClose, xaplAboutDialog::OnClose )
END_EVENT_TABLE()

xaplAboutDialog::xaplAboutDialog( wxWindow *parent, wxWindowID id ) : wxDialog( parent, id, wxT( "About xAudio Player Lite" ), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE ) 
{	
	// Instantiate the controls
	// Labels
	wxMemoryInputStream istream( xaplAppIcon, sizeof xaplAppIcon );
	wxImage icoImg( istream, wxBITMAP_TYPE_PNG );
	progIcon = new wxStaticBitmap( this, wxID_ANY, wxBitmap( icoImg ) );

	lblTitle = new wxStaticText( this, wxID_ANY, wxT( "xAudio Player Lite" ), wxDefaultPosition, wxDefaultSize );
	lblTitle->SetFont( wxFont( 18, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD ) );
	lblVersion = new wxStaticText( this, wxID_ANY, wxT( "Version " ) + APP_VERSION + wxT( " for " ) + APP_PLATFORM, wxDefaultPosition, wxDefaultSize );
	lblVersion->SetFont( wxFont( 13, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL ) );
	lblCopyright = new wxStaticText( this, wxID_ANY, wxT( "(C) 2007 Julia Anderson. Uses portions of the Tango icon theme." ), wxDefaultPosition, wxDefaultSize );
	lblCopyright->SetFont( wxFont( 10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL ) );
	lblFreeSoftware = new wxStaticText( this, wxID_ANY, wxT( "xAudio Player Lite is free software. It is licensed under the GNU GPL version 2:" ), wxDefaultPosition, wxDefaultSize );

	// Controls
	gplDisplay = new wxHtmlWindow( this, ID_Control_GplDisplay, wxDefaultPosition, wxDefaultSize, wxSTATIC_BORDER );
	gplDisplay->SetPage( gpl );

	btnClose = new wxButton( this, ID_Control_BtnClose, wxT( "Close" ), wxDefaultPosition, wxDefaultSize );

	// Create the layout sizer and add the controls
	gridbag = new wxGridBagSizer();

	wxFlexGridSizer *headerSizer = new wxFlexGridSizer( 2, 3 );
	headerSizer->Add( progIcon, 0, wxALIGN_CENTER_VERTICAL );
	headerSizer->Add( lblTitle, 0, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL | wxLEFT, 12 );
	headerSizer->AddSpacer( 1 );
	headerSizer->Add( lblVersion, 0, wxLEFT, 14 );
	headerSizer->AddSpacer( 1 );
	headerSizer->Add( lblCopyright, 0, wxLEFT | wxRIGHT, 14 );

	gridbag->Add( headerSizer, wxGBPosition( 0, 0 ), wxGBSpan( 1, 2 ), wxLEFT | wxTOP, 8 );

	gridbag->Add( lblFreeSoftware, wxGBPosition( 2, 0 ), wxGBSpan( 1, 2 ), wxLEFT | wxRIGHT | wxBOTTOM, 5 );
	gridbag->Add( gplDisplay, wxGBPosition( 3, 0 ), wxGBSpan( 7, 2 ), wxGROW | wxLEFT | wxRIGHT, 4 );

	gridbag->Add( btnClose, wxGBPosition( 10, 0 ), wxGBSpan( 1, 2 ), wxALIGN_RIGHT | wxALL, 5 );

	gridbag->AddGrowableCol( 0 );
	gridbag->SetEmptyCellSize( wxSize( 10, 20 ) );

	gridbag->Fit( this );
	gridbag->SetSizeHints( this );

	SetSizer( gridbag );
}

xaplAboutDialog::~xaplAboutDialog( void )
{
	delete progIcon;
	
	delete lblTitle;
	delete lblVersion;
	delete lblCopyright;
	delete lblFreeSoftware;

	delete gplDisplay;

	delete btnClose;
}

void xaplAboutDialog::Show( void )
{
	gplDisplay->Scroll( 0, 0 );
	ShowModal();
}

void xaplAboutDialog::OnClose( wxCommandEvent &WXUNUSED( evt ) )
{
	this->Close();
}
