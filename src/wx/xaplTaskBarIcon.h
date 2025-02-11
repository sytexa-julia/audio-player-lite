#ifndef __XAPL_TASK_BAR_ICON_H
#define __XAPL_TASK_BAR_ICON_H

#include <wx/wx.h>
#include <wx/taskbar.h>

#include "xaplAppIcon.h"
#include "xaplConstants.h"
#include "xaplApplication.h"

#ifdef __WINDOWS__
  #include "resource.h"
#else
  #include "../../art/app32.xpm"
#endif

class xaplTaskBarIcon :
	public wxTaskBarIcon
{
private:
	wxTopLevelWindow *m_mainWindow;
	wxString m_toolTip;
	wxIcon m_icon;
	wxString m_playPauseLabel;

public:
	xaplTaskBarIcon( wxTopLevelWindow *mainWin );
	wxMenu *CreatePopupMenu( void );

	void SetTooltip( const wxString &tt );
	void SetIconVisible( bool v );

	void OnDoubleClick( wxTaskBarIconEvent &evt );

	void OnMenuVolumeFull( wxCommandEvent &WXUNUSED(evt) );
	void OnMenuVolume60( wxCommandEvent &WXUNUSED(evt) );
	void OnMenuVolume30( wxCommandEvent &WXUNUSED(evt) );
	void OnMenuMute( wxCommandEvent &WXUNUSED(evt) );
	void OnMenuPlay( wxCommandEvent &WXUNUSED(evt) );
	void OnMenuPause( wxCommandEvent &WXUNUSED(evt) );
	void OnMenuStop( wxCommandEvent &WXUNUSED(evt) );
	void OnMenuNext( wxCommandEvent &WXUNUSED(evt) );
	void OnMenuPrevious( wxCommandEvent &WXUNUSED(evt) );
	void OnMenuOpen( wxCommandEvent &WXUNUSED(evt) );
	void OnMenuExit( wxCommandEvent &WXUNUSED(evt) );

	DECLARE_EVENT_TABLE();
};

enum TaskBarMenuItems
{
	MenuItem_VolumeFull = 251,
	MenuItem_Volume60,
	MenuItem_Volume30,
	MenuItem_Mute,

	MenuItem_Play,
	MenuItem_Pause,
	MenuItem_Stop,
	MenuItem_Next,
	MenuItem_Previous,

	MenuItem_Open,
	MenuItem_Exit
};

#endif
