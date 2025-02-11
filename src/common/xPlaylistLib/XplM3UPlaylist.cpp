#include "XplM3UPlaylist.h"
#include <sstream>
#include <fstream>

#ifdef _DEBUG
#include <crtdbg.h>
#define DEBUG_NEW new(_NORMAL_BLOCK ,__FILE__, __LINE__)
#else
#define DEBUG_NEW new
#endif
#ifdef _DEBUG
#define new DEBUG_NEW
#endif 

XplM3UPlaylist::XplM3UPlaylist( void ) : m_loaded( false )
{
	m_plData = new vector<XplData *>;
}

XplM3UPlaylist::XplM3UPlaylist( const string &plFile ) : m_plFile( string( plFile ) ), m_loaded( false )
{
	m_plData = new vector<XplData *>;
}
	
bool XplM3UPlaylist::ReadPlaylist( void )
{
	if ( m_plFile.empty() ) return false;
	return ReadPlaylist( m_plFile );
}

bool XplM3UPlaylist::ReadPlaylist( const string &file )
{
	// Clear old data
	FreeMemory();
	
	// Parse playlist file and load data into the plData array
	ifstream input( file.c_str() );
	if ( !input.is_open() )
		return false;

	string line, extInfo, display;
	bool ext;

	// Check for extended M3U (really, this is just to conform to the spec...
	// if one wanted, one could force the 'ext' var to true and the EXTINF would
	// be read just fine where it exists
	getline( input, line );
	if  ( line.compare( "#EXTM3U" ) == 0 )
		ext = true;

	while ( getline( input, line ) )
	{
		Trim( &line );

		// Blank line
		if ( line.empty() ) continue;

		// Extended M3U info line
		if ( ext )
		{
			size_t pos = line.find( "#EXTINF:" );
			if ( pos != string::npos )
			{
				extInfo = line.substr( pos, line.length() - pos );
				continue;
			}
		}
		// Comment line
		else if ( line[0] == '#' )
			continue;

		if ( !extInfo.empty() )
		{
			XplData *data = new XplData;
			
			// Parse the EXTINF
			size_t c = extInfo.find( "," );
			if ( c != string::npos && c > 8 )
			{
				data->SetLength( extInfo.substr( 8, c - 8 ) );
				data->SetTitle( extInfo.substr( c + 1, extInfo.length() - c - 1 ) );
			}

			// Previous EXTINF goes with this file path
			data->SetPath( line );
			m_plData->push_back( data );

			// Reset
			extInfo = "";
		}
		else
		{			
			// No EXTINF, just add the file
			XplData *data = new XplData;
			data->SetPath( line );
			m_plData->push_back( data );
		}
	}

	input.close();
	return true;
}

bool XplM3UPlaylist::WritePlaylist( void )
{
	if ( m_plFile.empty() ) return false;
	return WritePlaylist( m_plFile );
}

bool XplM3UPlaylist::WritePlaylist( const string &file )
{
	ofstream plFile( file.c_str(), std::ios::trunc );
	if ( plFile.is_open() )
	{
		// Write EXTM3U flag
		stringstream extInfId;
		extInfId << "#EXTM3U" << endl;
		plFile.write( extInfId.str().c_str(), extInfId.str().length() );

		for ( size_t i = 0; i < m_plData->size(); ++i )
		{
			// Fetch the EXTINF data
			string title = m_plData->at( i )->GetTitle();
			string seconds = m_plData->at( i )->GetLength();
			if ( seconds.empty() ) seconds = "-1";
			stringstream extinf;
			extinf << "#EXTINF:" << seconds << "," << title << endl;

			// Add the EXTINF line, if there is data for it
			if ( !title.empty())
				plFile.write( extinf.str().c_str(), extinf.str().length() );

			// Add the path
			stringstream file;
			file << m_plData->at( i )->GetPath() << endl;
			plFile.write( file.str().c_str(), file.str().length() );
		}

		plFile.close();
		return true;
	}

	return false;
}

// Create a blank playlist
bool XplM3UPlaylist::NewPlaylist( const string &file )
{
	ofstream plFile( file.c_str(), std::ios::out );
	if ( plFile.is_open() )
	{
		// Write EXTM3U flag
		plFile << "#EXTM3U" << endl;		
		plFile.close();
		return true;
	}

	return false;
}

bool XplM3UPlaylist::AppendToPlaylist( const string &file, XplData **data, size_t n )
{
	try
	{
		XplM3UPlaylist pl;
		pl.ReadPlaylist( file );

		for ( size_t i = 0; i < n; ++i )
			pl.m_plData->push_back( (XplData *) data[i]);

		pl.WritePlaylist( file );
		pl.FreeMemory();
	}
	catch (...)
	{
		return false;
	}

	return true;
}

void XplM3UPlaylist::FreeMemory( void )
{
	for ( vector<XplData *>::iterator it = m_plData->begin(); it != m_plData->end(); it++ )
		delete *it;
	
	m_plData->clear();
	m_loaded = false;
}

void XplM3UPlaylist::SetData( XplData **data, size_t n )
{
	if ( n == 0 ) return;

	FreeMemory();
	XplData *m3u;
	for ( size_t i = 0; i < n; ++i )
	{
		m3u = static_cast<XplData *>(data[i]);
		m_plData->push_back(m3u);
	}
}

void XplM3UPlaylist::GetData( long index, const string &key, string *data )
{
	if ( !m_loaded ) *data = "";
	
	if ( key.compare( "" ) == 0 || key.compare( "file" ) == 0 )
		data->assign( m_plData->at( index )->GetPath() );
	else if ( key.compare( "length" ) == 0 )
		*data = m_plData->at( index )->GetLength();
	else if ( key.compare( "title" ) == 0 )
		*data = m_plData->at( index )->GetTitle();
	else
		*data = "";
}

void XplM3UPlaylist::GetRawData( XplData **data )
{
	if ( data == NULL ) return;
	for ( size_t i = 0; i < m_plData->size(); ++i )
	{
		data[ i ] = new XplData( m_plData->at( i )->GetPath(),
									m_plData->at( i )->GetTitle(),
									m_plData->at( i )->GetLength() );
	}
}

bool XplM3UPlaylist::GetNext( string *data, const string &key )
{
	return false;
}

bool XplM3UPlaylist::HasExtXplData( long index )
{
	return !m_plData->at( index )->GetTitle().empty() || !m_plData->at( index )->GetLength().empty();
}

size_t XplM3UPlaylist::GetCount( void )
{
	return m_plData->size();
}

bool XplM3UPlaylist::KeyIsValid( const string &key )
{
	return key.compare( "file" ) == 0 ||
		key.compare( "length" ) == 0 ||
		key.compare( "title" ) == 0;
}

void XplM3UPlaylist::GetFormatExtension( string *ext ) const
{
	if ( ext != NULL )
		*ext = "m3u";
}

void XplM3UPlaylist::GetFormatFullName( string *fullName ) const
{
	if ( fullName != NULL )
		*fullName = "Moving Picture Experts Group Audio Layer 3 Uniform Resource Locator";
}
