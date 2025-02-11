#include "XplTextPlaylist.h"
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

XplTextPlaylist::XplTextPlaylist( void ) : m_loaded( false )
{
	m_plData = new vector<XplData *>;
}

XplTextPlaylist::XplTextPlaylist( const string &plFile ) : m_plFile( string( plFile ) ), m_loaded( false )
{
	m_plData = new vector<XplData *>;
}
	
bool XplTextPlaylist::ReadPlaylist( void )
{
	if ( m_plFile.empty() ) return false;
	return ReadPlaylist( m_plFile );
}

bool XplTextPlaylist::ReadPlaylist( const string &file )
{
	// Clear old data
	FreeMemory();
	
	// Parse playlist file and load data into the plData array
	ifstream input( file.c_str() );
	if ( !input.is_open() )
		return false;

	string line;
	while ( getline( input, line ) )
	{
		Trim( &line );

		// Blank line
		if ( line.empty() ) continue;

		// Comment line
		if ( line[0] == '#' ) continue;

		// Add the file
		XplData *data = new XplData;
		data->SetPath( line );
		m_plData->push_back( data );
	}

	input.close();
	return true;
}

bool XplTextPlaylist::WritePlaylist( void )
{
	if ( m_plFile.empty() ) return false;
	return WritePlaylist( m_plFile );
}

bool XplTextPlaylist::WritePlaylist( const string &file )
{
	ofstream plFile( file.c_str(), std::ios::trunc );
	if ( plFile.is_open() )
	{
		for ( size_t i = 0; i < m_plData->size(); ++i )
		{
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
bool XplTextPlaylist::NewPlaylist( const string &file )
{
	ofstream plFile( file.c_str(), std::ios::out );
	if ( plFile.is_open() )
	{
		plFile << "" << endl;		
		plFile.close();
		return true;
	}

	return false;
}

bool XplTextPlaylist::AppendToPlaylist( const string &file, XplData **data, size_t n )
{
	try
	{
		XplTextPlaylist pl;
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

void XplTextPlaylist::FreeMemory( void )
{
	for ( vector<XplData *>::iterator it = m_plData->begin(); it != m_plData->end(); it++ )
		delete *it;
	
	m_plData->clear();
	m_loaded = false;
}

void XplTextPlaylist::SetData( XplData **data, size_t n )
{
	if ( n == 0 ) return;

	FreeMemory();
	XplData *d;
	for ( size_t i = 0; i < n; ++i )
	{
		d = static_cast<XplData *>(data[i]);
		m_plData->push_back(d);
	}
}

void XplTextPlaylist::GetData( long index, const string &key, string *data )
{
	if ( !m_loaded ) *data = "";
	
	if ( key.compare( "" ) == 0 || key.compare( "file" ) == 0 )
		data->assign( m_plData->at( index )->GetPath() );
	else
		*data = "";
}

void XplTextPlaylist::GetRawData( XplData **data )
{
	if ( data == NULL ) return;
	for ( size_t i = 0; i < m_plData->size(); ++i )
	{
		data[ i ] = new XplData( m_plData->at( i )->GetPath(),
									m_plData->at( i )->GetTitle(),
									m_plData->at( i )->GetLength() );
	}
}

bool XplTextPlaylist::GetNext( string *data, const string &key )
{
	return false;
}

size_t XplTextPlaylist::GetCount( void )
{
	return m_plData->size();
}

bool XplTextPlaylist::KeyIsValid( const string &key )
{
	return key.compare( "file" ) == 0;
}

void XplTextPlaylist::GetFormatExtension( string *ext ) const
{
	if ( ext != NULL )
		*ext = "txt";
}

void XplTextPlaylist::GetFormatFullName( string *fullName ) const
{
	if ( fullName != NULL )
		*fullName = "Flat File Audio File List";
}
