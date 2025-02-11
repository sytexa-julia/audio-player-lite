//
//  XplTextPlaylist.m
//  Xapl
//
//  Created by Julia on 6/11/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import "XplTextPlaylist.h"
#import "XplData.h"

@implementation XplTextPlaylist

+ (BOOL) readData:(NSMutableArray *) data fromFile:(NSString *)file
{
	NSString *fileData, *currentLine;
	NSEnumerator *enumerator;
	NSArray *dataLines;
	NSError *error = nil;
	XplData *plData;

	
	// Clear contents of data array
	[data removeAllObjects];
	
	// Attempt to read file
	fileData = [NSString stringWithContentsOfFile:file encoding:NSASCIIStringEncoding error:&error];
	if (fileData == nil)
	{
		NSLog([NSString stringWithFormat:@"%@", error]);
		return NO;
	}
	
	// Normalize line endings to UNIX style
	fileData = [fileData stringByReplacingOccurrencesOfString:@"\r\n" withString:@"\n"];
	
	// Split file into lines
	dataLines = [fileData componentsSeparatedByString:@"\n"];
	enumerator = [dataLines objectEnumerator];	
	while (currentLine = (NSString *)[enumerator nextObject]) 
	{
		// Blank line
		if ( [currentLine length] == 0 ) continue;
		
		// Trim trailing and leading spaces
		currentLine = [currentLine stringByTrimmingCharactersInSet:[NSCharacterSet whitespaceAndNewlineCharacterSet]];
		
		// Blank line after trimming
		if ( [currentLine length] == 0 ) continue;

		// Add the file
		plData = [[XplData alloc] initWithPath:currentLine];
		[data addObject:plData];
	}
	
	return YES;
}

+ (BOOL) writeData:(NSArray *)data toFile:(NSString *)file
{
	NSEnumerator *enumerator;
	NSMutableString *output;
	NSError *error = nil;
	XplData *item;
	id object;
	
	// Write data
	enumerator = [data objectEnumerator];
	while (object = [enumerator nextObject])
	{
		if (![object isKindOfClass:[XplData class]])
			continue;
		
		item = (XplData *)object;
		[output appendFormat:@"%@\n", [item path]];
	}
	
	// Write out the file
	[output writeToFile:file atomically:YES encoding:NSUnicodeStringEncoding error:&error];
	
	if (error != nil)
		return NO;
	
	return YES;
}

+ (BOOL) writeBlankPlaylistToFile:(NSString *)file
{
	NSError *error = nil;
	[@"" writeToFile:file atomically:YES encoding:NSUnicodeStringEncoding error:&error];
	
	if (error != nil)
		return NO;
	
	return YES;
}

+ (BOOL) appendData:(NSArray *)data toFile:(NSString *)file
{
	NSMutableArray *currentData = [[[NSMutableArray alloc] init] autorelease];
	NSEnumerator *enumerator;
	XplData *plData;
	id object;
	
	// Load any current data from the file
	if ( ![XplTextPlaylist readData:currentData fromFile:file] )
		return NO;
	
	// Append data items
	enumerator = [data objectEnumerator];
	while (object = [enumerator nextObject])
	{
		if (![object isKindOfClass:[XplData class]])
			return NO;
		
		plData = (XplData *)object;
		[currentData addObject:plData];
	}
	
	// Write out combined data
	return [XplTextPlaylist writeData:currentData toFile:file];
}

+ (NSString *) formatExtension
{
	return @"txt";
}

+ (NSString *) fullFormatName
{
	return @"Flat File Audio File List";
}

@end
