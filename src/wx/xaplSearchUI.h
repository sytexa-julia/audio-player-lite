#ifndef __XAPL_SEARCH_UI_H
#define __XAPL_SEARCH_UI_H

#include <wx/wx.h>
#include "xaplMenuBar.h"
#include "xaplPlaylistCtrl.h"
#include "xaplConstants.h"

class xaplSearchUI :
	public wxPanel
{
private:
	wxTextCtrl *m_txtSearch;
	wxBitmapButton *m_btnClear;
	wxBitmapButton *m_btnHide;

	xaplMenuBar *m_menuBar;
	xaplPlaylistCtrl *m_playlist;

protected:
	void Initialize( void );

public:
	xaplSearchUI( wxWindow *parent, xaplMenuBar *mb, xaplPlaylistCtrl *pl );
	~xaplSearchUI( void );

	void Show( void );

	void FocusSearchBox( void );
	void AppendToSearchBox( wxString text );
	wxString GetSearchTerms( void ) const;
	const wxTextCtrl * const GetSearchTextCtrl( void ) { return m_txtSearch; };

	void OnSearchTermsChanged( wxCommandEvent &WXUNUSED( evt ) );
	void OnSearchTermsCleared( wxCommandEvent &WXUNUSED( evt ) );
	void OnSearchBarHidden( wxCommandEvent &evt );
	void OnSearchChar( wxKeyEvent &evt );

	DECLARE_EVENT_TABLE()
};

#endif
