//
//  XplPlaylistHelper.m
//  Xapl
//
//  Created by Julia on 6/11/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import "XplPlaylistHelper.h"
#import "XplM3UPlaylist.h"
#import "XplPLSPlaylist.h"
#import "XplTextPlaylist.h"

@implementation XplPlaylistHelper

+ (BOOL)autoReadData:(NSMutableArray *)data fromFile:(NSString *)file
{
	NSString *extension = [[file pathExtension] lowercaseString];
	if ([extension compare:@"m3u"] == NSOrderedSame)
		return [XplM3UPlaylist readData:data fromFile:file];
	else if ([extension compare:@"pls"] == NSOrderedSame)
		return [XplPLSPlaylist readData:data fromFile:file];
	else if ([extension compare:@"txt"] == NSOrderedSame)
		return [XplTextPlaylist readData:data fromFile:file];
	
	return NO;
}

+ (BOOL)autoWriteData:(NSArray *)data toFile:(NSString *)file
{
	NSString *extension = [[file pathExtension] lowercaseString];
	if ([extension compare:@"m3u"] == NSOrderedSame)
		return [XplM3UPlaylist writeData:data toFile:file];
	else if ([extension compare:@"pls"] == NSOrderedSame)
		return [XplPLSPlaylist writeData:data toFile:file];
	else if ([extension compare:@"txt"] == NSOrderedSame)
		return [XplTextPlaylist writeData:data toFile:file];
	
	return NO;
}

+ (BOOL) autoWriteBlankPlaylistToFile:(NSString *)file
{
	NSString *extension = [[file pathExtension] lowercaseString];
	if ([extension compare:@"m3u"] == NSOrderedSame)
		return [XplM3UPlaylist writeBlankPlaylistToFile:file];
	else if ([extension compare:@"pls"] == NSOrderedSame)
		return [XplPLSPlaylist writeBlankPlaylistToFile:file];
	else if ([extension compare:@"txt"] == NSOrderedSame)
		return [XplTextPlaylist writeBlankPlaylistToFile:file];
	
	return NO;
}

+ (BOOL) autoAppendData:(NSArray *)data toFile:(NSString *)file
{
	NSString *extension = [[file pathExtension] lowercaseString];
	if ([extension compare:@"m3u"] == NSOrderedSame)
		return [XplM3UPlaylist appendData:data toFile:file];
	else if ([extension compare:@"pls"] == NSOrderedSame)
		return [XplPLSPlaylist appendData:data toFile:file];
	else if ([extension compare:@"txt"] == NSOrderedSame)
		return [XplTextPlaylist appendData:data toFile:file];
	
	return NO;
}

@end
