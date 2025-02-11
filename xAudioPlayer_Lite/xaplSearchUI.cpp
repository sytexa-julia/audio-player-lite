#include "xaplSearchUI.h"

#include "wx/image.h"
#include "wx/mstream.h"

#include "xaplBitmaps.h"

BEGIN_EVENT_TABLE( xaplSearchUI, wxPanel )
	EVT_TEXT( CTRLID_SEARCH_TERMS , xaplSearchUI::OnSearchTermsChanged )
	EVT_BUTTON( CTRLID_CLEAR_SEARCH, xaplSearchUI::OnSearchTermsCleared )
	EVT_BUTTON( CTRLID_HIDE_SEARCH, xaplSearchUI::OnSearchBarHidden )
END_EVENT_TABLE()

xaplSearchUI::xaplSearchUI( wxWindow *parent ) : wxPanel( parent, CTRLID_XAPL_PL_SEARCH )
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
	m_txtSearch = new wxTextCtrl( this, CTRLID_SEARCH_TERMS, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER );
	
	wxMemoryInputStream clearImgStream( xaplSearchClear, sizeof xaplSearchClear );
	wxImage clearImg( clearImgStream, wxBITMAP_TYPE_PNG );
	m_btnClear = new wxBitmapButton( this, CTRLID_CLEAR_SEARCH, wxBitmap( clearImg ) );

	wxMemoryInputStream hideImgStream( xaplSearchHide, sizeof xaplSearchHide );
	wxImage hideImg( hideImgStream, wxBITMAP_TYPE_PNG );
	m_btnHide = new wxBitmapButton( this, CTRLID_HIDE_SEARCH, wxBitmap( hideImg ) );

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
	wxPanel::Show();
	this->InvalidateBestSize();
	this->GetParent()->Layout();
	this->GetParent()->Refresh();
	this->GetParent()->Update();
	this->SetSize( this->GetSize() );
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

}

void xaplSearchUI::OnSearchTermsCleared( wxCommandEvent &WXUNUSED( evt ) )
{
	m_txtSearch->Clear();
	m_txtSearch->SetFocus();
}

void xaplSearchUI::OnSearchBarHidden( wxCommandEvent &WXUNUSED( evt ) )
{
	m_txtSearch->Clear();
	
	this->Hide();
	this->InvalidateBestSize();
	this->GetParent()->Layout();
	this->GetParent()->Refresh();
	this->GetParent()->Update();
	this->SetSize( this->GetSize() );
}
