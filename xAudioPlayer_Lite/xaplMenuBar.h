#ifndef __XAPL_MENU_BAR_H
#define __XAPL_MENU_BAR_H

#include <wx/wx.h>
#include "xaplConstants.h"

class xaplMenuBar :
	public wxMenuBar
{
private:
	// Controls
	wxMenu *menuFile;
	wxMenu *menuView;
	wxMenu *menuPlay;
	wxMenu *menuMode;
	wxMenu *menuHelp;

	// State variables
	int currentPlayMode;
	int currentPlaylistFormat;

protected:
	void Initialize( void );

public:
	xaplMenuBar( void );
	
	// Getters
	bool GetPlaylistVisible( void ) const;
	bool GetSoftMuted( void ) const;
	int GetPlaylistFormat( void ) const;
	int GetPlayMode( void ) const;

	// Setters
	void TogglePlaylistVisible( bool chk = true );
	void ToggleSoftMute( bool chk = true );
	void TogglePlaylistFormat( int format = -1 );
	void TogglePlayMode( int mode = -1 );
};

enum MenuItems
{
	MenuItem_File_OpenFile = 1,
	MenuItem_File_OpenLocation,
	MenuItem_File_SavePlaylist,
	MenuItem_File_OpenPlaylist,
	MenuItem_File_Settings,
	MenuItem_File_Exit,

	MenuItem_View_TogglePlaylist,
	MenuItem_View_TogglePLFormat,
	MenuItem_View_PLFileName,
	MenuItem_View_PLFullPath,
	MenuItem_View_PLCustomTags,

	MenuItem_Play_SoftMute,
	MenuItem_Play_VolumeUp,
	MenuItem_Play_VolumeDown,
	MenuItem_Play_Play,
	MenuItem_Play_Pause,
	MenuItem_Play_Stop,
	MenuItem_Play_Next,
	MenuItem_Play_Prev,

	MenuItem_Mode_Toggle,
	MenuItem_Mode_LoopList,
	MenuItem_Mode_LoopSong,
	MenuItem_Mode_Shuffle,

	MenuItem_Help_Contents,
	MenuItem_Help_About
};

#endif
