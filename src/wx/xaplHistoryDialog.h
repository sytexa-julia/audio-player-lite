#ifndef __XAPL_HISTORY_DIALOG_H
#define __XAPL_HISTORY_DIALOG_H

#include <wx/wx.h>
#include "xaplPlaylistCtrl.h"
#include "xaplConstants.h"

class xaplPlaylistWindow;

class xaplHistoryDialog :
	public wxFrame
{
private:
	xaplPlaylistWindow *m_mainWin;
	xaplPlaylistCtrl *m_playlist;

	wxListBox *m_histList;
	wxMenu *m_contextMenu;

protected:
	void Initialize( void );

public:
	xaplHistoryDialog( xaplPlaylistWindow *mainWin, xaplPlaylistCtrl *playlist );
	~xaplHistoryDialog( void );

	void Append( long id );
	void Clear( void );

	void OnContextMenuPlay( wxCommandEvent &WXUNUSED(evt) );
	void OnContextMenuQueue( wxCommandEvent &WXUNUSED(evt) );
	void OnContextMenuCopyStrings( wxCommandEvent &WXUNUSED(evt) );
	void OnContextMenuCopyNames( wxCommandEvent &WXUNUSED(evt) );
	void OnContextMenuCopyPaths( wxCommandEvent &WXUNUSED(evt) );
	
	void OnItemActivated( wxCommandEvent &WXUNUSED(evt) );
	void OnRightClick( wxMouseEvent &evt );

	void OnClose( wxCloseEvent &evt );

	DECLARE_EVENT_TABLE();
};

#endif
