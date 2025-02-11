/* 
  Copyright (c) 2009, Xive Software
  All rights reserved.
  
  Redistribution and use in source and binary forms, with or without modification, 
  are permitted provided that the following conditions are met:

     * Redistributions of source code must retain the above copyright notice, this 
	   list of conditions and the following disclaimer.
     * Redistributions in binary form must reproduce the above copyright notice, 
	   this list of conditions and the following disclaimer in the documentation 
	   and/or other materials provided with the distribution.
     * Neither the name of Xive Software nor the names of its contributors 
	   may be used to endorse or promote products derived from this software 
	   without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY 
  EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES 
  OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT 
  SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, 
  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED 
  TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR 
  BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN 
  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH 
  DAMAGE.
*/

#ifndef __XAPL_CONSTANTS_H
#define __XAPL_CONSTANTS_H

#include <wx/string.h>
#include <wx/stdpaths.h>

/* Application version */
static const wxString APP_VERSION = wxT("0.9.5");

/* Standard paths */
static const wxStandardPaths STANDARD_PATHS;

/* Platform-specific defines */
#if defined(__WINDOWS__)
  #define EXPLORE_CMD wxT("explorer.exe /select,%s")
  #define APP_PLATFORM wxT("Windows")
  #define DIR_SEP_STR wxT("\\")
  #define DIR_SEP '\\'
  
  #define EXECUTABLE_DIR wxPathOnly(STANDARD_PATHS.GetExecutablePath())
  #define PLUGIN_DIR wxString::Format(wxT("%s%s%s%s%s"), EXECUTABLE_DIR, DIR_SEP_STR, wxT("plugins"), DIR_SEP_STR, wxT("sound"))
  #define PLUGIN_SPEC wxT("*.dll")

  #define KEY_PLAY_PAUSE VK_MEDIA_PLAY_PAUSE
  #define KEY_STOP VK_MEDIA_STOP
  #define KEY_NEXT VK_MEDIA_NEXT_TRACK
  #define KEY_PREV VK_MEDIA_PREV_TRACK
#elif defined(__LINUX__)
  #define EXPLORE_CMD wxT("xdg-open %s")
  #define APP_PLATFORM wxT("Linux")
  #define DIR_SEP_STR wxT("/")
  #define DIR_SEP '/'

  #define EXECUTABLE_DIR wxPathOnly(STANDARD_PATHS.GetExecutablePath())
  #define PLUGIN_DIR wxString::Format(wxT("%s%s%s%s%s"), STANDARD_PATHS.GetUserDataDir(), DIR_SEP_STR, wxT("plugins"), DIR_SEP_STR, wxT("sound"))
  #define PLUGIN_SPEC wxT("*.so")
#endif

/* Platform-independent defines */
#define CONFIG_DIR wxString::Format(wxT("%s%c"), STANDARD_PATHS.GetUserLocalDataDir().c_str(), DIR_SEP)
#define CONFIG_FILE wxString::Format(wxT("%sxapl.ini"), CONFIG_DIR.c_str())
#define FAVORITES_DIR wxString::Format(wxT("%sFavorites"), CONFIG_DIR.c_str())

/* Important enums */
typedef enum
{
	M3U,
	PLS,
	Text,
	Undefined
} xaplPlaylistFormat;

typedef enum
{
	PLFORMAT_FILENAME = 1,
	PLFORMAT_FULLPATH,
	PLFORMAT_EXTM3U
} xaplPlaylistDisplay;

typedef enum
{
	PLAYMODE_LOOP_LIST = 1,
	PLAYMODE_LOOP_SONG,
	PLAYMODE_SHUFFLE
} xaplPlayMode;

/* Menu Items */
enum MainMenuItems
{
	MenuItem_File_OpenFile = 1,
	MenuItem_File_OpenLocation,
	MenuItem_File_AddInternetStream,
	MenuItem_File_ClearPlaylist,
	MenuItem_File_NewPlaylist,
	MenuItem_File_SavePlaylist,
	MenuItem_File_OpenPlaylist,
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

	MenuItem_Favorites_Add,
	MenuItem_Favorites_Manage,

	MenuItem_Mode_Toggle,
	MenuItem_Mode_LoopList,
	MenuItem_Mode_LoopSong,
	MenuItem_Mode_Shuffle,

	MenuItem_Tools_Settings,
	MenuItem_Tools_ScrollSel,
	MenuItem_Tools_ScrollPlaying,
	MenuItem_Tools_SleepTimer,
	MenuItem_Tools_ResetInvalidFlags,
	MenuItem_Tools_DelMissing,
	MenuItem_Tools_DelDuplicate,
	MenuItem_Tools_ViewHistory,
	MenuItem_Tools_ClearHistory,

	MenuItem_Help_Contents,
	MenuItem_Help_CheckUpdate,
	MenuItem_Help_ReportBug,
	MenuItem_Help_About
};

enum ContextMenu
{
	ContextMenuItem_Queue = 61,
	ContextMenuItem_Remove,
	ContextMenuItem_Explore,
	ContextMenuItem_Properties,
	ContextMenuItem_NewFavorite
};

enum HistoryContextMenu
{
	HistContextMenuItem_Play = 71,
	HistContextMenuItem_Queue,
	HistContextMenuItem_CopyStrings,
	HistContextMenuItem_CopyNames,
	HistContextMenuItem_CopyPaths
};

static const int MenuItem_Favorites_ListStart = 81;
static const int MenuItem_Favorites_ListEnd = 91;

/* Controls */
static const int ListBox_Playlist = 101;
static const int ListBox_History = 102;

enum SearchUIControls
{
	TextCtrl_Search_Terms = 103,
	Button_Search_Clear,
	Button_Search_Hide
};

enum SleepTimerControls
{
	Button_TimerProgress_Abort = 107,
	Radio_SleepAction_CloseApp,
	Radio_SleepAction_SleepComputer,
	Radio_SleepAction_ShutdownComputer
};

enum FavoritesControls
{
	TextCtrl_FavAdd_Name = 115,

	Button_FavManage_Rename,
	Button_FavManage_Delete,
	Button_FavManage_Explore,
	Button_FavManage_Close
};

enum SettingsControls
{
	Button_Settings_BgColor = 135,
	Button_Settings_FgColor,
	Button_Settings_SelBgColor,
	Button_Settings_SelFgColor,
	Button_Settings_QBgColor,
	Button_Settings_QFgColor,
	Button_Settings_NpBgColor,
	Button_Settings_NpFgColor,

	Button_Settings_BrowseMediaDir,
	Button_Settings_ClearMediaDirs
};

enum AdvancedSearchControls
{
	TextCtrl_AdvSearch_Query = 160,
	Button_AdvSearch_Search,
	ListBox_AdvSearch_Results
};

#endif
