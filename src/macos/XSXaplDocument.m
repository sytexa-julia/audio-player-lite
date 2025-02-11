//
//  XSXaplDocument.m
//  Xapl
//
//  Created by Julia on 6/5/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import "XSXaplDocument.h"
#import "XSPlaylistItem.h"
#import "XplPlaylistHelper.h"
#import "XplPlaylist.h"
#import "XplData.h"

@implementation XSXaplDocument

- (id)init
{
	if (self = [super init])
	{
		playlistData = [[NSMutableArray alloc] init];
		return self;
	}
	
	return nil;
}

- (XSXaplWindow *)documentWindowController
{
	return documentWindowController;
}

- (NSString *)windowNibName 
{
    // Implement this to return a nib to load OR implement -makeWindowControllers to manually create your controllers.
    return @"XSXaplDocument";
}

- (BOOL)readFromURL:(NSURL *)absoluteURL 
			 ofType:(NSString *)typeName 
			  error:(NSError **)outError
{
	NSMutableArray *dataArray = [[NSMutableArray alloc] init];
	NSEnumerator *enumerator;
	XSPlaylistItem *playlistItem;
	XplData *currentData;
	BOOL result;
	long currentId = 1;
	
	result = [XplPlaylistHelper autoReadData:dataArray fromFile:[absoluteURL path]];
	if (!result)
		return NO;
	
	[playlistData removeAllObjects];
	enumerator = [dataArray objectEnumerator];
	while (currentData = [enumerator nextObject])
	{
		playlistItem = [[XSPlaylistItem alloc] initWithId:currentId++ andFilePath:[currentData path] format:XSFileName];
		[playlistData addObject:playlistItem];
	}
	
	return YES;
}

- (BOOL)writeToURL:(NSURL *)absoluteURL 
			ofType:(NSString *)typeName 
			 error:(NSError **)outError
{
	NSMutableArray *xplData = [[[NSMutableArray alloc] init] autorelease];
	XSPlaylistItem *playlistItem;
	NSEnumerator *enumerator;
	XplData *currentData;
	
	enumerator = [playlistData objectEnumerator];
	while (playlistItem = [enumerator nextObject])
	{
		currentData = [[XplData alloc] initWithPath:[playlistItem filePath]
											  title:[playlistItem m3uTitle]
										  andLength:[[NSNumber numberWithInt:[playlistItem m3uLength]] stringValue]];
		[xplData addObject:currentData];
	}
	
	return [XplPlaylistHelper autoWriteData:xplData toFile:[absoluteURL path]];
}

@end
