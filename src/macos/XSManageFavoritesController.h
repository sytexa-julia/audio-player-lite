//
//  ManageFavorites.h
//  Xapl
//
//  Created by Julia on 5/29/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@interface XSFavoritePlaylistData : NSObject
{
	@public
	NSString *filePath;
	BOOL showInMenu;
}

- (id) init;
- (id)initWithPath:(NSString *)p;

@property (copy) NSString *filePath;
@property (assign) BOOL showInMenu;

@end


@interface XSManageFavoritesController : NSWindowController 
{
	NSMutableArray *favoritesData;
	
	NSOpenPanel *importFavoritePanel;
	
	IBOutlet NSSegmentedControl *favoritesActions;
	IBOutlet NSTableView *favoritesList;
	IBOutlet NSMenu *addFavoriteMenu;
}

- (id)init;
- (void)awakeFromNib;

- (void) importFavoritePanelDidEnd: (NSOpenPanel*)openPanel returnCode: (int)returnCode contextInfo: (void *) x;

- (IBAction)addNewEmptyFavorite:(id)sender;
- (IBAction)importFavoriteFromFile:(id)sender;
- (IBAction)favoritesActionClicked:(id)sender;

@end
