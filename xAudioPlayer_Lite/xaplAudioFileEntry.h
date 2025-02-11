#ifndef __XAPL_AUDIO_FILE_ENTRY_H
#define __XAPL_AUDIO_FILE_ENTRY_H

#include <wx/string.h>
#include <wx/dynarray.h>
#include <wx/arrimpl.cpp>

class xaplAudioFileEntry
{
private:
	long index;
	wxString pathToFile;
	wxString artist;
	wxString album;
	wxString title;
	wxString genre;
	wxString label;
	int songNum;

public:
	xaplAudioFileEntry( void );
	xaplAudioFileEntry( long idx, wxString pth, wxString art = wxEmptyString, wxString albm = wxEmptyString, wxString ttl = wxEmptyString, wxString gnre = wxEmptyString, wxString lbl = wxEmptyString, int song = 0 );
	
	bool FileExists( void ) const;

	long GetIndex( void ) const { return index; };
	wxString GetPath( void ) const { return pathToFile; };
	wxString GetArtist( void ) const { return artist; };
	wxString GetAlbum( void ) const { return album; };
	wxString GetTitle( void ) const { return title; };
	wxString GetGenre( void ) const { return genre; };
	wxString GetLabel( void ) const { return label; };
	int GetSongNumber( void ) const { return songNum; };
};

WX_DECLARE_OBJARRAY( xaplAudioFileEntry, xaplAudioFileArray );

#endif
