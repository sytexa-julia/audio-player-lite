#ifndef __XPL_PLS_PLAYLIST_H
#define __XPL_PLS_PLAYLIST_H

#ifdef __OBJC__	// Objective-C class
	#import <Cocoa/Cocoa.h>
	#import "XplPlaylist.h"

	@interface XplPLSPlaylist : NSObject <XplPlaylist> 
	{}

	+ (BOOL) readData:(NSMutableArray *) data fromFile:(NSString *)file;
	+ (BOOL) writeData:(NSArray *)data toFile:(NSString *)file;
	+ (BOOL) writeBlankPlaylistToFile:(NSString *)file;
	+ (BOOL) appendData:(NSArray *)data toFile:(NSString *)file;
	+ (NSString *) formatExtension;
	+ (NSString *) fullFormatName;

	@end
#else // C++ class
	#include <string>
	#include <vector>
	#include "XplData.h"
	#include "XplPlaylist.h"
	using std::string;
	using std::vector;

	/* Interface for reading and writing PLS playlists */
	class XplPLSPlaylist
		: public XplPlaylist
	{
	private:
		vector<XplData *> *m_plData;
		string m_plFile;
		bool m_loaded;

	public:
		XplPLSPlaylist( void );
		XplPLSPlaylist( const string &plFile );
		virtual ~XplPLSPlaylist( void ) { FreeMemory(); delete m_plData; }
		
		bool ReadPlaylist( void );
		bool ReadPlaylist( const string &file );
		bool WritePlaylist( void );
		bool WritePlaylist( const string &file );
		static bool NewPlaylist( const string &file );
		static bool AppendToPlaylist( const string &file, XplData **data, size_t n );
		void FreeMemory( void );

		void SetData( XplData **data, size_t n );
		void GetData( long index, const string &key = NULL, string *data = NULL );
		void GetRawData( XplData **data );
		bool GetNext( string *data, const string &key = NULL );
		size_t GetCount( void );
		bool KeyIsValid( const string &key );

		void GetFormatExtension( string *ext ) const;
		void GetFormatFullName( string *fullName ) const;
	};
#endif

#endif
