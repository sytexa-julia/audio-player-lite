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

#ifndef __XAPL_KMP_SEARCH_LIB_H
#define __XAPL_KMP_SEARCH_LIB_H

#include <wx/wx.h>
#include <wx/thread.h>

class xaplPlaylistCtrl;

#define xaplNOT_FOUND -1

// Implements the KMP string search algorithm and 
// partial match table builder.
class xaplKmpSearchLib
{
public:
	// Convenience method returns true if needle is 
	// found in haystack, returns false otherwise
	static bool KmpContains( const wxString &haystack, const wxString &needle, const int *table, bool ignoreCase = true );
	
	// Get the 0-based location of string needle in
	// string haystack, or xaplNOT_FOUND if not found
	static int KmpSearch( const wxString &haystack, const wxString &needle, const int *table, bool ignoreCase = true );
	
	// Builds a partial match table for the specified 
	// haystack string
	static int* BuildKmpTable( const wxString &haystack, bool ignoreCase = true );

	// Builds a partial match table for all strings in
	// the haystack array
	static int** BuildKmpTable( const wxArrayString &haystack, bool ignoreCase = true );
};

// Events for xaplKmpTableBuilderThread
BEGIN_DECLARE_EVENT_TYPES()
	DECLARE_EVENT_TYPE(xaplEVT_KMP_TABLE_REBUILD_COMPLETE, 441)
END_DECLARE_EVENT_TYPES()

#define EVT_KMP_TABLE_REBUILD_COMPLETE(fn) \
    DECLARE_EVENT_TABLE_ENTRY( \
        xaplEVT_KMP_TABLE_REBUILD_COMPLETE, wxID_ANY, wxID_ANY, \
        (wxObjectEventFunction)(wxEventFunction) wxStaticCastEvent( wxCommandEventFunction, &fn ), \
        (wxObject *) NULL \
    ),

// A thread for rebuilding a KMP table in the
// background
class xaplKmpTableBuilderThread
	: public wxThread
{
private:
	xaplPlaylistCtrl* m_playlistCtrl;
	int** m_table;
	bool m_doRebuild;

public:
	// Constructor.
	xaplKmpTableBuilderThread( xaplPlaylistCtrl* playlist );
	// Destructor.
	~xaplKmpTableBuilderThread( void );

	// Triggers a rebuild of the KMP partial match table.
	void RebuildTable( void );
	// Get if the table is being rebuilt
	bool IsRebuilding( void );
	// Get the partial match table
	int** GetTable();

	// Entry point of the thread.
	virtual void* Entry( void );
};

#endif
