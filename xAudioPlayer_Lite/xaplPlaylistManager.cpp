#include <wx/dir.h>
#include "xaplPlaylistManager.h"

xaplPlaylistManager::xaplPlaylistManager( const wxString &defaultPlLoc, bool loadDefault ) : activePlLocation( defaultPlLoc ), activePlName( wxEmptyString ), currPlCreateDate( wxEmptyString ), customFormatStr( wxEmptyString )
{
	// Make sure the Default playlist exists
	if ( !wxFile::Exists( wxT("Default.xpl") ) )
	{
		wxFile file;
		if ( !file.Create( wxT("Default.xpl") ) )
		{
			wxMessageBox( wxT("ERROR: Default playlist does not exist and could not be created.") );
		}
	}

	if ( loadDefault )
	{
		if ( defaultPlLoc.Len() <= 0 )
			LoadPlaylist( wxT("Default.xpl") );
		else
			LoadPlaylist( defaultPlLoc );
	}
}

wxString xaplPlaylistManager::formatSingleEntry( const xaplAudioFileEntry &entry )
{
	wxString retVal = wxEmptyString;

	switch ( playlistFormat )
	{
	case PLFORMAT_FULLPATH:
		{
			retVal << entry.GetPath();
		}
		break;

	case PLFORMAT_CUSTOM:
		{
			retVal = wxString( customFormatStr );
			retVal.Replace( wxT("%ARTIST%"), entry.GetArtist() );
			retVal.Replace( wxT("%ALBUM%"), entry.GetAlbum() );
			retVal.Replace( wxT("%TITLE%"), entry.GetTitle() );
			retVal.Replace( wxT("%SONGNUM%"), wxString( wxT("") ) << entry.GetSongNumber() );
			retVal.Replace( wxT("%LABEL%"), entry.GetLabel() );
			retVal.Replace( wxT("%GENRE%"), entry.GetGenre() );
		}
		break;

	case PLFORMAT_FILENAME:
	default:
		{
			retVal = ::wxPathOnly( entry.GetPath() );
		}
		break;
	}

	return retVal;
}

bool xaplPlaylistManager::LoadPlaylist( const wxString &playlist, int format, void (*progressCallback)( int pos, int tot ) )
{
	activePlLocation = playlist;
	
	// Parse playlist file and load data into the plData array
	wxFFile input( playlist.c_str() );
	wxString fileContents;

	// Read entire file
	if ( !input.IsOpened() )
		return false;

	input.ReadAll( &fileContents );
	input.Close();

	fileContents.Replace( wxT("\r"), wxT("") );
	fileContents.Replace( wxT("\n"), wxT("") );

	// Make sure the file is approximately correctly formatted
	if ( fileContents.SubString( 0, 7 ) != wxT("XPLSTART") || fileContents.SubString( fileContents.Len() - 6, fileContents.Len() ) != wxT("XPLEND") )
	{
		return false;
	}

	// Read and store basic playlist info
	wxString plInfo = fileContents.SubString( 9, fileContents.Index( wxT("?") ) );
	activePlName = plInfo.SubString( 0, plInfo.Index( wxT("|") ) - 1 );
	plInfo = plInfo.SubString( plInfo.Index( wxT("|") ) + 1, plInfo.Len() );

	currPlCreateDate = plInfo.SubString( plInfo.Index( activePlName ) + 1, plInfo.Index( wxT("|") ) - 1 );
	plInfo = plInfo.SubString( plInfo.Index( wxT("|") ) + 1, plInfo.Len() );
	plInfo = plInfo.SubString( 0, plInfo.Index( wxT("?") ) - 1 );

	long currPlFileCount = 100;
	int count = 0;
	if ( !plInfo.ToLong( &currPlFileCount ) )
		return false;

	// Parse the file data in xaplAudioFileEntry objects
	plData.Clear();
	plData.Alloc( currPlFileCount );

	fileContents = fileContents.SubString( fileContents.Index( wxT("?") ) + 1, fileContents.Index( wxT("XPLEND") ) - 1 );
	wxString line = fileContents.SubString( 0, fileContents.Index( wxT("?") ) );

	while ( line.Len() > 0 )
	{
		long idx = 0;
		wxString temp = line.SubString( 0, line.Index( wxT("|") ) - 1 );
		line = line.SubString( line.Index( wxT("|") ) + 1, line.Len() );

		if ( !temp.ToLong( &idx ) )
			return false;

		wxString fullPath = line.SubString( 0, line.Index( wxT("|") ) - 1 );
		line = line.SubString( line.Index( wxT("|") ) + 1, line.Len() );

		wxString artist = line.SubString( 0, line.Index( wxT("|") ) - 1 );
		line = line.SubString( line.Index( wxT("|") ) + 1, line.Len() );

		wxString album = line.SubString( 0, line.Index( wxT("|") ) - 1 );
		line = line.SubString( line.Index( wxT("|") ) + 1, line.Len() );

		wxString title = line.SubString( 0, line.Index( wxT("|") ) - 1 );
		line = line.SubString( line.Index( wxT("|") ) + 1, line.Len() );
		
		long songNum = 0;
		temp = line.SubString( 0, line.Index(wxT("|")) - 1 );
		line = line.SubString( line.Index(wxT("|")) + 1, line.Len() );

		if ( temp.Len() > 0 && !temp.ToLong( &songNum ) )
			return false;

		wxString genre = line.SubString( 0, line.Index(wxT("|")) - 1 );
		line = line.SubString( line.Index( wxT("|") ) + 1, line.Len() );

		wxString label = line.SubString( 0, line.Index(wxT("?")) - 1 );

		// Create entry and add it to the list
		xaplAudioFileEntry entry( idx, fullPath, artist, album, title, genre, label, songNum );
		plData.Add( entry );

		// Get next line...
		fileContents = fileContents.SubString( fileContents.Index(wxT("?")) + 1, fileContents.Len() );
		line = fileContents.SubString( 0, fileContents.Index(wxT("?")) + 1 );

		// Send message to callback function if it's not NULL
		if ( progressCallback != NULL )
			progressCallback( count++, currPlFileCount );
	}

	SetPlaylistFormat( format );
	return true;
}

bool xaplPlaylistManager::SetPlaylistFormat( int format, const wxString &formatStr )
{
	if ( playlistFormat == format || format > 3 || format < 0 )
		return false;

	playlistFormat = format;

	if ( playlistFormat == PLFORMAT_CUSTOM && formatStr.Len() > 0 )
		customFormatStr = formatStr;

	// Rebuild the playlist
	formattedPlData.Clear();
	formattedPlData.Alloc( plData.Count() );

	for ( unsigned int i = 0; i < plData.Count(); ++i )
	{
		formattedPlData.Add( formatSingleEntry( plData.Item(i) ) );
	}

	return true;
}
const wxArrayString &xaplPlaylistManager::AddFile( const wxString &file )
{
	// If it is a directory, add all supported music files within the directory
	if ( wxDir::Exists( file ) )
	{

	}
	// Otherwise, make sure the file exists and is a supported type, and add it
	else if ( wxFile::Exists( file.c_str() ) )
	{
		xaplAudioFileEntry entry( (long) plData.Count(), file );
		plData.Add( entry );
		formattedPlData.Add( formatSingleEntry(entry) );
	}

	return GetPlaylist();
}

const wxArrayString &xaplPlaylistManager::AddFiles( const wxArrayString &fileList )
{
	// Scan the file list, add to active XPL file and plData collections.
	// Return wxArrayString of items added to formattedPlData

	return GetPlaylist();
}

const wxArrayString &xaplPlaylistManager::GetPlaylist( void )
{
	return formattedPlData;
}

const wxString &xaplPlaylistManager::GetActivePlaylistLocation( void ) const
{
	return activePlLocation;
}

const wxString &xaplPlaylistManager::GetActivePlaylistName( void ) const
{
	return activePlName;
}

int xaplPlaylistManager::GetPlaylistFormat( void ) const
{
	return playlistFormat;
}

const wxString &xaplPlaylistManager::GetPlaylistFormatString( void ) const
{
	return customFormatStr;
}

unsigned int xaplPlaylistManager::GetPlaylistItemCount( void ) const
{
	return (unsigned int) plData.Count();
}
