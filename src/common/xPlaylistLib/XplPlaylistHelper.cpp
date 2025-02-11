#include <string>
#include <algorithm>
#include "XplPlaylistHelper.h"
#include "XplM3UPlaylist.h"
#include "XplPLSPlaylist.h"
#include "XplTextPlaylist.h"

using std::string;
using std::transform;

static string GetFileExtension( const string &file )
{
	size_t pos = file.find_last_of( '.' );
	if ( pos == string::npos )
		return "";

	return file.substr( pos + 1 );
}

void Lower( string *input )
{
	std::transform( input->begin(), input->end(), input->begin(), tolower );
}

XplData **XplPlaylistHelper::AutoReadData( size_t *n, const string &sourceFile )
{
	if ( sourceFile.length() == 0 ) 
		return false;

	string ext = GetFileExtension( sourceFile );
	Lower( &ext );

	if ( ext.compare("m3u") == 0 )
	{
		XplM3UPlaylist m3uPlaylist;
		m3uPlaylist.ReadPlaylist( sourceFile );
		*n = m3uPlaylist.GetCount();
		XplData **data = new XplData*[*n];
		m3uPlaylist.GetRawData( data );
		return data;
	}
	else if ( ext.compare("pls") == 0 )
	{
		XplPLSPlaylist plsPlaylist;
		plsPlaylist.ReadPlaylist( sourceFile );
		*n = plsPlaylist.GetCount();
		XplData **data = new XplData*[*n];
		plsPlaylist.GetRawData( data );
		return data;
	}
	else if ( ext.compare("txt") == 0 )
	{
		XplTextPlaylist txtPlaylist;
		txtPlaylist.ReadPlaylist( sourceFile );
		*n = txtPlaylist.GetCount();
		XplData **data = new XplData*[*n];
		txtPlaylist.GetRawData( data );
		return data;
	}

	return false;
}

bool XplPlaylistHelper::AutoWriteData( XplData **data, size_t n, const string &destFile )
{
	if ( data == NULL || n == 0 || destFile.length() == 0 ) 
		return false;

	string ext = GetFileExtension( destFile );
	Lower( &ext );

	if ( ext.compare("m3u") == 0 )
	{
		XplM3UPlaylist m3uPlaylist;
		m3uPlaylist.SetData( data, n );
		return m3uPlaylist.WritePlaylist( destFile );
	}
	else if ( ext.compare("pls") == 0 )
	{
		XplPLSPlaylist plsPlaylist;
		plsPlaylist.SetData( data, n );
		return plsPlaylist.WritePlaylist( destFile );
	}
	else if ( ext.compare("txt") == 0 )
	{
		XplTextPlaylist txtPlaylist;
		txtPlaylist.SetData( data, n );
		return txtPlaylist.WritePlaylist( destFile );
	}

	return false;
}

bool XplPlaylistHelper::AutoWriteBlankPlaylist( const string &destFile )
{
	if ( destFile.length() == 0 ) 
		return false;

	string ext = GetFileExtension( destFile );
	Lower( &ext );

	if ( ext.compare("m3u") == 0 )
	{
		return XplM3UPlaylist::NewPlaylist( destFile );
	}
	else if ( ext.compare("pls") == 0 )
	{
		return XplPLSPlaylist::NewPlaylist( destFile );
	}
	else if ( ext.compare("txt") == 0 )
	{
		return XplTextPlaylist::NewPlaylist( destFile );
	}
	return false;
}

bool XplPlaylistHelper::AutoAppendData( XplData **data, size_t n, const string &destFile )
{
	if ( data == NULL || n == 0 || destFile.length() == 0 ) 
		return false;

	string ext = GetFileExtension( destFile );
	Lower( &ext );

	if ( ext.compare("m3u") == 0 )
	{
		return XplM3UPlaylist::AppendToPlaylist( destFile, data, n );
	}
	else if ( ext.compare("pls") == 0 )
	{
		return XplPLSPlaylist::AppendToPlaylist( destFile, data, n );
	}
	else if ( ext.compare("txt") == 0 )
	{
		return XplTextPlaylist::AppendToPlaylist( destFile, data, n );
	}
	return false;
}
