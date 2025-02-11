//
//  XSFileProperties.m
//  Xapl
//
//  Created by Julia on 6/7/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import "XSFilePropertiesController.h"

#include <taglib/tfile.h>
#include <taglib/tstringlist.h>
#include <taglib/tstring.h>
#include <taglib/tag.h>
#include <taglib/audioproperties.h>
#include <taglib/fileref.h>

@implementation XSFilePropertiesData

- (id)initWithKey:(NSString *)aKey value:(NSString *)aValue
{
	if (self = [super init])
	{
		self.key = aKey;
		self.value = aValue;
		return self;
	}
	
	return nil;
}

+ (id)filePropertiesWithKey:(NSString *)aKey value:(NSString *)aValue
{
	return [[XSFilePropertiesData alloc] initWithKey:aKey value:aValue];
}

@synthesize key;
@synthesize value;

@end


@implementation XSFilePropertiesController

- (id)init
{
	if (self = [super init])
	{
		fileProperties = [[NSMutableArray alloc] init];
		[self loadInfoForFile:nil];
		return self;
	}
	return nil;
}

- (void)awakeFromNib
{
	[filePropertiesTable setDataSource:self];
	[filePropertiesTable setDelegate:self];
}

- (void)loadInfoForFile:(NSString *)file
{
	if (file == nil)
	{
		[filePropertiesTable setEnabled:NO];
		return;
	}
	
	[filePropertiesTable setEnabled:YES];
	NSNumberFormatter *numberFormatter = [[NSNumberFormatter alloc] init];
	[numberFormatter setNumberStyle:NSNumberFormatterDecimalStyle];
		
	// Remove old data
	[fileProperties removeAllObjects];
	
	// Simple data
	[fileProperties addObject:[XSFilePropertiesData filePropertiesWithKey:@"File Name:" value:[file lastPathComponent]]];
	[fileProperties addObject:[XSFilePropertiesData filePropertiesWithKey:@"Ext. M3U Title:" value:@""]];
	[fileProperties addObject:[XSFilePropertiesData filePropertiesWithKey:@"Location:" value:file]];
	
	NSError *error;
	NSDictionary *fileAttributes = [[NSFileManager defaultManager] attributesOfItemAtPath:file error:&error];
	NSString *fileSize = [NSString stringWithFormat:@"%@ bytes", [numberFormatter stringFromNumber: [fileAttributes objectForKey:NSFileSize]]];
	[fileProperties addObject:[XSFilePropertiesData filePropertiesWithKey:@"Size:" value:fileSize]];
	
	// Complex data	
	TagLib::FileName fn([file fileSystemRepresentation]);
	TagLib::FileRef tlFileRef( fn );
	if (!tlFileRef.isNull() && tlFileRef.tag())
	{
		TagLib::Tag *tag = tlFileRef.tag();
		TagLib::String s;
		
		s = tag->title();
		if (!s.isNull())
			[fileProperties addObject:[XSFilePropertiesData filePropertiesWithKey:@"Title:" value:[NSString stringWithUTF8String:s.toCString(true)]]];
		
		s = tag->artist();
		if (!s.isNull())
			[fileProperties addObject:[XSFilePropertiesData filePropertiesWithKey:@"Artist:" value:[NSString stringWithUTF8String:s.toCString(true)]]];
		
		s = tag->album();
		if (!s.isNull())
			[fileProperties addObject:[XSFilePropertiesData filePropertiesWithKey:@"Album:" value:[NSString stringWithUTF8String:s.toCString(true)]]];
		
		s = tag->genre();
		if (!s.isNull())
			[fileProperties addObject:[XSFilePropertiesData filePropertiesWithKey:@"Genre:" value:[NSString stringWithUTF8String:s.toCString(true)]]];
		
		if (tag->year() != 0)
			[fileProperties addObject:[XSFilePropertiesData filePropertiesWithKey:@"Year:" value:[[NSNumber numberWithInt:tag->year()] stringValue]]];
		
		if (tag->track() != 0)
			[fileProperties addObject:[XSFilePropertiesData filePropertiesWithKey:@"Track #:" value:[[NSNumber numberWithInt:tag->track()] stringValue]]];

		s = tag->comment();
		if (!s.isNull())
			[fileProperties addObject:[XSFilePropertiesData filePropertiesWithKey:@"Comments:" value:[NSString stringWithUTF8String:s.toCString(true)]]];
		
		if (tlFileRef.audioProperties()) 
		{
			TagLib::AudioProperties *properties = tlFileRef.audioProperties();
			
			int seconds = properties->length() % 60;
			int minutes = ( properties->length() - seconds ) / 60;
			
			[fileProperties addObject:[XSFilePropertiesData filePropertiesWithKey:@"Length:" value:[NSString stringWithFormat:@"%i:%02i", minutes, seconds]]];
			
			if (properties->bitrate() != 0)
			{
				NSString *bitRate = [NSString stringWithFormat:@"%@ kbps", [numberFormatter stringFromNumber: [NSNumber numberWithInt:properties->bitrate()]]];
				[fileProperties addObject:[XSFilePropertiesData filePropertiesWithKey:@"Bitrate:" value:bitRate]];
			}
			
			if (properties->sampleRate() != 0)
			{
				NSString *sampleRate = [NSString stringWithFormat:@"%@ Hz", [numberFormatter stringFromNumber: [NSNumber numberWithInt:properties->sampleRate()]]];
				[fileProperties addObject:[XSFilePropertiesData filePropertiesWithKey:@"Sample Rate:" value:sampleRate]];
			}
			
			if (properties->channels() != 0)
				[fileProperties addObject:[XSFilePropertiesData filePropertiesWithKey:@"Channels:" value:[[NSNumber numberWithInt:properties->channels()] stringValue]]];
		}
	}
}

/****** NSTableView ******/
- (BOOL)tableView:(NSTableView *)aTableView 
shouldEditTableColumn:(NSTableColumn *)aTableColumn 
			  row:(NSInteger)rowIndex
{
	if ([fileProperties count] == 0) 
		return NO;
	
	if (aTableColumn == [[aTableView tableColumns] objectAtIndex:1] && rowIndex <= 1)
		return YES;
	
	return NO;
}

- (BOOL)tableView:(NSTableView *)aTableView 
  shouldSelectRow:(NSInteger)rowIndex
{
	if ([fileProperties count] == 0) 
		return NO;
	
	if (rowIndex == 2 || rowIndex == 5)
		return NO;
	
	return YES;
}

- (CGFloat)tableView:(NSTableView *)tableView 
		 heightOfRow:(NSInteger)rowIndex
{
	// First three rows regular sized
	if (rowIndex <= 2) return 16.0f;
	
	// Add height to the Location and Comments rows so all the content can be viewed
	NSInteger modifiedIndex = rowIndex;
	if (rowIndex > 2 && rowIndex < 5) --modifiedIndex;
	if (rowIndex > 5) modifiedIndex -= 2;
	XSFilePropertiesData *properties = [fileProperties objectAtIndex:modifiedIndex];
	if ([properties key] == @"Location:" || [properties key] == @"Comments:")
	{
		// TODO: the table gets messed up when the Location row shrinks to a single row...
		NSTableColumn *tableColumn = [[tableView tableColumns] objectAtIndex:1];
		NSTextStorage *textStorage = [[[NSTextStorage alloc] initWithString:[properties value]] autorelease];
		NSTextContainer *textContainer = [[[NSTextContainer alloc] initWithContainerSize: NSMakeSize([tableColumn width] - 4.0f, FLT_MAX)] autorelease];
		NSLayoutManager *layoutManager = [[[NSLayoutManager alloc] init] autorelease];
		
		[layoutManager addTextContainer:textContainer];
		[textStorage addLayoutManager:layoutManager];
		[textStorage addAttribute:NSFontAttributeName value:[[tableColumn dataCellForRow:modifiedIndex] font] range:NSMakeRange(0, [textStorage length])];
		[textContainer setLineFragmentPadding:0.0];
		
		(void) [layoutManager glyphRangeForTextContainer:textContainer];
		CGFloat newHeight = [layoutManager usedRectForTextContainer:textContainer].size.height;
		NSLog([NSString stringWithFormat:@"New Height:%f", newHeight]);
		return (newHeight < 16.0f) ? 16.0f : newHeight + (newHeight / 16.0f);
	}
	
	// Rest of rows regular sized
	return 16.0f;
}

/****** NSTableDataSource ******/
- (int)numberOfRowsInTableView:(NSTableView *)tableView
{
    return [fileProperties count] + 2;
}

- (id)tableView:(NSTableView *)tableView 
objectValueForTableColumn:(NSTableColumn *)tableColumn 
			row:(int)row
{
	if ([fileProperties count] == 0) return nil;
	
	// Separator rows
	if (row == 2 || row == 5) return @"";
	
	// Adjust for the two spacer rows
	if (row > 2 && row < 5) --row;
	if (row > 5) row -= 2;
	
	// First column: field name, Second column: field value
	if (tableColumn == [[tableView tableColumns] objectAtIndex:0])
	{
		return [[fileProperties objectAtIndex:row] key];
	}
	else if (tableColumn == [[tableView tableColumns] objectAtIndex:1])
	{
		return [[fileProperties objectAtIndex:row] value];
	}
	
	return nil;
}

- (void)tableView:(NSTableView *)tableView
   setObjectValue:(id)object
   forTableColumn:(NSTableColumn *)tableColumn 
			  row:(int)row
{
	if (row > 1 || tableColumn == [[tableView tableColumns] objectAtIndex:0])
		return;
	
	if ([object class] != [NSString class])
		return;
	
	[[fileProperties objectAtIndex:row] setValue: object];
	
	// TODO: rename file and/or save Ext. M3U data
}

@end
