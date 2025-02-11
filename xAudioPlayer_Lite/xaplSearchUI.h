#ifndef __XAPL_SEARCH_UI_H
#define __XAPL_SEARCH_UI_H

#include <wx/wx.h>

class xaplSearchUI :
	public wxPanel
{
private:
	wxTextCtrl *m_txtSearch;
	wxBitmapButton *m_btnClear;
	wxBitmapButton *m_btnHide;

protected:
	void Initialize( void );

public:
	static const int CTRLID_XAPL_PL_SEARCH = 217;

	xaplSearchUI( wxWindow *parent );
	~xaplSearchUI( void );

	void Show( void );

	void FocusSearchBox( void );
	void AppendToSearchBox( wxString text );
	wxString GetSearchTerms( void ) const;

	void OnSearchTermsChanged( wxCommandEvent &WXUNUSED( evt ) );
	void OnSearchTermsCleared( wxCommandEvent &WXUNUSED( evt ) );
	void OnSearchBarHidden( wxCommandEvent &WXUNUSED( evt ) );

	DECLARE_EVENT_TABLE()
};

typedef enum
{
	CTRLID_SEARCH_TERMS,
	CTRLID_CLEAR_SEARCH,
	CTRLID_HIDE_SEARCH
} xaplSearchUIControls;

#endif
