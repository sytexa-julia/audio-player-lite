#ifndef __XPL_PLAYLIST_HELPER_H
#define __XPL_PLAYLIST_HELPER_H

#ifdef __OBJC__	// Objective-C clas
	@interface XplPlaylistHelper : NSObject
	{ }

	+ (BOOL)autoReadData:(NSMutableArray *)data fromFile:(NSString *)file;
	+ (BOOL)autoWriteData:(NSArray *)data toFile:(NSString *)file;
	+ (BOOL) autoWriteBlankPlaylistToFile:(NSString *)file;
	+ (BOOL) autoAppendData:(NSArray *)data toFile:(NSString *)file;

	@end

#else	// C++ class
	#include <string>
	#include "XplData.h"
	using std::string;

	class XplPlaylistHelper
	{
	public:
		static XplData **AutoReadData( size_t *n, const string &sourceFile );
		static bool AutoWriteData( XplData **data, size_t n, const string &destFile );
		static bool AutoWriteBlankPlaylist( const string &destFile );
		static bool AutoAppendData( XplData **data, size_t n, const string &destFile );
	};
#endif

#endif
