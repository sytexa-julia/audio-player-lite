#include <wx/mstream.h>
#include <wx/colordlg.h>
#include <wx/dcbuffer.h>
#include "xaplApplication.h"
#include "xaplConstants.h"
#include "xaplBitmaps.h"
#include "xaplUtilities.h"
#include "xaplSettingsDialog.h"

#ifdef _DEBUG
#include <crtdbg.h>
#define DEBUG_NEW new(_NORMAL_BLOCK ,__FILE__, __LINE__)
#else
#define DEBUG_NEW new
#endif
#ifdef _DEBUG
#define new DEBUG_NEW
#endif 

/* ====================================================================
 * ==== xaplListcoloursPreview Definition =============================*/

BEGIN_EVENT_TABLE( xaplListColoursPreview, wxPanel )
	EVT_PAINT( xaplListColoursPreview::OnPaint )
	EVT_ERASE_BACKGROUND( xaplListColoursPreview::OnEraseBackground )
END_EVENT_TABLE()

xaplListColoursPreview::xaplListColoursPreview( wxWindow *parent, const wxString &text )
: wxPanel( parent ), m_label( text )
{
	SetLabel( text );
	SetFont( wxSystemSettings::GetFont( wxSYS_DEFAULT_GUI_FONT ) );
	SetBackgroundColour( *wxWHITE );
	SetForegroundColour( *wxBLACK );
}

xaplListColoursPreview::xaplListColoursPreview( wxWindow *parent, const wxString &text, const wxFont &font, const wxColour &bg, const wxColour &fg )
: wxPanel( parent ), m_label( text )
{
	SetLabel( text );
	SetFont( font );
	SetBackgroundColour( bg );
	SetForegroundColour( fg );
}

wxSize xaplListColoursPreview::GetClientSize( void ) const
{
	wxSize ret;
	wxMemoryDC memDC;
	memDC.SetFont( GetFont() );
	memDC.GetTextExtent( m_label, NULL, &ret.y );
	ret.x += wxPanel::GetSize().x;
	ret.y += 2;
	return ret;
}

wxSize xaplListColoursPreview::GetSize( void ) const
{
	return GetClientSize();
}

void xaplListColoursPreview::PaintBackground( wxDC &dc )
{
	#ifdef __WINDOWS__
	  wxColour ebg = wxSystemSettings::GetColour( wxSYS_COLOUR_WINDOW );
	#else
	  wxColour ebg = wxSystemSettings::GetColour( wxSYS_COLOUR_WINDOWFRAME );
	#endif
	
	wxColour bg = GetBackgroundColour();
	if ( !bg.Ok() )
		bg = ebg;

	wxSize panelSize = wxPanel::GetClientSize();
	wxSize prevSize = GetClientSize();
	int yp = ((double) abs(panelSize.y - prevSize.y)) / 2.0;

	// Clear entire background
	dc.SetBrush( wxBrush( ebg ) );
	dc.SetPen( wxPen( ebg, 1 ) );
	wxRect windowRect( wxPoint(0, 0), panelSize );
	dc.DrawRectangle( windowRect );

	// Draw background colour preview
	dc.SetBrush( wxBrush( bg ) );
	dc.SetPen( wxPen( bg, 1 ) );
	windowRect = wxRect( wxPoint(0, yp), prevSize );
	dc.DrawRectangle( windowRect );
}

void xaplListColoursPreview::OnPaint( wxPaintEvent &evt )
{
	wxPaintDC dc( this );
	PrepareDC( dc );
	PaintBackground( dc );

	wxColour fg = GetForegroundColour();
	if ( !fg.Ok() )
		fg = *wxBLACK;

	wxSize panelSize = wxPanel::GetClientSize();
	wxSize prevSize = GetClientSize();
	int yp = ((double) abs(panelSize.y - prevSize.y)) / 2.0;

	dc.SetFont( GetFont() );
	dc.SetTextForeground( fg );
	dc.DrawText( m_label, 2, yp );
}

void xaplListColoursPreview::OnEraseBackground( wxEraseEvent &evt )
{ }

/* ====================================================================
 * ==== xaplSettingsDialog Definition =====================================*/

BEGIN_EVENT_TABLE( xaplSettingsDialog, wxDialog )	
	EVT_BUTTON( Button_Settings_BgColor, xaplSettingsDialog::OnButtonBgColor )
	EVT_BUTTON( Button_Settings_FgColor, xaplSettingsDialog::OnButtonFgColor )
	EVT_BUTTON( Button_Settings_SelBgColor, xaplSettingsDialog::OnButtonSelBgColor )
	EVT_BUTTON( Button_Settings_SelFgColor, xaplSettingsDialog::OnButtonSelFgColor )
	EVT_BUTTON( Button_Settings_QBgColor, xaplSettingsDialog::OnButtonQBgColor )
	EVT_BUTTON( Button_Settings_QFgColor, xaplSettingsDialog::OnButtonQFgColor )
	EVT_BUTTON( Button_Settings_NpBgColor, xaplSettingsDialog::OnButtonNpBgColor )
	EVT_BUTTON( Button_Settings_NpFgColor, xaplSettingsDialog::OnButtonNpFgColor )
END_EVENT_TABLE()

xaplSettingsDialog::xaplSettingsDialog( wxWindow *parent )
: wxDialog( parent, wxID_ANY, wxT("Settings"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER )
{ 
	m_buffer = wxGetApp().GetConfig()->bufferSize;
	m_forceSoftware = wxGetApp().GetConfig()->forceSoftwareMixing;
#ifdef __WINDOWS__	
	m_transparency = wxGetApp().GetConfig()->windowTransparency;
#endif
	m_plStyle = wxGetApp().GetConfig()->plStyle;

	Initialize();
}

xaplSettingsDialog::~xaplSettingsDialog(void)
{ 
	delete nb;
	delete imgSettings;
	delete lblTitle;
	delete btnOk;
	delete btnCancel;
}

void xaplSettingsDialog::Initialize( void )
{
	// Not sure why these are different under different platforms
	#ifdef __WINDOWS__
	  wxColour bg = wxSystemSettings::GetColour( wxSYS_COLOUR_WINDOW );
	  wxSize szAudioLbl( 50, 28 );
	  wxSize szMediaLbl( 50, 40 );
	#else
	  wxColour bg = wxSystemSettings::GetColour( wxSYS_COLOUR_WINDOWFRAME );
	  wxSize szAudioLbl( 50, 35 );
	  wxSize szMediaLbl( 50, 52 );
	#endif

	// Instantiate the controls
	nb = new wxNotebook( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNB_TOP );

	// General page
	generalPage = new wxPanel( nb );
	lblDefaultForTypes = new wxStaticText( generalPage, wxID_ANY, wxT("Use Xapl to open these file types:") );

	chkDefaultMP3 = new wxCheckBox( generalPage, wxID_ANY, wxT("MP3") );
	chkDefaultOGG = new wxCheckBox( generalPage, wxID_ANY, wxT("OGG") );
	chkDefaultWAV = new wxCheckBox( generalPage, wxID_ANY, wxT("WAV") );
	chkDefaultWMA = new wxCheckBox( generalPage, wxID_ANY, wxT("WMA") );
	chkDefaultAAC = new wxCheckBox( generalPage, wxID_ANY, wxT("AAC") );
	chkDefaultFLAC = new wxCheckBox( generalPage, wxID_ANY, wxT("FLAC") );
	chkSingleInstance = new wxCheckBox( generalPage, wxID_ANY, wxT("Allow only one instance of Xapl at a time") );
	chkAlwaysOnTop = new wxCheckBox( generalPage, wxID_ANY, wxT("Keep playlist window always on top") );
	chkMinimizeToTray = new wxCheckBox( generalPage, wxID_ANY, wxT("Minimize to system tray") );
	chkMinimizeToTray->SetToolTip( wxT("Note: Not all platforms have a system tray!") );
	chkAutoRemoveInvalid = new wxCheckBox( generalPage, wxID_ANY, wxT("Remove invalid items when saving playlists") );

	generalGB = new wxGridBagSizer();
	generalGB->Add( lblDefaultForTypes, wxGBPosition(0, 0), wxGBSpan(1, 3), wxALIGN_CENTER_VERTICAL | wxGROW | wxALL, 5 );

	wxFlexGridSizer *genGridSz = new wxFlexGridSizer(2, 3, 0, 0);
	genGridSz->Add( chkDefaultMP3, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5 );
	genGridSz->Add( chkDefaultOGG, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5 );
	genGridSz->Add( chkDefaultWAV, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5 );
	genGridSz->Add( chkDefaultWMA, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5 );
	genGridSz->Add( chkDefaultAAC, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5 );
	genGridSz->Add( chkDefaultFLAC, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5 );

	generalGB->Add( genGridSz, wxGBPosition(1, 0) , wxGBSpan(1, 1), wxALIGN_CENTER_VERTICAL | wxLEFT, 15 );
	generalGB->Add( chkSingleInstance, wxGBPosition(2, 0), wxGBSpan(1, 3), wxALIGN_CENTER_VERTICAL | wxGROW | wxALL, 5 );
	generalGB->Add( chkAlwaysOnTop, wxGBPosition(3, 0), wxGBSpan(1, 3), wxALIGN_CENTER_VERTICAL | wxGROW | wxALL, 5 );
	generalGB->Add( chkMinimizeToTray, wxGBPosition(4, 0), wxGBSpan(1, 3), wxALIGN_CENTER_VERTICAL | wxGROW | wxALL, 5 );
	generalGB->Add( chkAutoRemoveInvalid, wxGBPosition(5, 0), wxGBSpan(1, 3), wxALIGN_CENTER_VERTICAL | wxGROW | wxALL, 5 );

	generalGB->AddGrowableCol( 2 );
	generalGB->SetEmptyCellSize( wxSize(8, 8) );
	generalGB->Fit( generalPage );
	generalGB->SetSizeHints( generalPage );
	generalPage->SetSizer( generalGB );

	// Audio page
	audioPage = new wxPanel( nb );
	lblBuffer = new wxStaticText( audioPage, wxID_ANY, wxT( "Buffer:" ), wxDefaultPosition, wxDefaultSize );

	lblReqRestart = new wxTextCtrl( audioPage, wxID_ANY, wxT( "Changes to audio settings will take effect the next time you run Xapl." ), wxDefaultPosition, szAudioLbl, wxTE_READONLY | wxTE_MULTILINE | wxBORDER_NONE );
	lblReqRestart->SetBackgroundColour( bg );
	lblReqRestart->SetScrollbar( wxVERTICAL, 0, 0, 0 );
	spnBuffer = new wxSpinCtrl( audioPage, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 4096, 101, 5000, 3000 );
	chkSoftwareMixing = new wxCheckBox( audioPage, wxID_ANY, wxT("Force software mixing"), wxDefaultPosition, wxDefaultSize );

	audioGB = new wxGridBagSizer();
	audioGB->Add( lblReqRestart, wxGBPosition(0, 0), wxGBSpan(1, 2), wxALIGN_CENTER_VERTICAL | wxGROW | wxALL, 5 );
	audioGB->Add( lblBuffer, wxGBPosition(1, 0), wxGBSpan(1, 1), wxALIGN_CENTER_VERTICAL | wxALIGN_LEFT | wxLEFT | wxRIGHT | wxBOTTOM, 5 );
	audioGB->Add( spnBuffer, wxGBPosition(1, 1), wxGBSpan(1, 1), wxALIGN_CENTER_VERTICAL | wxGROW | wxLEFT | wxRIGHT | wxBOTTOM, 5 );
	audioGB->Add( chkSoftwareMixing, wxGBPosition(2, 0), wxGBSpan(1, 2), wxLEFT, 15 );

	audioGB->AddGrowableCol( 1 );
	audioGB->Fit( audioPage );
	audioGB->SetSizeHints( audioPage );
	audioPage->SetSizer( audioGB );

	// Appearance page
	appearancePage = new wxPanel( nb );
#ifdef __WINDOWS__
	lblTransparency = new wxStaticText( appearancePage, wxID_ANY, wxT( "Window Opacity:" ), wxDefaultPosition, wxDefaultSize );
#endif
	lblNormalItem = new xaplListColoursPreview( appearancePage, wxT("Normal Item") );
	lblSelectedItem = new xaplListColoursPreview( appearancePage, wxT( "Selected Item" ) );
	lblQueuedItem = new xaplListColoursPreview( appearancePage, wxT( "Queued Item" ) );
	lblNowPlayingItem = new xaplListColoursPreview( appearancePage, wxT( "Now Playing Item" ) );
	wxFont npFont = lblNowPlayingItem->GetFont();
	npFont.SetWeight( wxBOLD );
	lblNowPlayingItem->SetFont( npFont );

#ifdef __WINDOWS__
	sldTransparency = new wxSlider( appearancePage, wxID_ANY, 255, 25, 255, wxDefaultPosition, wxDefaultSize, wxHORIZONTAL );
#endif
	btnBgColor = new wxButton( appearancePage, Button_Settings_BgColor, wxT( "BG" ), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT );
	btnFgColor = new wxButton( appearancePage, Button_Settings_FgColor, wxT( "FG" ), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT );
	btnSelBgColor = new wxButton( appearancePage, Button_Settings_SelBgColor, wxT( "BG" ), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT );
	btnSelFgColor = new wxButton( appearancePage, Button_Settings_SelFgColor, wxT( "FG" ), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT );
	btnQBgColor = new wxButton( appearancePage, Button_Settings_QBgColor, wxT( "BG" ), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT );
	btnQFgColor = new wxButton( appearancePage, Button_Settings_QFgColor, wxT( "FG" ), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT );
	btnNpBgColor = new wxButton( appearancePage, Button_Settings_NpBgColor, wxT( "BG" ), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT );
	btnNpFgColor = new wxButton( appearancePage, Button_Settings_NpFgColor, wxT( "FG" ), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT );

	appearanceGB = new wxGridBagSizer();

#ifdef __WINDOWS__
	appearanceGB->Add( lblTransparency, wxGBPosition(0, 0), wxGBSpan(1, 1), wxALIGN_CENTER_VERTICAL | wxALIGN_LEFT | wxLEFT | wxTOP, 5 );
	appearanceGB->Add( sldTransparency, wxGBPosition(0, 1), wxGBSpan(1, 1), wxALIGN_CENTER_VERTICAL | wxALIGN_RIGHT | wxGROW | wxTOP | wxLEFT | wxRIGHT, 5 );
	int gbStartY = 1;
#else
	int gbStartY = 0;
#endif

	wxFlexGridSizer *normalItmSzr = new wxFlexGridSizer( 1, 3, 0, 0 );
	normalItmSzr->Add( lblNormalItem, 0, wxALIGN_CENTER_VERTICAL | wxALIGN_LEFT | wxGROW | wxLEFT, 8 );
	normalItmSzr->Add( btnBgColor, 0, wxALIGN_CENTER_VERTICAL | wxALIGN_RIGHT );
	normalItmSzr->Add( btnFgColor, 0, wxALIGN_CENTER_VERTICAL | wxALIGN_RIGHT | wxRIGHT, 8 );
	normalItmSzr->AddGrowableCol( 0 );
	appearanceGB->Add( normalItmSzr, wxGBPosition(gbStartY++, 0), wxGBSpan(1, 2), wxTOP | wxGROW, 10 );
	
	wxFlexGridSizer *selectedItmSzr = new wxFlexGridSizer( 1, 3, 0, 0 );
	selectedItmSzr->Add( lblSelectedItem, 0, wxALIGN_CENTER_VERTICAL | wxALIGN_LEFT | wxGROW | wxLEFT, 8 );
	selectedItmSzr->Add( btnSelBgColor, 0, wxALIGN_CENTER_VERTICAL | wxALIGN_RIGHT );
	selectedItmSzr->Add( btnSelFgColor, 0, wxALIGN_CENTER_VERTICAL | wxALIGN_RIGHT | wxRIGHT, 8 );
	selectedItmSzr->AddGrowableCol( 0 );
	appearanceGB->Add( selectedItmSzr, wxGBPosition(gbStartY++, 0), wxGBSpan(1, 2), wxGROW );
	
	wxFlexGridSizer *queuedItmSzr = new wxFlexGridSizer( 1, 3, 0, 0 );
	queuedItmSzr->Add( lblQueuedItem, 0, wxALIGN_CENTER_VERTICAL | wxALIGN_LEFT | wxGROW | wxLEFT, 8 );
	queuedItmSzr->Add( btnQBgColor, 0, wxALIGN_CENTER_VERTICAL | wxALIGN_RIGHT );
	queuedItmSzr->Add( btnQFgColor, 0, wxALIGN_CENTER_VERTICAL | wxALIGN_RIGHT | wxRIGHT, 8 );
	queuedItmSzr->AddGrowableCol( 0 );
	appearanceGB->Add( queuedItmSzr, wxGBPosition(gbStartY++, 0), wxGBSpan(1, 2), wxGROW );
	
	wxFlexGridSizer *nowPlayingItmSzr = new wxFlexGridSizer( 1, 3, 0, 0 );
	nowPlayingItmSzr->Add( lblNowPlayingItem, 0, wxALIGN_CENTER_VERTICAL | wxALIGN_LEFT | wxGROW | wxLEFT, 8 );
	nowPlayingItmSzr->Add( btnNpBgColor, 0, wxALIGN_CENTER_VERTICAL | wxALIGN_RIGHT );
	nowPlayingItmSzr->Add( btnNpFgColor, 0, wxALIGN_CENTER_VERTICAL | wxALIGN_RIGHT | wxRIGHT, 8 );
	nowPlayingItmSzr->AddGrowableCol( 0 );
	appearanceGB->Add( nowPlayingItmSzr, wxGBPosition(gbStartY++, 0), wxGBSpan(1, 2), wxGROW );

	appearanceGB->AddGrowableCol( 1 );
	appearanceGB->Fit( appearancePage );
	appearanceGB->SetSizeHints( appearancePage );
	appearancePage->SetSizer( appearanceGB );

	// Main dialog
	wxMemoryInputStream istream( xaplSettingsIcon, sizeof xaplSettingsIcon );
	wxImage icoImg( istream, wxBITMAP_TYPE_PNG );
	imgSettings = new wxStaticBitmap( this, wxID_ANY, wxBitmap( icoImg ) );

	lblTitle = new wxStaticText( this, wxID_ANY, wxT( "Settings" ), wxDefaultPosition, wxDefaultSize );
	lblTitle->SetFont( wxFont( 15, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD ) );

	nb->AddPage( generalPage, wxT("General") );
	nb->AddPage( audioPage, wxT("Audio") );
	nb->AddPage( appearancePage, wxT("Appearance") );

	btnOk = new wxButton( this, wxID_OK, wxEmptyString );
	btnCancel = new wxButton( this, wxID_CANCEL, wxEmptyString );

	SetMinSize( wxSize(315, 350) );
	SetSize( 315, 350 );

	// Layout
	gridbag = new wxGridBagSizer();

	wxFlexGridSizer *headerSizer = new wxFlexGridSizer( 2, 1 );
	headerSizer->Add( imgSettings, 0, wxALIGN_CENTER_VERTICAL );
	headerSizer->Add( lblTitle, 0, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT, 12 );
	gridbag->Add( headerSizer, wxGBPosition( 0, 0 ), wxGBSpan( 1, 2 ), wxLEFT | wxTOP, 8 );

	gridbag->Add( nb, wxGBPosition( 1, 0 ), wxDefaultSpan, wxGROW | wxALL, 5 );

	wxFlexGridSizer *buttonSizer = new wxFlexGridSizer( 2, 1 );
	buttonSizer->Add( btnOk, 0 );
	buttonSizer->Add( btnCancel, 0, wxLEFT, 5 );

	gridbag->Add( buttonSizer, wxGBPosition( 2, 0 ), wxGBSpan( 1, 2 ), wxALIGN_RIGHT | wxALL, 5 );

	gridbag->AddGrowableCol( 0 );
	gridbag->AddGrowableRow( 1 );
	gridbag->SetEmptyCellSize( wxSize( 10, 20 ) );

	gridbag->Fit( this );
	SetSizer( gridbag );

	// Load settings
	xaplConfiguration *cfg = wxGetApp().GetConfig();

	chkDefaultMP3->SetValue( cfg->mp3 );
	chkDefaultOGG->SetValue( cfg->ogg );
	chkDefaultWAV->SetValue( cfg->wav );
	chkDefaultWMA->SetValue( cfg->wma );
	chkDefaultAAC->SetValue( cfg->aac );
	chkDefaultFLAC->SetValue( cfg->flac );
	chkSingleInstance->SetValue( cfg->singleInstance );
	chkAlwaysOnTop->SetValue( cfg->alwaysOnTop );
	chkMinimizeToTray->SetValue( cfg->minToTray );
	chkAutoRemoveInvalid->SetValue( cfg->autoDeleteInvalid );

	spnBuffer->SetValue( cfg->bufferSize );
	chkSoftwareMixing->SetValue( cfg->forceSoftwareMixing );
#ifdef __WINDOWS__
	sldTransparency->SetValue( cfg->windowTransparency );
#endif

	lblNormalItem->SetBackgroundColour( cfg->plStyle.bg );
	lblNormalItem->SetForegroundColour( cfg->plStyle.fg );
	lblSelectedItem->SetBackgroundColour( cfg->plStyle.selectionBg );
	lblSelectedItem->SetForegroundColour( cfg->plStyle.selectionFg );
	lblQueuedItem->SetForegroundColour( cfg->plStyle.queuedFg );
	lblNowPlayingItem->SetBackgroundColour( cfg->plStyle.nowPlayingBg );
	lblNowPlayingItem->SetForegroundColour( cfg->plStyle.nowPlayingFg );
}

// Save settings values to application-global xaplConfiguration instance.
// NOTE: additional processing is necessary for changes to have immediate
// effect.
void xaplSettingsDialog::Commit( void )
{
	xaplConfiguration *cfg = wxGetApp().GetConfig();

	// General
	cfg->mp3 = chkDefaultMP3->GetValue();
	cfg->ogg = chkDefaultOGG->GetValue();
	cfg->wav = chkDefaultWAV->GetValue();
	cfg->wma = chkDefaultWMA->GetValue();
	cfg->aac = chkDefaultAAC->GetValue();
	cfg->flac = chkDefaultFLAC->GetValue();
	cfg->singleInstance = chkSingleInstance->GetValue();
	cfg->alwaysOnTop = chkAlwaysOnTop->GetValue();
	cfg->minToTray = chkMinimizeToTray->GetValue();
	cfg->autoDeleteInvalid = chkAutoRemoveInvalid->GetValue();

	// Audio
	cfg->bufferSize = spnBuffer->GetValue();
	cfg->forceSoftwareMixing = chkSoftwareMixing->GetValue();

	// Appearance
#ifdef __WINDOWS__
	cfg->windowTransparency = sldTransparency->GetValue();
#endif
	cfg->plStyle.bg = m_plStyle.bg;
	cfg->plStyle.fg = m_plStyle.fg;
	cfg->plStyle.nowPlayingBg = m_plStyle.nowPlayingBg;
	cfg->plStyle.nowPlayingFg = m_plStyle.nowPlayingFg;
	cfg->plStyle.queuedBg = m_plStyle.queuedBg;
	cfg->plStyle.queuedFg = m_plStyle.queuedFg;
	cfg->plStyle.selectionBg = m_plStyle.selectionBg;
	cfg->plStyle.selectionFg = m_plStyle.selectionFg;

	wxGetApp().SaveConfig();
}

void xaplSettingsDialog::OnButtonBgColor( wxCommandEvent &WXUNUSED(evt) )
{
	wxColourData data;
	data.SetColour( m_plStyle.bg );
	data.SetChooseFull( false );
	wxColourDialog dlg( this, &data );
	if ( dlg.ShowModal() == wxID_OK )
	{
		m_plStyle.bg = dlg.GetColourData().GetColour();
		lblNormalItem->SetBackgroundColour( m_plStyle.bg );

		// Auto-adjust foreground color so it is readable. This is only done
		// when changing the background colors
		int fgBrightness = xaplUtilities::GetColorBrightness( m_plStyle.fg );
		int bgBrightness = xaplUtilities::GetColorBrightness( m_plStyle.bg );
		if ( abs( fgBrightness - bgBrightness ) < 130 )
		{
			wxColour newClr = bgBrightness < 130 ? *wxWHITE : *wxBLACK;
			m_plStyle.fg = newClr;
			lblNormalItem->SetForegroundColour( newClr );
		}
		
		lblNormalItem->Refresh();
	}
}

void xaplSettingsDialog::OnButtonFgColor( wxCommandEvent &WXUNUSED(evt) )
{
	wxColourData data;
	data.SetColour( m_plStyle.fg );
	data.SetChooseFull( false );
	wxColourDialog dlg( this, &data );
	if ( dlg.ShowModal() == wxID_OK )
	{
		m_plStyle.fg = dlg.GetColourData().GetColour();
		lblNormalItem->SetForegroundColour( m_plStyle.fg );
		lblNormalItem->Refresh();
	}
}

void xaplSettingsDialog::OnButtonSelBgColor( wxCommandEvent &WXUNUSED(evt) )
{
	wxColourData data;
	data.SetColour( m_plStyle.selectionBg );
	data.SetChooseFull( false );
	wxColourDialog dlg( this, &data );
	if ( dlg.ShowModal() == wxID_OK )
	{
		m_plStyle.selectionBg = dlg.GetColourData().GetColour();
		lblSelectedItem->SetBackgroundColour( m_plStyle.selectionBg );

		int fgBrightness = xaplUtilities::GetColorBrightness( m_plStyle.selectionFg );
		int bgBrightness = xaplUtilities::GetColorBrightness( m_plStyle.selectionBg );
		if ( abs( fgBrightness - bgBrightness ) < 130 )
		{
			wxColour newClr = bgBrightness < 130 ? *wxWHITE : *wxBLACK;
			m_plStyle.selectionFg = newClr;
			lblSelectedItem->SetForegroundColour( newClr );
		}

		lblSelectedItem->Refresh();
	}
}

void xaplSettingsDialog::OnButtonSelFgColor( wxCommandEvent &WXUNUSED(evt) )
{
	wxColourData data;
	data.SetColour( m_plStyle.selectionFg );
	data.SetChooseFull( false );
	wxColourDialog dlg( this, &data );
	if ( dlg.ShowModal() == wxID_OK )
	{
		m_plStyle.selectionFg = dlg.GetColourData().GetColour();
		lblSelectedItem->SetForegroundColour( m_plStyle.selectionFg );
		lblSelectedItem->Refresh();
	}
}

void xaplSettingsDialog::OnButtonQBgColor( wxCommandEvent &WXUNUSED(evt) )
{
	wxColourData data;
	data.SetColour( m_plStyle.queuedBg );
	data.SetChooseFull( false );
	wxColourDialog dlg( this, &data );
	if ( dlg.ShowModal() == wxID_OK )
	{
		m_plStyle.queuedBg = dlg.GetColourData().GetColour();
		lblQueuedItem->SetBackgroundColour( m_plStyle.queuedBg );

		int fgBrightness = xaplUtilities::GetColorBrightness( m_plStyle.queuedFg );
		int bgBrightness = xaplUtilities::GetColorBrightness( m_plStyle.queuedBg );
		if ( abs( fgBrightness - bgBrightness ) < 130 )
		{
			wxColour newClr = bgBrightness < 130 ? *wxWHITE : *wxBLACK;
			m_plStyle.queuedFg = newClr;
			lblQueuedItem->SetForegroundColour( newClr );
		}

		lblQueuedItem->Refresh();
	}
}

void xaplSettingsDialog::OnButtonQFgColor( wxCommandEvent &WXUNUSED(evt) )
{
	wxColourData data;
	data.SetColour( m_plStyle.queuedFg );
	data.SetChooseFull( false );
	wxColourDialog dlg( this, &data );
	if ( dlg.ShowModal() == wxID_OK )
	{
		m_plStyle.queuedFg = dlg.GetColourData().GetColour();
		lblQueuedItem->SetForegroundColour( m_plStyle.queuedFg );
		lblQueuedItem->Refresh();
	}
}

void xaplSettingsDialog::OnButtonNpBgColor( wxCommandEvent &WXUNUSED(evt) )
{
	wxColourData data;
	data.SetColour( m_plStyle.nowPlayingBg );
	data.SetChooseFull( false );
	wxColourDialog dlg( this, &data );
	if ( dlg.ShowModal() == wxID_OK )
	{
		m_plStyle.nowPlayingBg = dlg.GetColourData().GetColour();
		lblNowPlayingItem->SetBackgroundColour( m_plStyle.nowPlayingBg );

		int fgBrightness = xaplUtilities::GetColorBrightness( m_plStyle.nowPlayingFg );
		int bgBrightness = xaplUtilities::GetColorBrightness( m_plStyle.nowPlayingBg );
		if ( abs( fgBrightness - bgBrightness ) < 130 )
		{
			wxColour newClr = bgBrightness < 130 ? *wxWHITE : *wxBLACK;
			m_plStyle.nowPlayingFg = newClr;
			lblNowPlayingItem->SetForegroundColour( newClr );
		}

		lblNowPlayingItem->Refresh();
	}
}

void xaplSettingsDialog::OnButtonNpFgColor( wxCommandEvent &WXUNUSED(evt) )
{
	wxColourData data;
	data.SetColour( m_plStyle.nowPlayingFg );
	data.SetChooseFull( false );
	wxColourDialog dlg( this, &data );
	if ( dlg.ShowModal() == wxID_OK )
	{
		m_plStyle.nowPlayingFg = dlg.GetColourData().GetColour();
		lblNowPlayingItem->SetForegroundColour( m_plStyle.nowPlayingFg );
		lblNowPlayingItem->Refresh();
	}
}
