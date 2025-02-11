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

#include "xaplPlaylist.h"
#include "xaplApplication.h"

#ifdef _DEBUG
#include <crtdbg.h>
#define DEBUG_NEW new(_NORMAL_BLOCK ,__FILE__, __LINE__)
#else
#define DEBUG_NEW new
#endif
#ifdef _DEBUG
#define new DEBUG_NEW
#endif 

// Static initializers
wxMutex *xaplPlaylist::sm_nextIdMutex = new wxMutex( wxMUTEX_RECURSIVE );
wxMutex *xaplPlaylist::sm_plDataMutex = new wxMutex( wxMUTEX_RECURSIVE );
size_t xaplPlaylist::m_autoNumber = 1;

// Define events
DEFINE_EVENT_TYPE(xaplEVT_PL_ITEMOP_COMPLETE)
DEFINE_EVENT_TYPE(xaplEVT_PL_ITEM_QUEUED)
DEFINE_EVENT_TYPE(xaplEVT_PL_ITEM_DEQUEUED)

// Creates an xaplPlaylistItem on the heap and returns a pointer to it. 
// Delete the object when you are done with it.
xaplPlaylistItem *xaplCreatePlaylistItem( long id, const wxString &display, const wxString &path, 
										  const wxString &fileName, const wxString &m3uTitle, 
										  const wxString &m3uLength, bool isNetStream, bool ok )
{
	xaplPlaylistItem *plItem = new xaplPlaylistItem;
	plItem->id = id;
	plItem->display = display;
	plItem->path = path;
	plItem->fileName = fileName;
	plItem->m3uTitle = m3uTitle;
	plItem->m3uLength = m3uLength;
	plItem->isNetStream = isNetStream;
	plItem->ok = ok;

	return plItem;
}

/* ====================================================================
 * ==== Private Helpers ===============================================*/
wxString xaplPlaylist::PlaylistNameFromFilePath( const wxString &filePath )
{
	if ( filePath.empty() )
		return wxString::Format( wxT("Playlist %u"), m_autoNumber++ );
	
	size_t lastSep = filePath.find_last_of( DIR_SEP );
	size_t lastDot = filePath.find_last_of( wxT(".") );
	if ( lastSep != wxString::npos && lastDot != wxString::npos )
		return filePath.SubString( lastSep + 1, lastDot );
	else
		return wxString::Format( wxT("Playlist %u"), m_autoNumber++ );
}

int xaplPlaylist::PlaylistFormatFromFilePath( const wxString &filePath )
{
	size_t lastDot = filePath.find_last_of( wxT(".") );
	if ( lastDot != wxString::npos )
	{
		wxString extension = filePath.Right( filePath.Length() - lastDot ).Lower();
		if ( extension.compare( wxT("m3u") ) == 0 )
			return M3U;
		else if ( extension.compare( wxT("pls") ) == 0 )
			return PLS;
		else if (extension.compare( wxT("txt") ) == 0 )
			return Text;
		else
			return Undefined;
	}

	return Undefined;
}

void xaplPlaylist::RebuiltIdToIndexMap( void )
{
	m_idToIndexMap.clear();
	for ( size_t i = 0; i < m_playlistData.GetCount(); ++i )
		m_idToIndexMap[m_playlistData[i]->id] = i;
}

int xaplPlaylist::GetIndexById( long id ) const
{
	map<long, size_t>::const_iterator it;
	it = m_idToIndexMap.find( id );
	if( it != m_idToIndexMap.end() ) 
		return (int)it->second;

	return xaplNO_INDEX;
}

void xaplPlaylist::_Clear( void )
{
	wxMutexLocker nextIdLocker(*sm_nextIdMutex);
	wxMutexLocker plDataLocker(*sm_plDataMutex);

	// Clear old data
	for ( size_t i = 0; i < m_playlistData.GetCount(); ++i )
		delete m_playlistData[ i ];

	m_nextId = 0;
	m_playlistData.Clear();
	m_idToIndexMap.clear();
	m_playHistory.Clear();
}

void xaplPlaylist::PostItemopCompleteEvent( wxArrayInt *affectedIds, int opType )
{
	if ( m_evtHandler != NULL )
	{
		wxCommandEvent evt;
		evt.SetId( wxID_ANY );
		evt.SetInt( opType );
		evt.SetClientData( affectedIds );
		evt.SetEventType( xaplEVT_PL_ITEMOP_COMPLETE );
		wxPostEvent( m_evtHandler, evt );
	}
	else
	{
		delete affectedIds;
	}
}

void xaplPlaylist::PostItemQueuedEvent( long id )
{
	if ( m_evtHandler != NULL )
	{
		wxCommandEvent evt;
		evt.SetId( wxID_ANY );
		evt.SetExtraLong( id );
		evt.SetEventType( xaplEVT_PL_ITEM_QUEUED );
		wxPostEvent( m_evtHandler, evt );
	}
}

void xaplPlaylist::PostItemDequeuedEvent( long id )
{
	if ( m_evtHandler != NULL )
	{
		wxCommandEvent evt;
		evt.SetId( wxID_ANY );
		evt.SetExtraLong( id );
		evt.SetEventType( xaplEVT_PL_ITEM_DEQUEUED );
		wxPostEvent( m_evtHandler, evt );
	}
}

/* ====================================================================
 * ==== Constructors / Destructor =====================================*/
xaplPlaylist::xaplPlaylist( void )
	: m_playingId( xaplNO_ITEM )
{ }

xaplPlaylist::xaplPlaylist( const wxString &plFilePath )
	: m_plFilePath( plFilePath ),
	  m_playingId( xaplNO_ITEM ),
	  m_nextId( 0 )
{
	wxASSERT( !plFilePath.empty() );
	m_plName = PlaylistNameFromFilePath( plFilePath );
	m_plFormat = PlaylistFormatFromFilePath( plFilePath );
}

xaplPlaylist::xaplPlaylist( const wxString &plName, const wxString &plFilePath )
	: m_plName( plName ),
	  m_plFilePath( plFilePath ),
	  m_playingId( xaplNO_ITEM ),
	  m_nextId( 0 )
{
	wxASSERT( !plFilePath.empty() );
	m_plFormat = PlaylistFormatFromFilePath( plFilePath );
}

xaplPlaylist::~xaplPlaylist( void )
{
	Clear();
	delete sm_nextIdMutex;
	delete sm_plDataMutex;
}

/* ====================================================================
 * ==== Standard getters ==============================================*/
wxString xaplPlaylist::GetPlaylistFilePath( void )
{
	return m_plFilePath;
}

wxString xaplPlaylist::GetPlaylistName( void )
{
	return m_plName;
}

/* ====================================================================
 * ==== File Operations ===============================================*/
bool xaplPlaylist::SavePlaylist( void )
{
	return SavePlaylistAs( m_plFilePath );
}

bool xaplPlaylist::SavePlaylistAs( const wxString &filePath )
{
	if ( filePath.empty() ) return false;

	// Make sure nothing changes while saving
	wxMutexLocker nextIdLocker(*sm_nextIdMutex);
	wxMutexLocker plDataLocker(*sm_plDataMutex);

	XplData **data = new XplData*[m_playlistData.GetCount()];
	bool delInvalid = wxGetApp().GetConfig()->autoDeleteInvalid;
	size_t cnt = 0;
	for ( size_t i = 0; i < m_playlistData.GetCount(); ++i )
	{
		xaplPlaylistItem *cd = m_playlistData[ i ];
		if ( cd == NULL || (delInvalid && !cd->ok) ) continue;

		data[cnt++] = new XplData( string(cd->path.mb_str( wxConvLibc )), 
								   string(cd->m3uLength.mb_str( wxConvLibc )), 
								   string(cd->m3uTitle.mb_str( wxConvLibc )) );
	}

	bool result = XplPlaylistHelper::AutoWriteData( data, cnt, string(filePath.mb_str(wxConvLibc)) );
	delete [] data;
	return result;
}

bool xaplPlaylist::LoadPlaylist( const wxString &filePath, const wxString &plName )
{
	wxArrayInt *ids;
	XplData **data;
	size_t count;
	
	if ( filePath.IsEmpty() ) 
		return false;

	wxMutexLocker nextIdLocker(*sm_nextIdMutex);
	wxMutexLocker plDataLocker(*sm_plDataMutex);

	_Clear();

	m_plFilePath = filePath;
	if ( plName == wxEmptyString )
	{
		size_t lastSep = filePath.Find( DIR_SEP, true );	
		size_t lastDot = filePath.Find( '.', true );
		if ( lastSep != wxNOT_FOUND && lastDot != wxNOT_FOUND )
			m_plName = filePath.Mid( lastSep + 1, lastDot - lastSep - 1 );
		else
			m_plName = filePath;
	}
	else
	{
		m_plName = plName;
	}

	// Read playlist
	data = XplPlaylistHelper::AutoReadData( &count, string(filePath.mb_str(wxConvLibc)) );
	if ( count == 0 || data == NULL )
		return false;

	ids = new wxArrayInt;
	for ( size_t i = 0; i < count; ++i )
	{
		ids->Add( m_nextId );
		wxString path = wxString( data[i]->GetPath().c_str(), wxConvLibc ).Trim().Trim(false);
		xaplPlaylistItem *plData = xaplCreatePlaylistItem( m_nextId, 
														   wxEmptyString, 
														   path,
														   path.Mid( path.Last( DIR_SEP ) + 1 ), 
														   wxString( data[i]->GetTitle().c_str(), wxConvLibc ),
														   wxString( data[i]->GetLength().c_str(), wxConvLibc ), 
														   path.Left(4).compare( wxT("http") ) == 0 );
		FormatItem( plData );
		m_playlistData.push_back( plData );
		m_idToIndexMap[m_nextId] = i;
		delete data[i];

		++m_nextId;
	}

	delete [] data;
	PostItemopCompleteEvent( ids, xaplIO_PLAYLIST_LOADED );
	return true;
}

/* ====================================================================
 * ==== Formatting Operations =========================================*/
void xaplPlaylist::SetFormat( int format )
{
	if ( m_currentFormat == format || format > 3 || format < 0 )
		return;

	// Make sure someone else doesn't mess with the data while we're using it
	wxMutexLocker plDataLocker(*sm_plDataMutex);

	m_currentFormat = format;

	// Update the display string for all items
	for ( size_t i = 0; i < m_playlistData.GetCount(); ++i )
	{
		xaplPlaylistItem *data = m_playlistData[i];
		FormatItem( data );
	}
}

wxString xaplPlaylist::FormatItem( xaplPlaylistItem *item )
{
	wxASSERT( item != NULL );
	wxString retVal = wxEmptyString;

	switch ( m_currentFormat )
	{
	case PLFORMAT_FULLPATH:
		{
			retVal << item->path;
		}
		break;

	case PLFORMAT_EXTM3U:
		{
			// If there is no EXTINF data, show the file name
			if ( item->m3uTitle.IsEmpty() )
				retVal << item->fileName;
			else
				retVal << item->m3uTitle;
		}
		break;

	case PLFORMAT_FILENAME:
	default:
		{
			retVal << item->fileName;
		}
		break;
	}

	int pos = m_queuedItems.Index( item->id );
	if ( pos != wxNOT_FOUND )
		item->display = wxString::Format( wxT("%d > %s"), pos + 1, retVal.c_str() );
	else
		item->display = retVal;

	return retVal;
}

/* ====================================================================
 * ==== Item Operations ===============================================*/
void xaplPlaylist::AddItem( xaplPlaylistItem *item )
{
	wxMutexLocker nextIdLocker(*sm_nextIdMutex);
	wxMutexLocker plDataLocker(*sm_plDataMutex);

	if ( item != NULL )
	{
		item->id = m_nextId;	
		m_playlistData.push_back( item );
		m_idToIndexMap[m_nextId] = m_playlistData.GetCount() - 1;
		
		wxArrayInt *ids = new wxArrayInt;
		ids->Add( m_nextId );
		PostItemopCompleteEvent( ids, xaplIO_ITEMS_ADDED );

		++m_nextId;
	}
}

void xaplPlaylist::AddItems( xaplPlaylistItemArray items )
{
	wxMutexLocker nextIdLocker(*sm_nextIdMutex);
	wxMutexLocker plDataLocker(*sm_plDataMutex);
	wxArrayInt *ids = new wxArrayInt;

	for ( size_t i = 0; i < items.GetCount(); ++i )
	{
		if ( items[i] == NULL )
			continue;

		items[i]->id = m_nextId;
		m_playlistData.push_back( items[i] );
		m_idToIndexMap[m_nextId] = m_playlistData.GetCount() - 1;
		ids->Add( m_nextId );

		++m_nextId;
	}

	PostItemopCompleteEvent( ids, xaplIO_ITEMS_ADDED );
}

void xaplPlaylist::AddPlaylist( xaplPlaylist *otherPlaylist )
{
	AddItems( otherPlaylist->GetPlaylistData() );
}

void xaplPlaylist::InsertItem( xaplPlaylistItem *item, size_t pos )
{
	wxMutexLocker nextIdLocker(*sm_nextIdMutex);
	wxMutexLocker plDataLocker(*sm_plDataMutex);

	if ( item != NULL )
	{
		item->id = m_nextId;
		m_playlistData.Insert( item, pos );

		wxArrayInt *ids = new wxArrayInt;
		ids->Add( item->id );
		PostItemopCompleteEvent( ids, xaplIO_ITEMS_INSERTED );

		// Remap ids to indexes...
		for ( map<long, size_t>::iterator i = m_idToIndexMap.begin(); i != m_idToIndexMap.end(); ++i )
		{
			long id = i->first;
			size_t idx = i->second;

			if ( idx >= pos )
				m_idToIndexMap[id] = pos + 1;
		}
		
		// ... and insert new id mapping
		m_idToIndexMap[m_nextId] = pos;

		++m_nextId;
	}
}

void xaplPlaylist::InsertItems( xaplPlaylistItemArray items, size_t pos )
{
	wxMutexLocker nextIdLocker(*sm_nextIdMutex);
	wxMutexLocker plDataLocker(*sm_plDataMutex);
	wxArrayInt *ids = new wxArrayInt;

	// Remap ids to indexes...
	for ( map<long, size_t>::iterator i = m_idToIndexMap.begin(); i != m_idToIndexMap.end(); ++i )
	{
		long id = i->first;
		size_t idx = i->second;

		if ( idx >= pos )
			m_idToIndexMap[id] = pos + items.GetCount();
	}

	// Insert items
	for ( size_t i = 0; i < items.GetCount(); ++i )
	{
		if ( items[i] == NULL )
			continue;

		items[i]->id = m_nextId;
		m_playlistData.Insert( items[i], pos + i );
		ids->Add( items[i]->id );

		m_idToIndexMap[m_nextId] = pos + i;

		++m_nextId;
	}

	PostItemopCompleteEvent( ids, xaplIO_ITEMS_INSERTED );
}

void xaplPlaylist::InsertPlaylist( xaplPlaylist *otherPlayist, size_t pos )
{
	InsertItems( otherPlayist->GetPlaylistData(), pos );
}

void xaplPlaylist::Clear( void )
{
	_Clear();
	PostItemopCompleteEvent( NULL, xaplIO_ALL_ITEMS_REMOVED );
}

void xaplPlaylist::DeleteItem( xaplPlaylistItem *item )
{
	wxMutexLocker plDataLocker(*sm_plDataMutex);
	wxArrayInt *ids = new wxArrayInt;

	if ( item != NULL )
	{
		// Remap ids to indexes...
		size_t pos = m_idToIndexMap[item->id];
		m_idToIndexMap.erase(item->id);
		for ( map<long, size_t>::iterator i = m_idToIndexMap.begin(); i != m_idToIndexMap.end(); ++i )
		{
			long id = i->first;
			size_t idx = i->second;

			if ( idx >= pos )
				m_idToIndexMap[id] = idx - 1;
		}

		// Delete the item
		ids->Add( item->id );
		delete item;
		m_playlistData.Remove( item );

		PostItemopCompleteEvent( ids, xaplIO_ITEMS_REMOVED );
	}
}

void xaplPlaylist::DeleteItems( xaplPlaylistItemArray items )
{
	wxMutexLocker plDataLocker(*sm_plDataMutex);
	wxArrayInt *ids = new wxArrayInt;

	// Delete the items
	for ( size_t i = 0; i < items.GetCount(); ++i )
	{
		ids->Add( items[i]->id );
		m_playlistData.Remove( items[i] );
	}

	RebuiltIdToIndexMap();
	PostItemopCompleteEvent( ids, xaplIO_ITEMS_REMOVED );
}

void xaplPlaylist::DeleteItemAt( size_t index )
{
	if ( index < m_playlistData.GetCount() )
		DeleteItem( m_playlistData[index] );
}

void xaplPlaylist::DeleteItemsAt( wxArrayInt indices )
{
	wxMutexLocker plDataLocker(*sm_plDataMutex);
	wxArrayInt *ids = new wxArrayInt;
	for ( size_t i = 0; i < indices.GetCount(); ++i )
	{
		ids->Add( m_playlistData[i]->id );
		delete m_playlistData[i];
		m_playlistData.RemoveAt( i );
	}

	RebuiltIdToIndexMap();
	PostItemopCompleteEvent( ids, xaplIO_ITEMS_REMOVED );
}

void xaplPlaylist::MoveItem( size_t source, size_t dest )
{
	wxMutexLocker plDataLocker(*sm_plDataMutex);
	xaplPlaylistItem *item = m_playlistData[source];
	m_playlistData.RemoveAt( source );
	m_playlistData.Insert( item, dest );

	wxArrayInt *ids = new wxArrayInt;
	ids->Add( item->id );
	ids->Add( (int)source );
	ids->Add( (int)dest );

	RebuiltIdToIndexMap();
	PostItemopCompleteEvent( ids, xaplIO_ITEM_MOVED );
}

xaplPlaylistItem *xaplPlaylist::RandomItem( bool forceValid )
{
	wxMutexLocker plDataLocker(*sm_plDataMutex);

	size_t range = m_playlistData.GetCount();
	int idx = xaplUtilities::RandomInt( 0, (int)range );
	bool bad = false;
	size_t badCnt = 0;
	while ( badCnt < m_playlistData.GetCount() && forceValid )
	{
		if ( m_playlistData[idx]->ok )
		{
			break;
		}
		else
		{
			++badCnt;
			idx = xaplUtilities::RandomInt( 0, (int)range );
		}
	}

	return badCnt == GetPlaylistItemCount() ? NULL : m_playlistData[idx];
}

xaplPlaylistItem *xaplPlaylist::FirstItemAfterId( long id, bool forceValid )
{
	wxMutexLocker plDataLocker(*sm_plDataMutex);

	if ( id < 0 ) return NULL;
	size_t idx = GetIndexById( id );
	bool bad = false;
	size_t badCnt = 0;
	while ( badCnt < GetPlaylistItemCount() && forceValid )
	{
		if ( HasQueuedItems() )
			idx = GetIndexById( DequeueItem() );
		else if ( idx + 1 >= GetPlaylistItemCount() ) 
			idx = 0;
		else
			++idx;

		if ( m_playlistData[idx]->ok )
			break;
		else
			++badCnt;
	}

	return badCnt == GetPlaylistItemCount() ? NULL : m_playlistData[idx];
}

xaplPlaylistItem *xaplPlaylist::FirstItemBeforeId( long id, bool forceValid )
{
	wxMutexLocker plDataLocker(*sm_plDataMutex);

	if ( id < 0 ) return NULL;
	int idx = GetIndexById( id );
	bool bad = false;
	size_t badCnt = 0;
	while ( badCnt < GetPlaylistItemCount() && forceValid )
	{
		if ( idx - 1 < 0 ) 
			idx = (int)GetPlaylistItemCount() - 1;
		else
			--idx;

		if ( m_playlistData[idx]->ok )
			break;
		else
			++badCnt;
	}

	return badCnt == GetPlaylistItemCount() ? NULL : m_playlistData[idx];
}

void xaplPlaylist::SetItemOk( long id, bool ok )
{
	xaplPlaylistItem *item = GetItemById( id );
	if ( item != NULL )
		item->ok = ok;
}

void xaplPlaylist::ResetOkStatusForAllItems( bool ok )
{
	for ( size_t i = 0; i < GetPlaylistItemCount(); ++i )
		m_playlistData[i]->ok = ok;
}

/* ====================================================================
 * ==== Playing item management =======================================*/
void xaplPlaylist::SetPlayMode( int playMode )
{
	wxASSERT( playMode >= 1 && playMode <= 3 );
	m_playMode = playMode;
}

int  xaplPlaylist::GetPlayMode( void ) const
{
	return m_playMode;
}

bool xaplPlaylist::ItemIsPlaying( long id ) const
{
	return m_playingId == id;
}

void xaplPlaylist::SetPlayingItem( long id, bool addToHistory )
{
	if ( id < 0 )
	{
		m_playingId = xaplNO_ITEM;
		m_playHistory.Clear();
		return;
	}
	
	m_playingId = id;
	if ( addToHistory && ( m_playHistory.Count() == 0 || (m_playHistory.Count() > 0 && m_playHistory.back() != id) ) )
		m_playHistory.push_back( m_playingId );
}

long xaplPlaylist::GetPlayingItem( void ) const
{
	return m_playingId;
}

void xaplPlaylist::ClearHistory( void )
{
	m_playHistory.Clear();
}

long xaplPlaylist::Next( bool setPlaying )
{
	// If no song is playing, get an item ID to work from
	long workingId = m_playingId;
	if ( workingId < 0 )
	{
		wxMutexLocker plDataLocker(*sm_plDataMutex);
		workingId = m_playlistData[0]->id;
	}

	// Decide what the next item will be based on the play mode
	switch ( GetPlayMode() )
	{
	case PLAYMODE_SHUFFLE:
		workingId = RandomItem()->id;
		break;
	case PLAYMODE_LOOP_SONG:
			break;
	case PLAYMODE_LOOP_LIST:
	default:
		workingId = FirstItemAfterId( workingId )->id;
		break;
	}

	if ( setPlaying ) 
		SetPlayingItem( workingId );

	return workingId;
}

long xaplPlaylist::Previous( bool setPlaying )
{
	// Pop an item off the playback history stack
	if ( m_playHistory.GetCount() > 0 && m_playingId == m_playHistory.back() )
		m_playHistory.pop_back();
	
	// If no song is playing, get an item ID to work from
	long workingId = m_playingId;
	if ( workingId < 0 )
	{
		wxMutexLocker plDataLocker(*sm_plDataMutex);
		workingId = m_playlistData[0]->id;
	}

	// We go back through play history before going back sequentially
	int id = m_playHistory.Count() > 0 ? m_playHistory.back() : xaplNO_ITEM;
	if ( id <= xaplNO_ITEM || m_playHistory.Count() == 0 )
		workingId = FirstItemBeforeId( workingId )->id;
	else
		workingId = id;

	if ( setPlaying )
		SetPlayingItem( workingId );

	return workingId;
}

/* ====================================================================
 * ==== Queue Management ==============================================*/
bool xaplPlaylist::ItemIsQueued( long id ) const
{
	return m_queuedItems.Index( id ) != wxNOT_FOUND;
}

bool xaplPlaylist::HasQueuedItems( void ) const
{
	return m_queuedItems.GetCount() > 0;
}

size_t xaplPlaylist::GetQueueLength( void ) const
{
	return m_queuedItems.GetCount();
}

void xaplPlaylist::QueueItem( long id )
{
	int pos = m_queuedItems.Index( id );
	if ( pos != wxNOT_FOUND )
		RemoveQueuedItem( id );
	else
		m_queuedItems.Add( id );
	
	xaplPlaylistItem *item = GetItemById( id );
	FormatItem( item );

	PostItemQueuedEvent( id );
}

void xaplPlaylist::QueueItems( const wxArrayInt &ids )
{
	if ( ids.GetCount() == 0 ) return;
	
	for ( size_t i = 0; i < ids.Count(); ++i )
		QueueItem( ids[i] );
}

long xaplPlaylist::DequeueItem( void )
{
	long next = m_queuedItems.front();
	RemoveQueuedItem( next );
	PostItemDequeuedEvent( next );
	return next;
}

void xaplPlaylist::EmptyQueue( void )
{
	for ( size_t i = 0; i < m_queuedItems.GetCount(); )
	{
		long id = m_queuedItems[0];
		m_queuedItems.RemoveAt( 0 );
		FormatItem( GetItemById( id ) );
	}
}

void xaplPlaylist::RemoveQueuedItem( long id )
{
	size_t pos = m_queuedItems.Index( id );
	m_queuedItems.Remove( id );
	FormatItem( GetItemById( id ) );

	// Renumber the remaining items
	for ( size_t i = pos; i < m_queuedItems.GetCount(); ++i )
		FormatItem( GetItemById( m_queuedItems[i] ) );
}

/* ====================================================================
 * ==== Data Retrieval ================================================*/
size_t xaplPlaylist::GetPlaylistItemCount( void ) const
{
	wxMutexLocker plDataLocker(*sm_plDataMutex);
	return m_playlistData.GetCount();
}

xaplPlaylistItem *xaplPlaylist::operator[]( int idx )
{
	return GetItemByIndex( idx );
}

xaplPlaylistItem *xaplPlaylist::GetItemByIndex( int idx )
{
	wxMutexLocker plDataLocker(*sm_plDataMutex);
	return m_playlistData[ idx ];
}

xaplPlaylistItem *xaplPlaylist::operator()( long id )
{
	return GetItemById( id );
}

xaplPlaylistItem *xaplPlaylist::GetItemById( long id ) const
{
	wxMutexLocker plDataLocker(*sm_plDataMutex);
	
	map<long, size_t>::const_iterator it;
	it = m_idToIndexMap.find( id );
	if( it != m_idToIndexMap.end() ) 
	{
		size_t pos = it->second;
		xaplPlaylistItem *itm = m_playlistData[pos];
		if ( itm != NULL )
			return itm;
	}

	return NULL;
}

xaplPlaylistItem *xaplPlaylist::GetPlayingItemData( void ) const
{
	return GetItemById( m_playingId );
}

xaplPlaylistItemArray xaplPlaylist::GetPlaylistData( void )
{
	wxMutexLocker plDataLocker(*sm_plDataMutex);
	return m_playlistData;
}

bool xaplPlaylist::IsItemOk( long id )
{
	return GetItemById( id )->ok; 
}

bool xaplPlaylist::GetMetaData( int idx, xaplPlaylistItemMetaData *metaData )
{
	if ( metaData == NULL )
		return false;

	wxMutexLocker plDataLocker(*sm_plDataMutex);
	xaplPlaylistItem *plItem = m_playlistData[ idx ];
		
	// Path and size
	{
		wxLogNull noLog;
		wxFile fd( plItem->path );
		if ( !fd.IsOpened() )
			return false;

		metaData->path = plItem->path.Mid( 0, plItem->path.Last(DIR_SEP) + 1 );
		metaData->sizeBytes = fd.Length();
		metaData->sizeBytesFormatted = xaplUtilities::FormatThousands( wxString::Format( wxT("%i"), fd.Length() ) );

		fd.Close();
	}

	// Tag info
	char str[2048];
#ifdef __WINDOWS__
	strcpy_s( str, 2048, (const char *)plItem->path.mb_str(wxConvUTF8) );
#else
	strcpy( str, (const char *)plItem->path.mb_str(wxConvUTF8) ); 
#endif
	TagLib::FileName fn( str );
	TagLib::FileRef file( fn );
	if ( !file.isNull() && file.tag() )
	{
		TagLib::Tag *tag = file.tag();

		metaData->title = wxString(tag->title().toCString(true), wxConvLibc).Trim().Trim(false);
		metaData->artist = wxString(tag->artist().toCString(true), wxConvLibc).Trim().Trim(false);
		metaData->album = wxString(tag->album().toCString(true), wxConvLibc).Trim().Trim(false);
		metaData->year = tag->year();
		metaData->comments = wxString(tag->comment().toCString(true), wxConvLibc).Trim().Trim(false);
		metaData->trackNumber = tag->track();
		metaData->genre = wxString(tag->genre().toCString(true), wxConvLibc).Trim().Trim(false);

		if ( file.audioProperties() ) 
		{
			TagLib::AudioProperties *properties = file.audioProperties();

			int seconds = properties->length() % 60;
			int minutes = ( properties->length() - seconds ) / 60;

			metaData->lengthSeconds = properties->length();
			metaData->lengthFormatted = wxString::Format( wxT("%i:%02i"), minutes, seconds );
			metaData->bitRate = properties->bitrate();
			metaData->sampleRate = properties->sampleRate();
			metaData->channelCount = properties->channels();

			return true;
		}
		else
		{
			metaData->lengthSeconds = 0;
			metaData->lengthFormatted = wxEmptyString;
			metaData->bitRate = 0;
			metaData->sampleRate = 0;
			metaData->channelCount = 0;

			return true;
		}
	}

	return false;
}
