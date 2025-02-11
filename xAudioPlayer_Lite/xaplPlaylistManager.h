#ifndef __XAPL_PLAYLIST_MANAGER_H
#define __XAPL_PLAYLIST_MANAGER_H

#include <wx/wx.h>
#include <wx/file.h>
#include <wx/wfstream.h>
#include "xaplAudioFileEntry.h"
#include "xaplConstants.h"

class xaplPlaylistManager
{
private:
	xaplAudioFileArray plData;
	wxArrayString formattedPlData;

	wxString activePlLocation;
	wxString activePlName;
	wxString currPlCreateDate;

	wxString customFormatStr;
	int playlistFormat;

protected:
	wxString formatSingleEntry( const xaplAudioFileEntry &entry );

public:
	xaplPlaylistManager( const wxString &defaultPlLoc = wxT( "Default.xpl" ), bool loadDefault = false );

	bool LoadPlaylist( const wxString &playlist, int format = PLFORMAT_FILENAME, void (*progressCallback)( int pos, int tot ) = NULL );
	virtual bool SetPlaylistFormat( int format, const wxString &formatStr = wxEmptyString );

	const wxArrayString &AddFile( const wxString &file );
	const wxArrayString &AddFiles( const wxArrayString &fileList );

	const wxArrayString &GetPlaylist( void );
	const wxString &GetActivePlaylistLocation( void ) const;
	const wxString &GetActivePlaylistName( void ) const;
	int GetPlaylistFormat( void ) const;
	const wxString &GetPlaylistFormatString( void ) const;
	unsigned int GetPlaylistItemCount( void ) const;
};

#endif
