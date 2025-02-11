//
//  XplPlsPlaylist.m
//  Xapl
//
//  Created by Julia on 5/30/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import "XplPLSPlaylist.h"
#import "XplData.h"

@implementation XplPLSPlaylist

+ (BOOL) readData:(NSMutableArray *) data fromFile:(NSString *)file
{
	int numberOfEntries = -1, currentEntry = 1;
	BOOL playlistFlag = NO, isVersion2 = NO;
	NSString *fileData, *currentLine, *lowerLine, *comp, *dataFinder, *filePath, *title, *length;
	NSArray *dataLines;
	NSEnumerator *enumerator;
	NSError *error = nil;
	XplData *plsData;
	
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
		
		// Lower case copy
		lowerLine = [currentLine lowercaseString];
		
		// Header information
		if ( !playlistFlag && [lowerLine compare:@"[playlist]"] == 0 )
		{
			playlistFlag = true;
			continue;
		}
		else if ( numberOfEntries < 0 && [[lowerLine substringToIndex:15] compare:@"numberofentries"] == NSOrderedSame )
		{
			numberOfEntries = [[lowerLine substringFromIndex:16] integerValue];
			continue;
		}
		// Playlist items
		else if ( playlistFlag && currentEntry <= numberOfEntries )
		{
			// File path
			if ( [filePath length] == 0 )
			{
				// Is the numbering correct?
				comp = [lowerLine substringWithRange:NSMakeRange(4, [lowerLine rangeOfString:@"="].location - 4)];
				if ( [comp compare:[NSString stringWithFormat:@"%u", currentEntry]] != NSOrderedSame )
					continue;
				
				// Store file path
				dataFinder = [NSString stringWithFormat:@"file%u=", currentEntry];				
				if ( [lowerLine rangeOfString:file].location != NSNotFound )
					filePath = [currentLine substringFromIndex:[dataFinder length]];
			}
			// Track title, optional
			else if ( [title length] == 0 )
			{
				// Is the numbering correct?
				comp = [lowerLine substringWithRange:NSMakeRange(5, [lowerLine rangeOfString:@"="].location - 5)];
				if ( [comp compare:[NSString stringWithFormat:@"%u", currentEntry]] != NSOrderedSame )
					continue;
				
				// Store track title
				dataFinder = [NSString stringWithFormat:@"title%u=", currentEntry];				
				if ( [lowerLine rangeOfString:file].location != NSNotFound )
					title = [currentLine substringFromIndex:[dataFinder length]];
			}
			// Track length
			else if ( [length length] == 0 )
			{
				// Is the numbering correct?
				comp = [lowerLine substringWithRange:NSMakeRange(6, [lowerLine rangeOfString:@"="].location - 6)];
				if ( [comp compare:[NSString stringWithFormat:@"%u", currentEntry]] != NSOrderedSame )
					continue;
				
				// Store track title
				dataFinder = [NSString stringWithFormat:@"length%u=", currentEntry];				
				if ( [lowerLine rangeOfString:file].location != NSNotFound )
					length = [currentLine substringFromIndex:[dataFinder length]];
			}
			
			// Have all required components
			if ( [filePath length] != 0 && [length length] != 0 && [[lowerLine substringToIndex:5] compare:@"title"] != NSOrderedSame )
			{
				plsData = [[XplData alloc] initWithPath:filePath];
				[plsData setLength:length];
				if ([title length] != 0)
					[plsData setTitle:title];
				
				[data addObject:plsData];
				filePath  = @"";
				title = @"";
				length = @"";
			}
		}
		// Footer
		else if ( playlistFlag && currentEntry > numberOfEntries )
		{
			if ( [lowerLine compare:@"version=2"] == NSOrderedSame )
			{
				isVersion2 = YES;
				break;
			}
		}
	}
	
	return YES;
}

+ (BOOL) writeData:(NSArray *)data toFile:(NSString *)file
{
	NSEnumerator *enumerator;
	NSMutableString *output = [[[NSMutableString alloc] init] autorelease];
	NSError *error = nil;
	XplData *item;
	int counter = 1;
	id object;
	
	// Header
	[output appendString:@"[Playlist]\n"];
	[output appendFormat:@"NumberOfEntries=%u\n", [data count]];
	
	// Write data
	enumerator = [data objectEnumerator];
	while (object = [enumerator nextObject])
	{
		if (![object isKindOfClass:[XplData class]])
			return NO;
		
		item = (XplData *)object;
		
		// File path
		[output appendFormat:@"File%u=%@\n", counter, [item path]];
		
		// Title (if applicable)
		if ([[item title] length] != 0)
			[output appendFormat:@"Title%u=%@\n", counter, [item title]];
		
		// Length (required, -1 inserted if none specified)
		if ([[item length] length] != 0)
			[output appendFormat:@"Length%u=%@\n", counter, [item length]];
		else
			[output appendFormat:@"Length%u=-1\n", counter];
				 
		 ++counter;
	}
	
	// Footer
	[output appendString:@"Version=2\n"];
	
	// Write out the file
	[output writeToFile:file atomically:YES encoding:NSUnicodeStringEncoding error:&error];
	
	if (error != nil)
		return NO;
	
	return YES;
}

+ (BOOL) writeBlankPlaylistToFile:(NSString *)file
{
	NSError *error = nil;
	[@"[Playlist]\nNumberOfEntries=0\nVersion=2" writeToFile:file atomically:YES encoding:NSUnicodeStringEncoding error:&error];
	
	if (error != nil)
		return NO;
	
	return YES;
}

+ (BOOL) appendData:(NSArray *)data toFile:(NSString *)file
{
	NSMutableArray *currentData = [[NSMutableArray alloc] init];
	NSEnumerator *enumerator;
	XplData *plsData;
	id object;
	
	// Load any current data from the file
	if ( ![XplPLSPlaylist readData:currentData fromFile:file] )
		return NO;
	
	// Append data items
	enumerator = [data objectEnumerator];
	while (object = [enumerator nextObject])
	{
		if (![object isKindOfClass:[XplData class]])
			return NO;
		
		plsData = (XplData *)object;
		[currentData addObject:plsData];
	}
	
	// Write out combined data
	return [XplPLSPlaylist writeData:currentData toFile:file];
}

+ (NSString *) formatExtension
{
	return @"pls";
}

+ (NSString *) fullFormatName
{
	return @"Generic Playlist File";
}

@end
