#include <wx/image.h>
#include <wx/mstream.h>
#include "xaplPlaylistWindow.h"
#include "xaplBitmaps.h"
#include "xaplSearchUI.h"

#ifdef _DEBUG
#include <crtdbg.h>
#define DEBUG_NEW new(_NORMAL_BLOCK ,__FILE__, __LINE__)
#else
#define DEBUG_NEW new
#endif
#ifdef _DEBUG
#define new DEBUG_NEW
#endif 

BEGIN_EVENT_TABLE( xaplSearchUI, wxPanel )
	EVT_TEXT( TextCtrl_Search_Terms, xaplSearchUI::OnSearchTermsChanged )
	EVT_BUTTON( Button_Search_Clear, xaplSearchUI::OnSearchTermsCleared )
	EVT_BUTTON( Button_Search_Hide, xaplSearchUI::OnSearchBarHidden )
END_EVENT_TABLE()

xaplSearchUI::xaplSearchUI( wxWindow *parent, xaplMenuBar *mb, xaplPlaylistCtrl *pl ) : wxPanel( parent, wxID_ANY ), m_menuBar( mb ), m_playlist( pl )
{
	Initialize();
}

xaplSearchUI::~xaplSearchUI(void)
{
	delete m_txtSearch;
	delete m_btnClear;
	delete m_btnHide;
}

void xaplSearchUI::Initialize( void )
{
	m_txtSearch = new wxTextCtrl( this, TextCtrl_Search_Terms, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER );
	m_txtSearch->GetValidator()->SetBellOnError( false );
	m_txtSearch->Connect( wxEVT_CHAR, wxKeyEventHandler(xaplSearchUI::OnSearchChar), NULL, this );

	wxMemoryInputStream clearImgStream( xaplSearchClearIcon, sizeof xaplSearchClearIcon );
	wxImage clearImg( clearImgStream, wxBITMAP_TYPE_PNG );
	m_btnClear = new wxBitmapButton( this, Button_Search_Clear, wxBitmap( clearImg ) );

	wxMemoryInputStream hideImgStream( xaplSearchHideIcon, sizeof xaplSearchHideIcon );
	wxImage hideImg( hideImgStream, wxBITMAP_TYPE_PNG );
	m_btnHide = new wxBitmapButton( this, Button_Search_Hide, wxBitmap( hideImg ) );

	wxFlexGridSizer *sizer = new wxFlexGridSizer( 1, 3, 0, 0 );
	sizer->Add( m_txtSearch, 0, wxGROW | wxTOP | wxLEFT | wxBOTTOM, 2 );
	sizer->Add( m_btnClear, 0, wxTOP | wxLEFT | wxBOTTOM, 2 );
	sizer->Add( m_btnHide, 0, wxTOP | wxLEFT | wxRIGHT | wxBOTTOM, 2 );

	sizer->AddGrowableCol( 0 );

	SetSizer( sizer );
	sizer->Fit( this );
	sizer->SetSizeHints( this );
}

void xaplSearchUI::Show( void )
{
	if ( m_menuBar != NULL )
		m_menuBar->SetSpaceAcceleratorEnabled( false );
	
	m_txtSearch->Clear();

	wxPanel::Show();
	this->InvalidateBestSize();
	this->GetParent()->Layout();
	this->GetParent()->Refresh();
	this->GetParent()->Update();
}

void xaplSearchUI::FocusSearchBox( void )
{
	m_txtSearch->SetFocus();
}

void xaplSearchUI::AppendToSearchBox( wxString text )
{
	m_txtSearch->AppendText( text );
}

wxString xaplSearchUI::GetSearchTerms( void ) const
{
	return m_txtSearch->GetValue();
}

void xaplSearchUI::OnSearchTermsChanged( wxCommandEvent &WXUNUSED( evt ) )
{
	m_playlist->SetFilter( m_txtSearch->GetValue() );
	((xaplPlaylistWindow *) m_playlist->GetParent())->UpdateSongCountStatusText();
}

void xaplSearchUI::OnSearchTermsCleared( wxCommandEvent &WXUNUSED( evt ) )
{
	m_txtSearch->Clear();
	m_txtSearch->SetFocus();
}

void xaplSearchUI::OnSearchBarHidden( wxCommandEvent &evt )
{
	if ( m_menuBar != NULL )
		m_menuBar->SetSpaceAcceleratorEnabled( true );

	m_txtSearch->Clear();

	if ( evt.GetEventObject() == m_btnHide )
		((xaplPlaylistWindow *) m_playlist->GetParent())->OnSearchBarHidden();

	this->Hide();
	this->InvalidateBestSize();
	this->GetParent()->Layout();
	this->GetParent()->Refresh();
	this->GetParent()->Update();
}

void xaplSearchUI::OnSearchChar( wxKeyEvent &evt )
{
	// Work around Windows design flaw that causes system bell on Return or Escape
	int kc = evt.GetKeyCode();
	if ( kc == WXK_RETURN || kc == WXK_ESCAPE || kc == WXK_UP || kc == WXK_DOWN || kc == WXK_RETURN )
		return;

	evt.Skip();
}
