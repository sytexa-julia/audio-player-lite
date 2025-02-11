//
//  XplM3UPlaylist.m
//  Xapl
//
//  Created by Julia on 5/30/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import "XplM3UPlaylist.h"
#import "XplData.h"

@implementation XplM3UPlaylist

+ (BOOL) readData:(NSMutableArray *) data fromFile:(NSString *)file
{
	NSString *fileData, *currentLine, *extInfo = nil;
	NSEnumerator *enumerator;
	NSArray *dataLines;
	NSError *error = nil;
	NSRange range;
	XplData *m3uData;
	BOOL ext = NO;
	
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
	
	// Check for extended M3U (really, this is just to conform to the spec...
	// if one wanted, one could force the 'ext' var to true and the EXTINF would
	// be read just fine where it exists
	currentLine = [dataLines objectAtIndex:0];
	if ( [currentLine compare:@"#EXTM3U"] == NSOrderedSame )
		ext = YES;
	
	enumerator = [dataLines objectEnumerator];	
	if (ext) [enumerator nextObject]; // skip first line?
	while (currentLine = (NSString *)[enumerator nextObject]) 
	{
		// Blank line
		if ( [currentLine length] == 0 ) continue;
		
		// Trim trailing and leading spaces
		currentLine = [currentLine stringByTrimmingCharactersInSet:[NSCharacterSet whitespaceAndNewlineCharacterSet]];
		
		// Blank line after trimming
		if ( [currentLine length] == 0 ) continue;
		
		// Extended M3U info line
		if ( ext )
		{
			range = [currentLine rangeOfString:@"#EXTINF:"];
			if ( range.location != NSNotFound )
			{
				extInfo = [[currentLine substringFromIndex:range.location] stringByTrimmingCharactersInSet:[NSCharacterSet whitespaceAndNewlineCharacterSet]];
				continue;
			}
		}
		// Comment line
		else if ( [currentLine characterAtIndex:0] == '#' )
			continue;
		
		if ( extInfo != nil && [extInfo length] != 0 )
		{
			m3uData = [[XplData alloc] init];
			
			// Parse the EXTINF
			range = [extInfo rangeOfString:@","];
			if ( range.location != NSNotFound && range.location > 8 )
			{
				[m3uData setLength:[extInfo substringWithRange:NSMakeRange(8, range.location - 8)]];
				[m3uData setTitle:[extInfo substringWithRange:NSMakeRange(range.location, [extInfo length] - range.location - 1)]];
			}
			
			// Previous EXTINF goes with this file path
			[m3uData setPath:currentLine];
			[data addObject:m3uData];
			
			// Reset
			extInfo = @"";
		}
		else
		{			
			// No EXTINF, just add the file
			m3uData = [[XplData alloc] initWithPath:currentLine];
			[data addObject:m3uData];
		}
	}
	
	return YES;
}

+ (BOOL) writeData:(NSArray *)data toFile:(NSString *)file
{
	NSEnumerator *enumerator;
	NSMutableString *output = [[[NSMutableString alloc] init] autorelease];
	NSString *seconds;
	NSError *error = nil;
	XplData *item;
	id object;

	// Write EXTM3U flag
	[output appendString:@"EXTM3U\n"];
	
	// Write data
	enumerator = [data objectEnumerator];
	while (object = [enumerator nextObject])
	{
		if (![object isKindOfClass:[XplData class]])
			return NO;
		
		item = (XplData *)object;
		if (item == nil || [[item length] length] == 0)
			seconds = @"-1";
		else
			seconds = [item length];
		
		// No title, so don't write EXTINF
		if ([item title] == nil || [[item title] length] == 0)
		{
			[output appendFormat:@"%@\n", [item path]];
		}
		// Title is valid, write EXTINF
		else
		{
			[output appendFormat:@"#EXTINF:%@,%@\n", seconds, [item title]];
			[output appendFormat:@"%@\n", [item path]];
		}
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
	[@"#EXTM3U\n" writeToFile:file atomically:YES encoding:NSUnicodeStringEncoding error:&error];
	
	if (error != nil)
		return NO;
	
	return YES;
}

+ (BOOL) appendData:(NSArray *)data toFile:(NSString *)file
{
	NSMutableArray *currentData = [[NSMutableArray alloc] init];
	NSEnumerator *enumerator;
	XplData *m3uData;
	id object;
	
	// Load any current data from the file
	if ( ![XplM3UPlaylist readData:currentData fromFile:file] )
		return NO;
	
	// Append data items
	enumerator = [data objectEnumerator];
	while (object = [enumerator nextObject])
	{
		if (![object isKindOfClass:[XplData class]])
			return NO;
		
		m3uData = (XplData *)object;
		[currentData addObject:m3uData];
	}
	
	// Write out combined data
	return [XplM3UPlaylist writeData:currentData toFile:file];
}

+ (NSString *) formatExtension
{
	return @"m3u";
}

+ (NSString *) fullFormatName
{
	return @"Moving Picture Experts Group Audio Layer 3 Uniform Resource Locator";
}

@end
