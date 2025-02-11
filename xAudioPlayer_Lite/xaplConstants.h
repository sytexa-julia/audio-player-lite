#ifndef __XAPL_CONSTANTS_H
#define __XAPL_CONSTANTS_H

#include <wx/string.h>
#include <wx/stdpaths.h>

/* Application version specification */
static const wxString APP_VERSION = wxT( "0.5.0" );

/* Platform text and font specification */
#if defined(__WINDOWS__)
  static const wxString APP_PLATFORM = wxT( "Windows" );
  static const wxString SUPPORTED_FORMATS = wxT( "All Supported Files|*.mp1;*.mp2;*.mp3;*.wav;*.ogg;*.wma;*.m4a;*.aac;*.flac|MPEG Audio Files (*.mp1,*.mp2;*.mp3)|*.mp1;*.mp2;*.mp3|Waveform Audio Files (*.wav)|*.wav|Ogg Vorbis Audio Files (*.ogg)|*.ogg|Windows Media Audio Files (*.wma)|*.wma|AAC Audio Files (*.m4a,*.aac)|*.m4a;*.aac|FLAC Audio Files (*.flac)|*.flac" );
  static const wxChar DIR_SEP = '\\';
#elif defined(__LINUX__)
  static const wxString APP_PLATFORM = wxT( "Linux" );
  static const wxString SUPPORTED_FORMATS = wxT( "All Supported Files|*.mp1;*.mp2;*.mp3;*.wav;*.ogg;*.m4a;*.aac;*.flac|MPEG Audio Files (*.mp1,*.mp2;*.mp3)|*.mp1;*.mp2;*.mp3|Waveform Audio Files (*.wav)|*.wav|Ogg Vorbis Audio Files (*.ogg)|*.ogg|AAC Audio Files (*.m4a,*.aac)|*.m4a;*.aac|FLAC Audio Files (*.flac)|*.flac" );
  static const wxChar DIR_SEP = '/';
#elif defined(__DARWIN__)
  static const wxString APP_PLATFORM = wxT( "Mac OS X" );
  static const wxString SUPPORTED_FORMATS = wxT( "All Supported Files|*.mp1;*.mp2;*.mp3;*.wav;*.ogg;*.m4a;*.aac;*.flac|MPEG Audio Files (*.mp1,*.mp2;*.mp3)|*.mp1;*.mp2;*.mp3|Waveform Audio Files (*.wav)|*.wav|Ogg Vorbis Audio Files (*.ogg)|*.ogg|AAC Audio Files (*.m4a,*.aac)|*.m4a;*.aac|FLAC Audio Files (*.flac)|*.flac" );
  static const wxChar DIR_SEP = '/';
#endif

/* Configuration file */
static const wxStandardPaths STANDARD_PATHS;
static const wxString EXECUTABLE_DIR = wxString( STANDARD_PATHS.GetExecutablePath() ).SubString( 0, STANDARD_PATHS.GetExecutablePath().Last( DIR_SEP ) );
static const wxString CONFIG_FILENAME = wxT( "xapl.ini" );

typedef enum
{
	PLFORMAT_FILENAME = 1,
	PLFORMAT_FULLPATH,
	PLFORMAT_CUSTOM
} PlaylistFormat;

typedef enum
{
	PLAYMODE_LOOP_LIST = 1,
	PLAYMODE_LOOP_SONG,
	PLAYMODE_SHUFFLE
} PlayMode;

#endif
