//
//  XSPlaylistItem.h
//  Xapl
//
//  Created by Julia on 6/1/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>

typedef enum
{
	XSFileName,
	XSFullPath,
	XSExtendedM3UTitle
} XSPlaylistItemFormat;

// Global helper to create a XSPlaylistItem and get its pointer
@class XSPlaylistItem;
XSPlaylistItem* XSCreatePlaylistItem(long anId, NSString* aPath, int aFormat,
									   NSString* m3uTitle, unsigned int m3uLength, 
									   BOOL netStream);

@interface XSPlaylistItem : NSObject
{
	long identifier;
	
	BOOL isNetStream;
	BOOL isOk;
	
	int format;
	NSString* displayText;
	
	NSString* filePath;
	unsigned int m3uLength;
	NSString* m3uTitle;
}

@property(readonly) long identifier;

@property(assign) BOOL isNetStream;
@property(assign) BOOL isOk;

@property(readonly) int format;
@property(retain) NSString* displayText;
@property(retain) NSString* filePath;
@property(assign) unsigned int m3uLength;
@property(retain) NSString* m3uTitle;

- (id) initWithId:(long)anId andFilePath:(NSString*)aFilePath format:(int)aFormat;
- (void) dealloc;

- (void) setFormat:(int)aFormat;

@end
