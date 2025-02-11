//
//  ManageFavorites.m
//  Xapl
//
//  Created by Julia on 5/29/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import "XSManageFavoritesController.h"

@implementation XSFavoritePlaylistData

- (id) init
{
	if (self = [super init])
	{
		showInMenu = NO;
		return self;
	}
	
	return nil;
}

- (id)initWithPath:(NSString *)p
{
	if (self = [super init])
	{
		filePath = [p retain];
		showInMenu = NO;
		return self;
	}
	
	return nil;
}

@synthesize filePath;
@synthesize showInMenu;

@end


@implementation XSManageFavoritesController

- (id) init
{
	if (self = [super init])
	{
		// Favorites data storage
		favoritesData = [[NSMutableArray alloc] init];
		// TODO: load list of favorites from files in favorites directory
		
		// Open panels
		importFavoritePanel = [[NSOpenPanel alloc] init];
		[importFavoritePanel setCanChooseDirectories: NO];
		[importFavoritePanel setCanChooseFiles:YES];
		[importFavoritePanel setAllowsMultipleSelection: YES];
		
		return self;
	}
	
	return nil;
}

- (void)awakeFromNib
{	
	// Set data source
	[favoritesList setDataSource:self];
	
	// Set menu for segmented control in Manage Favorites
	[favoritesActions setMenu:addFavoriteMenu forSegment:0];
	
	// Register file dragged types
	[favoritesList registerForDraggedTypes:[NSArray arrayWithObject:NSFilenamesPboardType]];
}

- (void) importFavoritePanelDidEnd: (NSOpenPanel*)openPanel returnCode: (int)returnCode contextInfo: (void *) x
{
	NSArray *files = [openPanel filenames];
	NSEnumerator *enumerator = [files objectEnumerator];
	NSString *path;
	while (path = [enumerator nextObject])
	{
		XSFavoritePlaylistData *fpd = [[XSFavoritePlaylistData alloc] initWithPath:path];
		[favoritesData addObject:fpd];
		// TODO: copy file and convert any relative paths to absolute
	}
	
	[favoritesList reloadData];
}

/****** Drag and Drop ******/
- (NSDragOperation)tableView:(NSTableView*)tv 
				validateDrop:(id <NSDraggingInfo>)info 
				 proposedRow:(int)row 
	   proposedDropOperation:(NSTableViewDropOperation)op
{
    NSPasteboard *pboard = [info draggingPasteboard];
    if ( [[pboard types] containsObject:NSFilenamesPboardType] )
	{
		NSArray *files = [pboard propertyListForType:NSFilenamesPboardType];
		NSEnumerator *enumerator = [files objectEnumerator];
		NSString *fileName;
		while (fileName = [enumerator nextObject])
		{
			// Only accept m3u and pls playlist files
			NSString *ext = [[fileName substringFromIndex:[fileName length] - 4] lowercaseString];
			if ([ext compare:@".m3u"] != NSOrderedSame && [ext compare:@".pls"] != NSOrderedSame)
				return NSDragOperationNone;
		}
		
		return NSDragOperationCopy;
	}
	else
	{
		return NSDragOperationNone;
	}
}

- (BOOL)tableView:(NSTableView *)aTableView 
	   acceptDrop:(id <NSDraggingInfo>)info
			  row:(int)row 
	dropOperation:(NSTableViewDropOperation)operation
{
	NSPasteboard *pboard = [info draggingPasteboard];
    if ( [[pboard types] containsObject:NSFilenamesPboardType] ) 
	{
        NSArray *files = [pboard propertyListForType:NSFilenamesPboardType];
		NSEnumerator *enumerator = [files objectEnumerator];
		NSString *fileName;
		while (fileName = [enumerator nextObject])
		{
			XSFavoritePlaylistData *fpd = [[XSFavoritePlaylistData alloc] initWithPath:fileName];
			[favoritesData insertObject:fpd atIndex:row];
			// TODO: copy file and convert any relative paths to absolute
		}
		[favoritesList reloadData];
    }
    return YES;
}

/****** NSTableDataSource ******/
- (int)numberOfRowsInTableView:(NSTableView *)tableView
{
    return [favoritesData count];
}

- (id)tableView:(NSTableView *)tableView 
	objectValueForTableColumn:(NSTableColumn *)tableColumn 
			row:(int)row
{
	if (tableColumn == [[favoritesList tableColumns] objectAtIndex:0])
	{
		NSString *path = [[favoritesData objectAtIndex:row] filePath];
		NSUInteger start = [path rangeOfString:@"/" options:NSBackwardsSearch].location + 1;
		if (start == NSNotFound)
			return path;
		
		return [path substringWithRange:NSMakeRange(start, [path length] - start - 4)];
	}
	else
	{
		return [[favoritesData objectAtIndex:row] showInMenu] ? [NSNumber numberWithInt:NSOnState] : [NSNumber numberWithInt:NSOffState];
	}
	
	return nil;
}

- (void)tableView:(NSTableView *)tableView
   setObjectValue:(id)object
   forTableColumn:(NSTableColumn *)tableColumn 
			  row:(int)row
{
	if (tableColumn == [[favoritesList tableColumns] objectAtIndex:0])
	{
		NSString *data = [[favoritesData objectAtIndex:row] filePath];
		NSUInteger start = [data rangeOfString:@"/" options:NSBackwardsSearch].location + 1;
		if (start == NSNotFound)
			[[favoritesData objectAtIndex:row] setFilePath:data];
		
		NSString *name = [data substringWithRange:NSMakeRange(start, [data length] - start - 4)];
		data = [data stringByReplacingOccurrencesOfString:name withString:object];
		[[favoritesData objectAtIndex:row] setFilePath:data];
	}
	else
	{
		[[favoritesData objectAtIndex:row] setShowInMenu:[object boolValue]];
	}
}

/****** IBActions ******/

- (IBAction)addNewEmptyFavorite:(id)sender
{	
	[favoritesList deselectAll:nil];	// To commit and end the current edit
	[favoritesData insertObject:[[XSFavoritePlaylistData alloc] initWithPath:@"/Users/Julia/Desktop/New Favorite.m3u"] atIndex:0];
	[favoritesList reloadData];
	[favoritesList selectRow:0 byExtendingSelection:NO];
	[favoritesList editColumn:0 row:0 withEvent:nil select:YES];
}

- (IBAction)importFavoriteFromFile:(id)sender
{
	[importFavoritePanel beginSheetForDirectory: NSHomeDirectory()
									  file: nil
									 types: [NSArray arrayWithObjects: @"m3u", @"pls", nil]
							modalForWindow: [self window]
							 modalDelegate: self
							didEndSelector: @selector(importFavoritePanelDidEnd:returnCode:contextInfo:)
							   contextInfo: NULL];
}

- (IBAction)favoritesActionClicked:(id)sender
{
    NSIndexSet* indices;
	NSUInteger currIndex;
	
	if ([sender isKindOfClass:[NSSegmentedControl class]])
    {
		switch ([favoritesActions selectedSegment])
        {
            case 0: // Add, make blank by default
				[self addNewEmptyFavorite:sender];
                break;
            case 1: // Remove
				indices = [favoritesList selectedRowIndexes];
				currIndex = [indices lastIndex];
				while (currIndex != NSNotFound)
				{
					[favoritesData removeObjectAtIndex:currIndex];
					currIndex = [indices indexLessThanIndex:currIndex];
				}
								
				[favoritesList reloadData];
                break;
            case 2: // Reveal in Finder
				// All favorites are stored in the same dir, so this is really easy
				[[NSWorkspace sharedWorkspace] selectFile:@"/Users/Julia/Desktop/sdfadfgadfh.m3u" inFileViewerRootedAtPath:@"/Users/Julia/Desktop/"];
                break;
            default:
                break;
        }
	}
}

@end
