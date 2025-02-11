#ifndef __XPL_M3U_PLAYLIST_H
#define __XPL_M3U_PLAYLIST_H

#ifdef __OBJC__	// Objective-C class
	#import "XplPlaylist.h"
	@interface XplM3UPlaylist : NSObject<XplPlaylist>
	{ }

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

	/* Interface for reading and writing M3U/Extended M3U playlist files */
	class XplM3UPlaylist
		: public XplPlaylist
	{
	private:
		vector<XplData *> *m_plData;
		string m_plFile;
		bool m_loaded;

	public:
		XplM3UPlaylist( void );
		XplM3UPlaylist( const string &plFile );
		virtual ~XplM3UPlaylist( void ) { FreeMemory(); delete m_plData; }
		
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
		bool HasExtXplData( long index );
		size_t GetCount( void );
		bool KeyIsValid( const string &key );

		void GetFormatExtension( string *ext ) const;
		void GetFormatFullName( string *fullName ) const;
	};
#endif

#endif
