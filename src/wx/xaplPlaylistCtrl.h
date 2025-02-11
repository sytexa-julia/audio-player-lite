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

#ifndef __XAPL_PLAYLIST_CTRL_H
#define __XAPL_PLAYLIST_CTRL_H

#include <map>
#include <list>

#include <wx/wx.h>
#include <wx/dc.h>
#include <wx/dir.h>
#include <wx/file.h>
#include <wx/image.h>
#include <wx/vlbox.h>
#include <wx/fs_mem.h>
#include <wx/hashmap.h>
#include <wx/progdlg.h>
#include <wx/wfstream.h>
#include <wx/filename.h>
#include <wx/settings.h>
#include <wx/dcmemory.h>

#include "../common/xPlaylistLib/XplM3UPlaylist.h"

#include "xaplPlaylist.h"
#include "xaplConstants.h" 
#include "xaplUtilities.h"
#include "xaplKmpSearchLib.h"

using std::list;
using std::map;

// Structures
typedef struct
{
	wxFont font;
	wxColour bg;
	wxColour fg;
	wxColour selectionBg;
	wxColour selectionFg;
	wxColour queuedBg;
	wxColour queuedFg;
	wxColour nowPlayingBg;
	wxColour nowPlayingFg;
} xaplPlaylistStyle;

// Events
BEGIN_DECLARE_EVENT_TYPES()
	DECLARE_EVENT_TYPE(xaplEVT_ADD_ITEM, 331)
	DECLARE_EVENT_TYPE(xaplEVT_BACKGROUND_OP_COMPLETE, 332)
END_DECLARE_EVENT_TYPES()

#define EVT_ADD_ITEM(fn) \
    DECLARE_EVENT_TABLE_ENTRY( \
        xaplEVT_ADD_ITEM, wxID_ANY, wxID_ANY, \
        (wxObjectEventFunction)(wxEventFunction) wxStaticCastEvent( wxCommandEventFunction, &fn ), \
        (wxObject *) NULL \
    ),

#define EVT_BACKGROUND_OP_COMPLETE(fn) \
    DECLARE_EVENT_TABLE_ENTRY( \
        xaplEVT_BACKGROUND_OP_COMPLETE, wxID_ANY, wxID_ANY, \
        (wxObjectEventFunction)(wxEventFunction) wxStaticCastEvent( wxCommandEventFunction, &fn ), \
        (wxObject *) NULL \
    ),

// Class declaration
class xaplFileLoader;

class xaplPlaylistCtrl 
	: public wxVListBox, 
	  public xaplPlaylist
{
	friend class xaplFileLoader;

private:
	// the really important stuff
	wxArrayInt m_visibleData;
	xaplPlaylistStyle m_style;
	xaplFileLoader *m_fileLoader;
	xaplKmpTableBuilderThread *m_kmpTableBuilder;
	
	// search filter save states
	bool m_hasTempData;
	list<wxArrayInt *> m_searchStates;
	wxArrayString m_filter;
	
	// etc.
	size_t m_savedSelection;
	size_t m_dragStart;
	bool m_dragging;
	bool m_dirty;

protected:
	// wxVListBox implementation
	virtual void OnDrawBackground(wxDC& dc, const wxRect& rect, size_t n) const;
    virtual void OnDrawItem(wxDC& dc, const wxRect& rect, size_t n) const;
    virtual wxCoord OnMeasureItem(size_t n) const;

public:
	// Mutexes
	static wxMutex *sm_dataVisMutex;

	// Constructor / Destructor
	xaplPlaylistCtrl( wxWindow* parent, wxWindowID id );
	~xaplPlaylistCtrl( void );

	// Playing item management
	virtual void SetPlayingItem( long id, bool addToHistory = true );
	virtual long Next( bool setPlaying = true );
	virtual long Previous( bool setPlaying = true );

	// Item management
	void AddItem( const wxString &path, bool inferNetRadio = true );
	void AddItems( const wxArrayString &pathList, bool inferNetRadio = true );

	void Reorder( size_t source, size_t dest );

	void DeleteItemsWithIndices( wxArrayInt indices );
	void DeleteSelected( void );
	void Clear( void );

	void RemoveInvalidItems( void );
	void RemoveDuplicateItems( void );
	
	// Misc. getters and setters
	void SetStyle( xaplPlaylistStyle style );
	xaplPlaylistStyle GetStyle( void ) const;

	// Search filter
	int SetFilter( const wxString &filter );
	void ClearFilter( void );

	// ID <-> Index conversion
	long GetItemIdByIndex( size_t item ) const;
	long GetItemIdByListIndex( size_t idx ) const;
	int GetIndexByItemId( long itemId ) const;

	// Extended data access
	xaplPlaylistItem *GetItemByListIndex( size_t idx ) const;
	xaplPlaylistItem *GetFirstSelectedPlaylistItem( void );

	// Misc. selection helpers
	long GetFirstSelectedItemId( void ) const;
	void ReloadFirstSelectedItem( void );
	int GetSelections( wxArrayInt &sel ) const;
	int GetSelectedIds( wxArrayInt &sel ) const;
	size_t GetFirstSelection( void ) const;
	const wxString &GetFirstSelectedFilePath( void ) const;
	
	// Scroll helpers
	void ScrollToSelection( void );
	void ScrollToPlayingItem( bool select = false );

	// Event handlers
	void OnAddItem( wxCommandEvent &evt );
	void OnBackgroundOpComplete( wxCommandEvent &evt );

	void OnPlaylistItemOpComplete( wxCommandEvent &evt );
	void OnPlaylistItemQueued( wxCommandEvent &evt );
	void OnPlaylistItemDequeued( wxCommandEvent &evt );

	void OnKmpTableRebuildComplete( wxCommandEvent &evt );

	void OnMouseLeftDown( wxMouseEvent &evt );
	void OnMouseLeftUp( wxMouseEvent &evt );
	void OnMouseMotion( wxMouseEvent &evt );
	void OnKeyDown( wxKeyEvent &evt );

	DECLARE_EVENT_TABLE();
};

#endif
