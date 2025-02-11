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

#include "xaplFileLoader.h"
#include "xaplPlaylistCtrl.h"
#include "xaplPlaylistWindow.h"

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
wxMutex *xaplPlaylistCtrl::sm_dataVisMutex = new wxMutex( wxMUTEX_RECURSIVE );

// Define events
DEFINE_EVENT_TYPE(xaplEVT_ADD_ITEM)
DEFINE_EVENT_TYPE(xaplEVT_BACKGROUND_OP_COMPLETE)

// Event table
BEGIN_EVENT_TABLE( xaplPlaylistCtrl, wxVListBox )
	EVT_ADD_ITEM( xaplPlaylistCtrl::OnAddItem )
	EVT_BACKGROUND_OP_COMPLETE( xaplPlaylistCtrl::OnBackgroundOpComplete )

	EVT_PL_ITEMOP_COMPLETE( xaplPlaylistCtrl::OnPlaylistItemOpComplete )
	EVT_PL_ITEM_QUEUED( xaplPlaylistCtrl::OnPlaylistItemQueued )
	EVT_PL_ITEM_DEQUEUED( xaplPlaylistCtrl::OnPlaylistItemDequeued )

	EVT_KMP_TABLE_REBUILD_COMPLETE( xaplPlaylistCtrl::OnKmpTableRebuildComplete )

	EVT_KEY_DOWN( xaplPlaylistCtrl::OnKeyDown )
	EVT_LEFT_DOWN( xaplPlaylistCtrl::OnMouseLeftDown )
	EVT_LEFT_UP( xaplPlaylistCtrl::OnMouseLeftUp )
	EVT_LEAVE_WINDOW( xaplPlaylistCtrl::OnMouseLeftUp )
	EVT_MOTION( xaplPlaylistCtrl::OnMouseMotion )
END_EVENT_TABLE()

/* ====================================================================
 * ==== wxVListBox Implementation =====================================*/
void xaplPlaylistCtrl::OnDrawBackground( wxDC& dc, const wxRect& rect, size_t n ) const
{
	wxMutexLocker plDataVisLocker(*sm_dataVisMutex);
	const bool isSelected = IsSelected(n);
	const bool isQueued = ItemIsQueued( GetItemIdByIndex( m_visibleData[n] ) );

    if ( isSelected )
        dc.SetBrush( wxBrush(m_style.selectionBg, wxSOLID) );
    else if ( ItemIsPlaying( GetItemById(m_visibleData[n])->id ) )
		dc.SetBrush( wxBrush(m_style.nowPlayingBg, wxSOLID) );
	else if ( isQueued )
		dc.SetBrush( wxBrush(m_style.queuedBg, wxSOLID) );
	else
        dc.SetBrush( wxBrush(m_style.bg, wxSOLID) );

    dc.SetPen(*wxTRANSPARENT_PEN);
    dc.DrawRectangle(rect);
}

void xaplPlaylistCtrl::OnDrawItem( wxDC& dc, const wxRect& rect, size_t n ) const
{
	xaplPlaylistItem *data = GetItemByListIndex( n );
	wxFont font = m_style.font;
	
	if ( data->id == GetPlayingItem() )
		font.SetWeight( wxFONTWEIGHT_BOLD );
	
	if ( IsSelected(n) )
		dc.SetTextForeground( m_style.selectionFg );
	else if ( !data->ok )
		dc.SetTextForeground( *wxRED );
	else if ( data->id == GetPlayingItem() )
		dc.SetTextForeground( m_style.nowPlayingFg );
	else if ( ItemIsQueued(data->id) )
		dc.SetTextForeground( m_style.queuedFg );
	else
		dc.SetTextForeground( m_style.fg );

	dc.SetFont( font );
	dc.DrawText( data->display, rect.x + 2, rect.y );
}

wxCoord xaplPlaylistCtrl::OnMeasureItem( size_t n ) const
{
	wxMemoryDC memDC;
	wxCoord y;
	xaplPlaylistItem *plItem = GetItemByListIndex( n );

	if ( plItem != NULL )
	{
		memDC.SetFont( m_style.font );
		memDC.GetTextExtent( plItem->display, NULL, &y );
		return y;
	}

	return 0;
}

/* ====================================================================
 * ==== Constructor / Destructor ======================================*/

// Constructor.
xaplPlaylistCtrl::xaplPlaylistCtrl( wxWindow* parent, wxWindowID id ) 
	:	wxVListBox( parent, id, wxDefaultPosition, wxDefaultSize, wxLB_MULTIPLE | wxBORDER_NONE ), 
		xaplPlaylist(),
		m_dragging( false ),
		m_dirty( false )
{ 
	xaplPlaylist::SetEvtHandler( this );
	
	m_savedSelection = GetFirstSelection();
	SetItemCount( 0 );

	m_fileLoader = new xaplFileLoader( this );
	m_fileLoader->Create();
	m_fileLoader->SetPriority( 75 );
	m_fileLoader->Run();

	m_kmpTableBuilder = new xaplKmpTableBuilderThread( this );
	m_kmpTableBuilder->Create();
	m_kmpTableBuilder->SetPriority( 50 );
	m_kmpTableBuilder->Run();

	// Default style
	m_style.bg = *wxWHITE;
	m_style.fg = *wxBLACK;
	m_style.selectionBg = wxSystemSettings::GetColour( wxSYS_COLOUR_HIGHLIGHT );
	m_style.selectionFg = *wxWHITE;
	m_style.font = wxSystemSettings::GetFont( wxSYS_DEFAULT_GUI_FONT );
	m_style.nowPlayingBg = *wxWHITE;
	m_style.nowPlayingFg = *wxBLACK;
	m_style.queuedFg = wxColor(0x007FFF);
	m_style.queuedBg = *wxWHITE;
}


// Destructor. Frees all playlist data on the heap and destroys the control.
xaplPlaylistCtrl::~xaplPlaylistCtrl( void )
{
	m_fileLoader->Delete();
	delete sm_dataVisMutex;
}

/* ====================================================================
 * ==== Playing Item Management =======================================*/
void xaplPlaylistCtrl::SetPlayingItem( long id, bool addToHistory )
{
	xaplPlaylist::SetPlayingItem( id, addToHistory );
	Refresh();
}

long xaplPlaylistCtrl::Next( bool setPlaying )
{
	// If no song is playing, get an item ID to work from
	long workingId = GetPlayingItem();
	if ( workingId < 0 )
	{
		if ( GetSelectedCount() > 0 )
		{
			size_t sel = GetFirstSelection();
			workingId = GetItemIdByListIndex( sel );
		}
		else
		{
			workingId = GetItemIdByListIndex( GetItemCount() - 1 );
		}
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

long xaplPlaylistCtrl::Previous( bool setPlaying )
{
	// Pop an item off the playback history stack
	if ( m_playHistory.GetCount() > 0 && GetPlayingItem() == m_playHistory.back() )
		m_playHistory.pop_back();
	
	// If no song is playing, get an item ID to work from
	long workingId = GetPlayingItem();
	if ( workingId < 0 )
	{
		if ( GetSelectedCount() > 0 )
		{
			size_t sel = GetFirstSelection();
			workingId = GetItemIdByListIndex( sel );
		}
		else
		{
			workingId = GetItemIdByListIndex( 0 );
		}
	}

	// We go back through play history before going back sequentially
	int id = m_playHistory.Count() > 0 ? m_playHistory.back() : -1;
	if ( id < 0 || m_playHistory.Count() == 0 )
		workingId = FirstItemBeforeId( workingId )->id;
	else
		workingId = id;

	if ( setPlaying )
		SetPlayingItem( workingId );

	return workingId;
}

/* ====================================================================
 * ==== File management ===============================================*/

void xaplPlaylistCtrl::AddItem( const wxString &path, bool inferNetRadio )
{
	xaplPlaylistWindow *mainWnd = static_cast<xaplPlaylistWindow *>(GetParent());
	mainWnd->SetTempStatusText( wxT("Opening...") );

	m_fileLoader->AddWork( path );
}

void xaplPlaylistCtrl::AddItems( const wxArrayString &pathList, bool inferNetRadio )
{
	xaplPlaylistWindow *mainWnd = static_cast<xaplPlaylistWindow *>(GetParent());
	mainWnd->SetTempStatusText( wxT("Opening...") );

	m_fileLoader->AddWork( pathList );
}

void xaplPlaylistCtrl::Reorder( size_t source, size_t dest )
{
	MoveItem( source, dest );

	m_dirty = true;
	Refresh();
}

void xaplPlaylistCtrl::DeleteItemsWithIndices( wxArrayInt indices )
{
	// Deselect the items
	for ( size_t i = 0; i < indices.GetCount(); ++i )
		Select( indices[i], false );

	for ( int i = (int)indices.Count() - 1; i >= 0; --i )
	{
		long id = GetItemIdByIndex( indices[i] );

		// Remove from play history
		if ( m_playHistory.Index( id, true ) != wxNOT_FOUND )
			m_playHistory.Remove( id );

		DeleteItemAt( indices[i] );

		// Remove deleted item from stored search states
		if ( m_searchStates.size() > 0 )
		{
			for ( list<wxArrayInt *>::iterator it = m_searchStates.begin(); it != m_searchStates.end(); it++ )
			{
				// Remove the removed item's id from this state
				(*it)->Remove( id );

				// Re-align the indices in the search state
				for ( size_t j = id; j < (*it)->GetCount(); ++j )
					(*it)->Item(j)--;
			}
		}

		// Remove from visibility index
		m_visibleData.Remove( id );
	}

	SetItemCount( m_visibleData.GetCount() );
	m_dirty = true;
}

void xaplPlaylistCtrl::DeleteSelected( void )
{
	Freeze();
	wxArrayInt selections;
	GetSelections( selections );
	DeleteItemsWithIndices( selections );
	Thaw();

	if ( GetItemCount() > 0 )
	{
		// Select the next item
		int newSel = selections.Item( 0 );
		newSel = newSel > (int)GetItemCount() - 1 ? (int)GetItemCount() - 1 : newSel;
		
		DeselectAll();
		SetSelection( newSel );
		ScrollToSelection();
	}
}

void xaplPlaylistCtrl::Clear( void )
{
	xaplPlaylist::Clear();
}

void xaplPlaylistCtrl::RemoveInvalidItems( void )
{
	size_t selection = GetFirstSelection();
	
	wxProgressDialog *progress = new wxProgressDialog( wxT("Removing Missing Files..."), 
		wxT("This may take a minute if the playlist is very large."), 100, GetParent(), 
		wxPD_AUTO_HIDE | wxPD_SMOOTH | wxPD_CAN_ABORT );
	progress->Show();
	progress->SetFocus();

	// Find all items that are marked invalid or whose path points to a missing file
	wxArrayInt indexes;
	for ( size_t i = 0; i < GetItemCount(); ++i )
	{
		xaplPlaylistItem *data = GetItemById( GetItemIdByIndex( (size_t) i ) );
		if ( !data->ok || !wxFileExists( data->path ) )
		{
			indexes.Add( (int)i );
		}
		
		int pos = (int) ((double) i / (double) GetItemCount() * 100.0);
		pos = pos <= 2 ? 0 : pos - 2;
		if ( !progress->Update( pos ) )
		{
			delete progress;
			wxMessageBox( wxT("No item were removed from the list."), wxT("Xapl"), 
				wxICON_INFORMATION | wxOK );
			return;
		}
	}

	// Remove missing items
	Freeze();
	DeleteItemsWithIndices( indexes );
	progress->Update( 99 );

	m_dirty = true;
	SavePlaylist();
	progress->Update( 100 );

	Refresh();
	delete progress;
	
	DeselectAll();
	SetSelection( (int)selection );
	ScrollToSelection();
	Thaw();

	// Notify how many items were removed
	wxString msg = indexes.GetCount() == 1 ? wxT("1 item was removed from the list.") : wxString::Format( wxT("%i item were removed from the list."), indexes.GetCount() );
	wxMessageBox( msg, wxT("Xapl"), wxICON_INFORMATION | wxOK );
}

void xaplPlaylistCtrl::RemoveDuplicateItems( void )
{
	size_t selection = GetFirstSelection();
	
	wxProgressDialog *progress = new wxProgressDialog( wxT("Removing Duplicate Files..."), 
		wxT("This may take a minute if the playlist is very large."), 100, GetParent(), 
		wxPD_AUTO_HIDE | wxPD_SMOOTH | wxPD_CAN_ABORT );
	progress->Show();
	progress->SetFocus();

	// Counts the number of times each path occurs
	wxArrayInt dupItems;
	map<wxString, int> dupMap;
	for ( size_t i = 0; i < GetItemCount(); ++i )
	{		
		xaplPlaylistItem *data = GetItemById( GetItemIdByIndex( (size_t) i ) );
		dupMap[data->path]++;

		if ( !progress->Update( (int) ((double) i / (double) GetItemCount() * 50.0) ) )
		{
			delete progress;
			wxMessageBox( wxT("No duplicates were removed from the list."), wxT("Xapl"), 
				wxICON_INFORMATION | wxOK );
			return;
		}
	}

	// Find all occurances of each path after the first one
	bool firstSkipped;
	size_t pos = 0;
	for ( std::map<wxString, int>::iterator it = dupMap.begin(); it != dupMap.end(); it++ )
	{
		if ( (*it).second > 1 )
		{
			firstSkipped = false;
			for ( size_t j = 0; j < GetPlaylistItemCount(); ++j )
			{
				if ( (*it).first == GetItemById( GetItemIdByIndex( j ) )->path )
				{
					if ( firstSkipped )
						dupItems.Add( (int)j );
					else
						firstSkipped = true;
				}
			}
		}

		if ( !progress->Update( (int) 48 + ((double) pos++ / (double) dupMap.size() * 50.0) ) )
		{
			delete progress;
			wxMessageBox( wxT("No duplicates were removed from the list."), wxT("Xapl"), 
				wxICON_INFORMATION | wxOK );
			return;
		}
	}

	// Remove extra occurances
	Freeze();
	dupItems.Sort( &xaplUtilities::IntSortFunc );
	DeleteItemsWithIndices( dupItems );
	progress->Update( 99 );

	m_dirty = true;
	SavePlaylist();
	progress->Update( 100 );

	Refresh();
	delete progress;
	
	DeselectAll();
	SetSelection( (int)selection );
	ScrollToSelection();
	Thaw();

	// Notify how many items were removed
	wxString msg = dupItems.GetCount() == 1 ? wxT("1 duplicate was removed from the list.") : wxString::Format( wxT("%i duplicates were removed from the list."), dupItems.GetCount() );
	wxMessageBox( msg, wxT("Xapl"), wxICON_INFORMATION | wxOK );
}

/* ====================================================================
 * ==== Misc. getters and setters =====================================*/

void xaplPlaylistCtrl::SetStyle( xaplPlaylistStyle style )
{
	m_style = style;
	SetBackgroundColour( m_style.bg );
	Refresh();
}

xaplPlaylistStyle xaplPlaylistCtrl::GetStyle( void ) const
{
	return m_style;
}

/* ====================================================================
 * ==== Search filter =================================================*/

int xaplPlaylistCtrl::SetFilter( const wxString &filter )
{
	// Reset list
	if ( filter == wxEmptyString  )
	{
		m_filter.clear();
		Freeze();
		
		// Show all playlist items
		m_visibleData.Empty();
		for ( size_t i = 0; i < GetPlaylistItemCount(); ++i )
			m_visibleData.push_back( (int)i );

		SetItemCount( m_visibleData.GetCount() );

		// Restore selection		
		if ( GetItemCount() > 0 )
		{
			DeselectAll();
			if ( GetPlayingItem() > -1 )
			{
				SetSelection( GetIndexByItemId( GetPlayingItem() ) );
				ScrollToLine( GetIndexByItemId( GetPlayingItem() ) );
			}
			else
			{
				SetSelection( 0 );
				ScrollToLine( 0 );
			}
		}

		Thaw();
		return -1;
	}

	// Only show items that match the filter
	Freeze();
	m_visibleData.Clear();
	for ( int i = 0; i < GetPlaylistItemCount(); ++i )
	{
		xaplPlaylistItem *data = GetItemByIndex( i );
		if ( data != NULL && xaplKmpSearchLib::KmpContains( data->path, filter, m_kmpTableBuilder->GetTable()[i] ) )
		{
			m_visibleData.Add( data->id );
		}
	}

	// Set new number of list items
	SetItemCount( m_visibleData.GetCount() );
	if ( GetItemCount() > 0 )
	{
		DeselectAll();
		SetSelection( 0 );
		ScrollToLine( 0 );
	}
	Thaw();

	return (int)GetItemCount();
}

void xaplPlaylistCtrl::ClearFilter( void )
{
	SetFilter( wxEmptyString );
}

/* ====================================================================
 * ==== ID <-> Index conversion =======================================*/
long xaplPlaylistCtrl::GetItemIdByIndex( size_t index ) const
{
	if ( index > GetItemCount() - 1 ) return -1;

	wxMutexLocker plDataVisLocker(*sm_dataVisMutex);
	return m_visibleData[index];
}

long xaplPlaylistCtrl::GetItemIdByListIndex( size_t idx ) const
{
	wxMutexLocker plDataVisLocker(*sm_dataVisMutex);
	return m_visibleData[idx];
}

int xaplPlaylistCtrl::GetIndexByItemId( long itemId ) const
{
	for ( size_t i = 0; i < GetPlaylistItemCount(); ++i )
	{
		if ( GetItemIdByIndex( i ) == itemId )
			return (int)i;
	}

	return -1;
}

/* ====================================================================
 * ==== Extended Data Access ==========================================*/
xaplPlaylistItem *xaplPlaylistCtrl::GetItemByListIndex( size_t idx ) const
{
	return GetItemById( GetItemIdByListIndex( idx ) );
}

xaplPlaylistItem *xaplPlaylistCtrl::GetFirstSelectedPlaylistItem( void )
{
	wxMutexLocker plDataVisLocker(*sm_dataVisMutex);
	return GetItemById( m_visibleData[GetFirstSelection()] );
}

/* ====================================================================
 * ==== Misc. Selection Helpers =======================================*/
long xaplPlaylistCtrl::GetFirstSelectedItemId( void ) const
{
	if ( GetItemCount() == 0 || GetSelectedCount() == 0 ) return -1;

	wxMutexLocker plDataVisLocker(*sm_dataVisMutex);
	return m_visibleData[GetFirstSelection()];
}

void xaplPlaylistCtrl::ReloadFirstSelectedItem( void )
{
	wxMutexLocker plDataVisLocker(*sm_dataVisMutex);

	size_t sel = GetFirstSelection();
	FormatItem( GetItemById( GetItemIdByIndex( m_visibleData[sel] ) ) );
	m_dirty = true;

	wxVListBox::RefreshAll();
}

int xaplPlaylistCtrl::GetSelections( wxArrayInt &sel ) const
{
	unsigned long cookie;
	int item = GetFirstSelected( cookie );
	while ( item != wxNOT_FOUND )
	{
		sel.Add( item );
		item = GetNextSelected( cookie );
	}

	return (int)sel.GetCount();
}

int xaplPlaylistCtrl::GetSelectedIds( wxArrayInt &sel ) const
{
	GetSelections( sel );
	for ( size_t i = 0; i < sel.GetCount(); ++i )
	{
		sel[i] = GetItemIdByIndex( sel[i] );
	}

	return (int)sel.GetCount();
}

size_t xaplPlaylistCtrl::GetFirstSelection( void ) const
{
	unsigned long cookie;
	return GetFirstSelected(cookie);
}

const wxString &xaplPlaylistCtrl::GetFirstSelectedFilePath( void ) const
{
	size_t index = GetFirstSelection();
	wxMutexLocker plDataVisLocker(*sm_dataVisMutex);
	xaplPlaylistItem *data = GetItemById( m_visibleData[index] );
	return data->path;
}

/* ====================================================================
 * ==== Scroll helpers ================================================*/

void xaplPlaylistCtrl::ScrollToSelection( void )
{
	size_t sel = GetFirstSelection();
	if ( sel >= GetVisibleBegin() && sel <= GetVisibleEnd() ) return;
	ScrollToLine( sel );
}

void xaplPlaylistCtrl::ScrollToPlayingItem( bool select )
{
	int idx = GetIndexByItemId( GetPlayingItem() );
	if ( idx < 0 || idx > (int) (GetItemCount() - 1) ) return;
	if ( select )
	{
		DeselectAll();
		SetSelection( idx );
	}
	ScrollToLine( idx );
}

/* ====================================================================
 * ==== Event handlers ================================================*/

void xaplPlaylistCtrl::OnAddItem( wxCommandEvent &evt )
{
	xaplPlaylistItem *item = static_cast<xaplPlaylistItem *>( evt.GetClientData() );
	xaplPlaylist::AddItem( item );

	// Don't show new items if a search is active... they will be shown when the search is cleared
	if ( m_searchStates.size() <= 0 )
	{
		wxMutexLocker plDataVisLocker(*sm_dataVisMutex);
		m_visibleData.push_back( item->id );
	}
}

void xaplPlaylistCtrl::OnBackgroundOpComplete( wxCommandEvent &evt )
{
	SetItemCount( m_visibleData.GetCount() );
	Refresh();

	xaplPlaylistWindow *mainWnd = dynamic_cast<xaplPlaylistWindow *>( GetParent() );
	mainWnd->UpdateSongCountStatusText();
	mainWnd->RestoreTempStatusText();

	m_kmpTableBuilder->RebuildTable();

	m_dirty = true;
}

void xaplPlaylistCtrl::OnPlaylistItemOpComplete( wxCommandEvent &evt )
{
	// Skip item add events if the background loader is working
	if ( m_fileLoader->HasWork() && evt.GetInt() != xaplIO_ITEMS_REMOVED )
		return;

	wxMutexLocker plDataVisLocker(*sm_dataVisMutex);
	wxArrayInt *items = static_cast<wxArrayInt *>( evt.GetClientData() );
	xaplPlaylistWindow *mainWnd;

	switch ( evt.GetInt() )
	{
	// Need to mark new items as visible...
	case xaplIO_ITEMS_ADDED:
	case xaplIO_ITEMS_INSERTED:
		m_dirty = true;
	case xaplIO_PLAYLIST_LOADED:
		m_visibleData.Clear();
		if ( items != NULL )
		{
			for ( size_t i = 0; i < items->GetCount(); ++i )
				m_visibleData.Add( items->Item( i ) );
		}

		SetItemCount( GetPlaylistItemCount() );
		Refresh();

		m_kmpTableBuilder->RebuildTable();

		mainWnd = dynamic_cast<xaplPlaylistWindow *>( GetParent() );
		mainWnd->UpdateSongCountStatusText();
		break;
	case xaplIO_ITEMS_REMOVED:
		m_dirty = true;
		break;
	case xaplIO_ALL_ITEMS_REMOVED:
		m_dirty = true;

		m_visibleData.Clear();
		SetItemCount( 0 );
		mainWnd = dynamic_cast<xaplPlaylistWindow *>( GetParent() );
		mainWnd->UpdateSongCountStatusText();
		break;
	case xaplIO_ITEM_MOVED:
		long id = items->Item( 0 );
		size_t src = items->Item( 1 );
		size_t dest = items->Item( 2 );

		m_visibleData.RemoveAt( src );
		m_visibleData.Insert( id, dest );
		break;
	}

	delete items;
}

void xaplPlaylistCtrl::OnKmpTableRebuildComplete( wxCommandEvent &evt )
{
	
}

void xaplPlaylistCtrl::OnPlaylistItemQueued( wxCommandEvent &evt )
{
	Refresh();
}

void xaplPlaylistCtrl::OnPlaylistItemDequeued( wxCommandEvent &evt )
{
	Refresh();
}

void xaplPlaylistCtrl::OnMouseLeftDown( wxMouseEvent &evt )
{
	m_dragging = true;
	m_dragStart = HitTest( evt.GetX(), evt.GetY() );
	evt.Skip();
}

void xaplPlaylistCtrl::OnMouseLeftUp( wxMouseEvent &evt )
{
	m_dragging = false;
	m_dragStart = 0;
	evt.Skip();
}

void xaplPlaylistCtrl::OnMouseMotion( wxMouseEvent &evt )
{
	if ( m_dragging && GetSelectedCount() < GetItemCount() )
	{
		int newStart = HitTest( evt.GetX(), evt.GetY() );
		if ( newStart > (int) GetItemCount() || newStart < 0 ) return;

		wxArrayInt sels;
		GetSelections( sels );
		if ( sels.GetCount() == 0  || newStart == m_dragStart || sels.GetCount() > 50 ) 
		{
			evt.Skip();
			return;
		}

		int amnt = newStart - (int)m_dragStart;
		if ( amnt < 0 ) // move down
		{
			for ( size_t i = 0; i < sels.GetCount(); ++i )
			{
				if ( sels[i] + amnt < 0 ) break;
				
				Reorder( sels[i], sels[i] + amnt );
				Select( sels[i], false );
				Select( sels[i] + amnt, true );
			}
		}
		else // move up (need to process selections in reverse)
		{
			for ( int i = (int)sels.GetCount() - 1; i >= 0; --i )
			{
				if ( sels[i] + amnt >= (int)GetItemCount() ) break;

				Reorder( sels[i], sels[i] + amnt );
				Select( sels[i], false );
				Select( sels[i] + amnt, true );
			}
		}
		m_dragStart = newStart;
	}
	evt.Skip();
}

// Let the global key handler handle key events for this control
void xaplPlaylistCtrl::OnKeyDown( wxKeyEvent &evt )
{
	return;
}
