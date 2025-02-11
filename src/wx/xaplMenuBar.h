#ifndef __XAPL_MENU_BAR_H
#define __XAPL_MENU_BAR_H

#include <wx/wx.h>
#include <wx/accel.h>
#include "xaplConstants.h"
#ifdef __WXGTK20__
#include <gtk/gtk.h>
#endif

#define xaplAUTO_TOGGLE -1

class xaplMenuBar :
	public wxMenuBar
{
	friend class xaplFavoritesManager;

private:
	// Controls
	wxMenu *menuFile;
	wxMenu *menuView;
	wxMenu *menuPlay;
	wxMenu *menuFavorites;
	wxMenu *menuMode;
	wxMenu *menuTools;
	wxMenu *menuHelp;

	// State variables
#ifdef __WXGTK20__
	GtkAccelGroup *storedAccel;
#endif
	int currentPlayMode;
	int currentPlaylistFormat;

protected:
	void Initialize( void );

public:
	xaplMenuBar( void );

	void SetSpaceAcceleratorEnabled( bool e );
	
	// Getters
	wxMenu *GetFileMenu( void ) { return menuFile; }
	wxMenu *GetViewMenu( void ) { return menuView; }
	wxMenu *GetPlayMenu( void ) { return menuPlay; }
	wxMenu *GetFavoritesMenu( void ) { return menuFavorites; }
	wxMenu *GetModeMenu( void ) { return menuMode; }
	wxMenu *GetToolsMenu( void ) { return menuTools; }
	wxMenu *GetHelpMenu( void ) { return menuHelp; }

	bool GetPlaylistVisible( void ) const;
	bool GetSoftMuted( void ) const;
	int GetPlaylistFormat( void ) const;
	int GetPlayMode( void ) const;

	// Setters
	void TogglePlaylistVisible( bool chk = true );
	void ToggleSoftMute( bool chk = true );
	void TogglePlaylistFormat( int format = xaplAUTO_TOGGLE );
	int TogglePlayMode( int mode = xaplAUTO_TOGGLE );
};

#endif
