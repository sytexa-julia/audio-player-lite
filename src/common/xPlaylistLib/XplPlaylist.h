#ifndef __X_PLAYLIST_H
#define __X_PLAYLIST_H

#ifdef __OBJC__	// Objective-C clas
	@protocol XplPlaylist

	+ (BOOL) readData:(NSMutableArray *) data fromFile:(NSString *)file;
	+ (BOOL) writeData:(NSArray *)data toFile:(NSString *)file;
	+ (BOOL) writeBlankPlaylistToFile:(NSString *)file;
	+ (BOOL) appendData:(NSArray *)data toFile:(NSString *)file;
	+ (NSString *) formatExtension;
	+ (NSString *) fullFormatName;

	@end
#else	// C++ class
	#include <string>
	#include <algorithm>
	#include "XplData.h"
	using namespace std;

	class XplPlaylist
	{
	public:
		virtual ~XplPlaylist( void ) {};

		virtual bool ReadPlaylist( void ) = 0;
		virtual bool ReadPlaylist( const string &file ) = 0;
		virtual bool WritePlaylist( void ) = 0;
		virtual bool WritePlaylist( const string &file ) = 0;
		static bool NewPlaylist( const string &file );
		static bool AppendToPlaylist( const string &file, XplData **data, size_t n ) { return false; };
		virtual void FreeMemory( void ) = 0;

		virtual void SetData( XplData **data, size_t n ) = 0;
		virtual void GetData( long index, const string &key = NULL, string *data = NULL ) = 0;
		virtual void GetRawData( XplData **data ) = 0;
		virtual bool GetNext( string *data, const string &key = NULL ) = 0;
		virtual size_t GetCount( void ) = 0;
		virtual bool KeyIsValid( const string &key ) = 0;

		virtual void GetFormatExtension( string *ext ) const = 0;
		virtual void GetFormatFullName( string *fullName ) const = 0;

		// Trim trailing and leading spaces from a string
		void Trim( string *input )
		{
			if ( input != NULL )
			{
				size_t startpos = input->find_first_not_of( " \t" );
				size_t endpos = input->find_last_not_of( " \t" );
			   
				 // if all spaces or empty return an empty string  
				 if ( ( string::npos == startpos ) || ( string::npos == endpos) ) 
					 *input = "";   
				 else  
					 *input = input->substr( startpos, endpos-startpos + 1 );
			}
		}

		// Convert a string to lower case
		void Lower( string *input )
		{
			std::transform( input->begin(), input->end(), input->begin(), tolower );
		}
	};
#endif

#endif
