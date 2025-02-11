#ifndef __XAPL_FILE_LOADER_H
#define __XAPL_FILE_LOADER_H

#include <wx/thread.h>

#include "xaplAudio.h"
#include "xaplApplication.h"
#include "xaplPlaylistCtrl.h"

static wxMutex *gs_fileLoaderMutex = new wxMutex( wxMUTEX_RECURSIVE );

class xaplFileLoader :
	public wxThread
{
private:
	xaplPlaylistCtrl *m_playlist;
	wxArrayString m_fileList;

protected:
	void AddFile( const wxString &file );

public:
	xaplFileLoader( xaplPlaylistCtrl *playlist );
	void AddWork( const wxString &file );
	void AddWork( const wxArrayString &fileList );
	void CancelWork( void );
	bool HasWork( void );

	virtual void *Entry( void );
};

#endif
