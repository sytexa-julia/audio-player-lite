#ifndef __XPL_DATA_H
#define __XPL_DATA_H

#ifdef __OBJC__	// Objective-C class
	@interface XplData : NSObject
	{
		NSString *path;
		NSString *length;
		NSString *title;
		NSString *comment;
	}

	- (id) init;
	- (id) initWithPath:(NSString*)p;
	- (id) initWithPath:(NSString *)p andTitle:(NSString *)t;
	- (id) initWithPath:(NSString *)p title:(NSString *)t andLength:(NSString *)l;
	- (id) initWithPath:(NSString *)p title:(NSString *)t length:(NSString *)l andComment:(NSString *)c;
	- (void) dealloc;

	- (NSString *) path;
	- (void) setPath:(NSString *)p;
	- (NSString *)length;
	- (void) setLength:(NSString *)l;
	- (NSString *)title;
	- (void) setTitle:(NSString *)t;
	- (NSString *)comment;
	- (void) setComment:(NSString *)c;
	@end
#else	// C++ class
	#include <string>
	using namespace std;

	class XplData
	{
	protected:
		string path;
		string title;
		string length;
		string comment;

	public:
		XplData( string pth ) : path( pth ) {}
		XplData( string pth, string ttl ) : path( pth ), title( ttl ) {}
		XplData( string pth, string ttl, string len ) : path( pth ), title( ttl ), length( len ) {}
		XplData( string pth, string ttl, string len, string cmnt ) : path( pth ), title( ttl ), length( len ), comment( cmnt ) {}
		XplData( const XplData &d ) { *this = d; }
		XplData( void ) {}
		virtual ~XplData( void ) { };
		XplData &operator=( const XplData &d ) { path = d.path; title = d.title; length = d.length; comment = d.comment; return *this; }

		const string &GetPath( void ) const { return path; }
		const string &GetTitle( void ) const { return title; }
		const string &GetLength( void ) const { return length; }
		const string &GetComment( void ) const { return comment; }
		void SetPath( const string &pth ) { path = pth; }
		void SetTitle( const string &ttl ) { title = ttl; }
		void SetLength( const string &len ) { length = len; }
		void SetComment( const string &cmnt ) { comment = cmnt; }
	};
#endif

#endif
