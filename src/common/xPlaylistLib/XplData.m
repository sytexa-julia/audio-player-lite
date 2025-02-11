//
//  XplData.m
//  Xapl
//
//  Created by Julia on 5/30/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import "XplData.h"

@implementation XplData

- (id) init
{
	if (self = [super init])
	{
		return self;
	}
	
	return nil;
}

- (id) initWithPath:(NSString*)p
{
	if (self = [super init])
	{
		path = [p retain];	
		return self;
	}
	
	return nil;
}

- (id) initWithPath:(NSString *)p andTitle:(NSString *)t
{
	if (self = [super init])
	{
		path = [p retain];
		title = [t retain];
		return self;
	}
	
	return nil;
}

- (id) initWithPath:(NSString *)p title:(NSString *)t andLength:(NSString *)l
{
	if (self = [super init])
	{
		path = [p retain];
		title = [t retain];
		length = [l retain];
		return self;
	}
	
	return nil;
}

- (id) initWithPath:(NSString *)p title:(NSString *)t length:(NSString *)l andComment:(NSString *)c
{
	if (self = [super init])
	{
		path = [p retain];
		title = [t retain];
		length = [l retain];
		comment = [c retain];
		return self;
	}
	
	return nil;
}

- (void) dealloc
{
	[path release];
	[title release];
	[length release];
	
	[super dealloc];
}

- (NSString *) path
{
	return path;
}

- (void) setPath:(NSString *)p
{
	path = [p retain];
}

- (NSString *)length
{
	return length;
}

- (void) setLength:(NSString *)l
{
	length = [l retain];
}

- (NSString *)title
{
	return title;
}

- (void) setTitle:(NSString *)t
{
	title = [t retain];
}

- (NSString *)comment
{
	return comment;
}

- (void) setComment:(NSString *)c
{
	comment = [c retain];
}

@end
