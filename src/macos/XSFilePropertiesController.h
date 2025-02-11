//
//  XSFileProperties.h
//  Xapl
//
//  Created by Julia on 6/7/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@interface XSFilePropertiesData : NSObject
{
	NSString *key;
	NSString *value;
}

- (id)initWithKey:(NSString *)aKey value:(NSString *)aValue;
+ (id)filePropertiesWithKey:(NSString *)aKey value:(NSString *)aValue;

@property (copy) NSString *key;
@property (copy) NSString *value;

@end


@interface XSFilePropertiesController : NSWindowController 
{
	// Data storage
	NSMutableArray *fileProperties;
	
	IBOutlet NSTableView *filePropertiesTable;
}

- (id)init;
- (void)awakeFromNib;
- (void)loadInfoForFile:(NSString *)file;

@end
