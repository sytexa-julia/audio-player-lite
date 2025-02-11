/* 
  Copyright (c) 2009, Xive Software
  All rights reserved.
  
  Redistribution and use in source and binary forms, with or without modification, 
  are permitted provided that the following conditions are met:

     * Redistributions of source code must retain the above copyright notice, this 
	   list of conditions and the following disclaimer.
     * Redistributions in binary form must reproduce the above copyright notice, 
	   this list of conditions and the following disclaimer in the documentation 
	   and/or other materials provided with the distribution.
     * Neither the name of Xive Software nor the names of its contributors 
	   may be used to endorse or promote products derived from this software 
	   without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY 
  EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES 
  OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT 
  SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, 
  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED 
  TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR 
  BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN 
  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH 
  DAMAGE.
*/

#ifndef __XAPL_PLAYLIST_H
#define __XAPL_PLAYLIST_H

#include <map>

#include <wx/wx.h>
#include <wx/file.h>
#include <wx/filename.h>

#include "../../lib/include/taglib/tag.h"
#include "../../lib/include/taglib/fileref.h"
#include "../common/xPlaylistLib/XplPlaylistHelper.h"

#include "xaplConstants.h"
#include "xaplUtilities.h"

using std::map;

// Structures
typedef struct
{
	wxString path;
	int sizeBytes;
	wxString sizeBytesFormatted;
	
	wxString title;
	wxString artist;
	wxString album;
	size_t year;
	wxString comments;
	size_t trackNumber;
	wxString genre;

	size_t lengthSeconds;
	wxString lengthFormatted;
	size_t bitRate;
	size_t sampleRate;
	size_t channelCount;
} xaplPlaylistItemMetaData;

typedef struct
{
	long id;
	int order;
	wxString display;
	wxString path;
	wxString fileName;
	wxString m3uLength;
	wxString m3uTitle;
	bool isNetStream;
	bool ok;
} xaplPlaylistItem;

// Global helper to create a xaplPlaylistItem and get its pointer
xaplPlaylistItem *xaplCreatePlaylistItem( long id, const wxString &display, const wxString &path, 
										  const wxString &fileName, const wxString &m3uTitle = wxEmptyString, 
										  const wxString &m3uLength = wxEmptyString, bool isNetStream = false, 
										  bool ok = true );


// Define custom arrays
WX_DEFINE_ARRAY( xaplPlaylistItem *, xaplPlaylistItemArray );
WX_DEFINE_ARRAY( wxEvtHandler *, xaplEvtHandlerArray );

// Events
BEGIN_DECLARE_EVENT_TYPES()
	DECLARE_EVENT_TYPE(xaplEVT_PL_ITEMOP_COMPLETE, wxID_ANY)	// Sent after items are added, inserted, deleted, or cleared
	DECLARE_EVENT_TYPE(xaplEVT_PL_ITEM_QUEUED, wxID_ANY)		// Sent after an item is added to the queue
	DECLARE_EVENT_TYPE(xaplEVT_PL_ITEM_DEQUEUED, wxID_ANY)		// Sent after an item is dequeued from the play queue
END_DECLARE_EVENT_TYPES()

#define EVT_PL_ITEMOP_COMPLETE(fn) \
    DECLARE_EVENT_TABLE_ENTRY( \
        xaplEVT_PL_ITEMOP_COMPLETE, wxID_ANY, wxID_ANY, \
        (wxObjectEventFunction)(wxEventFunction) wxStaticCastEvent( wxCommandEventFunction, &fn ), \
        (wxObject *) NULL \
    ),

#define EVT_PL_ITEM_QUEUED(fn) \
    DECLARE_EVENT_TABLE_ENTRY( \
        xaplEVT_PL_ITEM_QUEUED, wxID_ANY, wxID_ANY, \
        (wxObjectEventFunction)(wxEventFunction) wxStaticCastEvent( wxCommandEventFunction, &fn ), \
        (wxObject *) NULL \
    ),

#define EVT_PL_ITEM_DEQUEUED(fn) \
    DECLARE_EVENT_TABLE_ENTRY( \
        xaplEVT_PL_ITEM_DEQUEUED, wxID_ANY, wxID_ANY, \
        (wxObjectEventFunction)(wxEventFunction) wxStaticCastEvent( wxCommandEventFunction, &fn ), \
        (wxObject *) NULL \
    ),

// Item operations
typedef enum
{
	xaplIO_PLAYLIST_LOADED,
	xaplIO_ITEMS_ADDED,
	xaplIO_ITEMS_INSERTED,
	xaplIO_ITEMS_REMOVED,
	xaplIO_ALL_ITEMS_REMOVED,
	xaplIO_ITEM_MOVED
} xaplItemOperation;

// Names for some magic numbers
#define xaplNO_ITEM -1
#define xaplNO_INDEX -1

// Class declaration
class xaplPlaylist
{
private:
	static size_t m_autoNumber;

	xaplPlaylistItemArray m_playlistData;
	map<long, size_t> m_idToIndexMap;

	wxString m_plFilePath;
	wxString m_plName;
	long m_playingId;
	int m_playMode;
	int m_plFormat;
	int m_currentFormat;

	wxString PlaylistNameFromFilePath( const wxString &filePath );
	int PlaylistFormatFromFilePath( const wxString &filePath );
	void RebuiltIdToIndexMap( void );
	int GetIndexById( long id ) const;
	void _Clear( void );

	void PostItemopCompleteEvent( wxArrayInt *affectedIds, int opType );
	void PostItemQueuedEvent( long id );
	void PostItemDequeuedEvent( long id );

protected:
	wxArrayInt m_playHistory;
	wxArrayInt m_queuedItems;

	wxEvtHandler *m_evtHandler;
	long m_nextId;

	void SetEvtHandler( wxEvtHandler *handler ) { m_evtHandler = handler; }

public:
	// Mutexes
	static wxMutex *sm_nextIdMutex;
	static wxMutex *sm_plDataMutex;

	// Constructors / Destructor
	xaplPlaylist( void );
	xaplPlaylist( const wxString &plFilePath );
	xaplPlaylist( const wxString &plName, const wxString &plFilePath );
	virtual ~xaplPlaylist( void );

	// Standard getters
	wxString GetPlaylistFilePath( void );
	wxString GetPlaylistName( void );

	// File operations
	virtual bool SavePlaylist( void );
	virtual bool SavePlaylistAs( const wxString &filePath );
	virtual bool LoadPlaylist( const wxString &filePath, const wxString &plName = wxEmptyString );

	// Formatting operations
	virtual void SetFormat( int format );
	wxString FormatItem( xaplPlaylistItem *item );
	
	// Item operations
	virtual void AddItem( xaplPlaylistItem *item );
	virtual void AddItems( xaplPlaylistItemArray items );
	virtual void AddPlaylist( xaplPlaylist *otherPlaylist );
	virtual void InsertItem( xaplPlaylistItem *item, size_t pos );
	virtual void InsertItems( xaplPlaylistItemArray items, size_t pos );
	virtual void InsertPlaylist( xaplPlaylist *otherPlayist, size_t pos );

	virtual void Clear( void );
	virtual void DeleteItem( xaplPlaylistItem *item );
	virtual void DeleteItems( xaplPlaylistItemArray items );
	virtual void DeleteItemAt( size_t index );
	virtual void DeleteItemsAt( wxArrayInt indices );

	virtual void MoveItem( size_t source, size_t dest );

	xaplPlaylistItem *RandomItem( bool forceValid = true );
	xaplPlaylistItem *FirstItemAfterId( long id, bool forceValid = true );
	xaplPlaylistItem *FirstItemBeforeId( long id, bool forceValid = true );

	void SetItemOk( long id, bool ok = true );
	void ResetOkStatusForAllItems( bool ok = true );

	// Playing item management
	void SetPlayMode( int playMode );
	int  GetPlayMode( void ) const;

	bool ItemIsPlaying( long id ) const;
	virtual void SetPlayingItem( long id, bool addToHistory = true );
	long GetPlayingItem( void ) const;
	void ClearHistory( void );

	virtual long Next( bool setPlaying = true );
	virtual long Previous( bool setPlaying = true );

	// Queue management
	bool ItemIsQueued( long id ) const;
	bool HasQueuedItems( void ) const;
	size_t GetQueueLength( void ) const;
	virtual void QueueItem( long id );
	virtual void QueueItems( const wxArrayInt &ids );
	virtual long DequeueItem( void );
	virtual void EmptyQueue( void );
	virtual void RemoveQueuedItem( long id );

	// Data retrieval
	size_t GetPlaylistItemCount( void ) const;
	xaplPlaylistItem *operator[]( int idx );
	xaplPlaylistItem *GetItemByIndex( int idx );
	xaplPlaylistItem *operator()( long id );
	xaplPlaylistItem *GetItemById( long id ) const;
	xaplPlaylistItem *GetPlayingItemData( void ) const;
	xaplPlaylistItemArray GetPlaylistData( void );

	bool IsItemOk( long id );
	bool GetMetaData( int idx, xaplPlaylistItemMetaData *metaData );
};

#endif
