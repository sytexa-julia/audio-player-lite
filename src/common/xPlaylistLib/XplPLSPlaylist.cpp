#include "XplPLSPlaylist.h"
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

XplPLSPlaylist::XplPLSPlaylist( void ) : m_loaded( false )
{
	m_plData = new vector<XplData *>;
}

XplPLSPlaylist::XplPLSPlaylist( const string &plFile ) : m_plFile( string( plFile ) ), m_loaded( false )
{
	m_plData = new vector<XplData *>;
}
	
bool XplPLSPlaylist::ReadPlaylist( void )
{
	if ( m_plFile.empty() ) return false;
	return ReadPlaylist( m_plFile );
}

bool XplPLSPlaylist::ReadPlaylist( const string &file )
{
	// Clear old data
	FreeMemory();
	
	// Parse playlist file and load data into the plData array
	ifstream input( file.c_str() );
	if ( !input.is_open() )
		return false;

	long numberOfEntries = -1, currentEntry = 1;
	bool playlistFlag = false, isVersion2 = false;
	string line, lowerLine, filePath, title, length;
	stringstream buffer;
	while ( getline( input, line ) )
	{
		// Trim trailing and leading spaces
		Trim( &line );

		// Convert to lower case
		lowerLine = line;
		Lower( &lowerLine );

		// Blank line
		if ( lowerLine.empty() ) continue;

		// Comment line
		if ( line[0] == ';' ) continue;

		// Header information
		if ( !playlistFlag && lowerLine.compare("[playlist]") == 0 )
		{
			playlistFlag = true;
			continue;
		}
		else if ( numberOfEntries < 0 && lowerLine.substr(0, 15).compare("numberofentries") == 0 )
		{
			numberOfEntries = atoi(lowerLine.substr(16).c_str());
			continue;
		}
		// Playlist items
		else if ( playlistFlag && currentEntry <= numberOfEntries )
		{
			// File path
			if ( filePath.length() == 0 && lowerLine.substr(0, 4).compare("file") == 0 )
			{
				// Is the numbering correct?
				buffer.str("");
				buffer << currentEntry;
				string test = lowerLine.substr(4, lowerLine.find("=") - 4 );
				if ( lowerLine.substr(4, lowerLine.find("=") - 4 ).compare( buffer.str() ) != 0 )
					continue;
				
				// Store file path
				buffer.str("");
				buffer << "file" << currentEntry << "=";

				if ( lowerLine.find( buffer.str() ) != string::npos )
					filePath = line.substr( buffer.str().length() );
			}
			// Track title, optional
			if ( title.length() == 0  && lowerLine.substr(0, 5).compare("title") == 0 )
			{
				// Is the numbering correct?
				buffer.str("");
				buffer << currentEntry;
				if ( lowerLine.substr(5, lowerLine.find("=") - 5 ).compare( buffer.str() ) != 0 )
					continue;

				// Store track title
				buffer.str("");
				buffer << "title" << currentEntry << "=";

				if ( lowerLine.find( buffer.str() ) != string::npos )
					title = line.substr( buffer.str().length() );
			}
			// Track length
			if ( length.length() == 0 && lowerLine.substr(0, 6).compare("length") == 0 )
			{
				// Is the numbering correct?
				buffer.str("");
				buffer << currentEntry;
				if ( lowerLine.substr(6, lowerLine.find("=") - 6 ).compare( buffer.str() ) != 0 )
					continue;

				// Store the track length
				buffer.str("");
				buffer << "length" << currentEntry << "=";

				if ( lowerLine.find( buffer.str() ) != string::npos )
					length = line.substr( buffer.str().length() );
			}
			
			// Have all required components
			if ( filePath.length() != 0 && length.length() != 0 && lowerLine.substr( 0, 5 ).compare("title") != 0 )
			{
				XplData *data = new XplData( filePath, title, length );
				m_plData->push_back( data );
				++currentEntry;
				filePath.clear();
				title.clear();
				length.clear();
			}
		}
		// Footer
		else if ( playlistFlag && currentEntry > numberOfEntries )
		{
			if ( lowerLine.compare("version=2") == 0 )
			{
				isVersion2 = true;
				break;
			}
		}
	}

	input.close();
	return true;
}

bool XplPLSPlaylist::WritePlaylist( void )
{
	if ( m_plFile.empty() ) return false;
	return WritePlaylist( m_plFile );
}

bool XplPLSPlaylist::WritePlaylist( const string &file )
{
	ofstream plFile( file.c_str(), std::ios::trunc );
	if ( plFile.is_open() )
	{
		// Header
		stringstream header;
		header << "[Playlist]" << endl << "NumberOfEntries=" << m_plData->size() << endl;
		plFile.write( header.str().c_str(), header.str().length() );

		for ( size_t i = 0; i < m_plData->size(); ++i )
		{
			stringstream itemData;

			// File path
			itemData << "File" << (i + 1) << "=" << m_plData->at( i )->GetPath() << endl;
			
			// Title (if applicable)
			if ( m_plData->at( i )->GetTitle().length() > 0 )
				itemData << "Title" << (i + 1) << "=" << m_plData->at( i )->GetTitle() << endl;

			// Length (required, -1 inserted if none specified)
			if ( m_plData->at( i )->GetLength().length() > 0 )
				itemData << "Length" << (i + 1) << "=" << m_plData->at( i )->GetLength() << endl;
			else
				itemData << "Length" << (i + 1) << "=-1" << endl;

			// Write it out
			plFile.write( itemData.str().c_str(), itemData.str().length() );
		}

		// Footer
		stringstream footer;
		footer << endl << "Version=2";
		plFile.write( footer.str().c_str(), footer.str().length() );

		// Close file
		plFile.close();
		return true;
	}

	return false;
}

// Create a blank playlist
bool XplPLSPlaylist::NewPlaylist( const string &file )
{
	ofstream plFile( file.c_str(), std::ios::out );
	if ( plFile.is_open() )
	{
		// Header
		stringstream header;
		header << "[Playlist]" << endl << "NumberOfEntries=0" << endl;
		plFile.write( header.str().c_str(), header.str().length() );

		// Footer
		stringstream footer;
		footer << endl << "Version=2";
		plFile.write( footer.str().c_str(), footer.str().length() );

		plFile.close();
		return true;
	}

	return false;
}

bool XplPLSPlaylist::AppendToPlaylist( const string &file, XplData **data, size_t n )
{
	try
	{
		XplPLSPlaylist pl;
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

void XplPLSPlaylist::FreeMemory( void )
{
	for ( vector<XplData *>::iterator it = m_plData->begin(); it != m_plData->end(); it++ )
		delete *it;
	
	m_plData->clear();
	m_loaded = false;
}

void XplPLSPlaylist::SetData( XplData **data, size_t n )
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

void XplPLSPlaylist::GetData( long index, const string &key, string *data )
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

void XplPLSPlaylist::GetRawData( XplData **data )
{
	if ( data == NULL ) return;
	for ( size_t i = 0; i < m_plData->size(); ++i )
	{
		data[ i ] = new XplData( m_plData->at( i )->GetPath(),
									m_plData->at( i )->GetTitle(),
									m_plData->at( i )->GetLength() );
	}
}

bool XplPLSPlaylist::GetNext( string *data, const string &key )
{
	return false;
}

size_t XplPLSPlaylist::GetCount( void )
{
	return m_plData->size();
}

bool XplPLSPlaylist::KeyIsValid( const string &key )
{
	return key.compare( "file" ) == 0 ||
		key.compare( "length" ) == 0 ||
		key.compare( "title" ) == 0;
}

void XplPLSPlaylist::GetFormatExtension( string *ext ) const
{
	if ( ext != NULL )
		*ext = "pls";
}

void XplPLSPlaylist::GetFormatFullName( string *fullName ) const
{
	if ( fullName != NULL )
		*fullName = "Generic Playlist File";
}
