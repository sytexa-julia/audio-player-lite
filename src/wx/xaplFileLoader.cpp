#include <wx/wx.h>
#include <wx/file.h>
#include <wx/dir.h>
#include <wx/filename.h>
#include "xaplFileLoader.h"

#ifdef _DEBUG
#include <crtdbg.h>
#define DEBUG_NEW new(_NORMAL_BLOCK ,__FILE__, __LINE__)
#else
#define DEBUG_NEW new
#endif
#ifdef _DEBUG
#define new DEBUG_NEW
#endif 

xaplFileLoader::xaplFileLoader( xaplPlaylistCtrl *playlist )
	: wxThread(), m_playlist( playlist )
{ }

void xaplFileLoader::AddWork( const wxString &file )
{
	gs_fileLoaderMutex->Lock();
	m_fileList.Add( file );
	gs_fileLoaderMutex->Unlock();
}

void xaplFileLoader::AddWork( const wxArrayString &fileList )
{
	gs_fileLoaderMutex->Lock();
	for ( size_t i = 0; i < fileList.GetCount(); ++i )
		m_fileList.Add( fileList[i] );
	gs_fileLoaderMutex->Unlock();
}

void xaplFileLoader::CancelWork( void )
{
	gs_fileLoaderMutex->Lock();
	m_fileList.Clear();
	gs_fileLoaderMutex->Unlock();
}

bool xaplFileLoader::HasWork( void )
{
	wxMutexLocker lock( *gs_fileLoaderMutex );
	return m_fileList.GetCount() > 0;
}

void *xaplFileLoader::Entry( void )
{
	while ( IsAlive() && !TestDestroy() && !IsPaused() )
	{
		while ( m_fileList.GetCount() > 0 )
		{
			if ( !IsAlive() || TestDestroy() )
				break;

			AddFile( m_fileList.Item( 0 ) );
			
			gs_fileLoaderMutex->Lock();
			m_fileList.RemoveAt( 0 );
			gs_fileLoaderMutex->Unlock();

			if ( m_fileList.GetCount() == 0 )
			{
				wxCommandEvent evt;
				evt.SetId( wxID_ANY );
				evt.SetEventType( xaplEVT_BACKGROUND_OP_COMPLETE );
				wxPostEvent( m_playlist, evt );
			}

			wxThread::Sleep( 5 );
		}

		wxThread::Sleep( 250 );
	}

	return NULL;
}

void xaplFileLoader::AddFile( const wxString &file )
{
	bool isDir = wxDir::Exists(file);

	// If it's a directory, add all supported music files within the directory
	if ( isDir )
	{
		wxArrayString files;
		wxDir::GetAllFiles( file, &files, wxT("*.*"), wxDIR_FILES | wxDIR_DIRS );
		
		bool ok;
		for ( size_t i = 0; i < files.Count(); ++i )
		{
			ok = false;
			wxFileName fn( files.Item( i ) );

			// Ensure the file has an allowed extension
			ok = ok || xaplAudio::GetInstance()->IsExtensionPlayable( wxT("*.") + fn.GetExt() );

			if ( !ok ) continue;

			// Make sure it's the full path
			fn.MakeAbsolute();	

			// Create a playlist item (xaplPlaylist will assign the next available ID for us)
			xaplPlaylistItem *item = xaplCreatePlaylistItem( -1, wxEmptyString, fn.GetFullPath(), fn.GetFullName() );
			
			if ( item != NULL )
			{
				// File in the displayText property
				m_playlist->FormatItem( item );

				// Send event to GUI thread to add the item
				wxCommandEvent evt;
				evt.SetId( wxID_ANY );
				evt.SetEventType( xaplEVT_ADD_ITEM );
				evt.SetClientData( item );
				wxPostEvent( m_playlist, evt );
			}
		}
	}
	// Otherwise, make sure the file exists and is a supported type, and add it
	else if ( wxFile::Exists( file.c_str() ) )
	{
		wxFileName fn( file );
		wxString ext = fn.GetExt();
		bool ok = false;

		// Ensure the file has an allowed extension
		ok = ok || xaplAudio::GetInstance()->IsExtensionPlayable( wxT("*.") + fn.GetExt() );

		// Make sure it's the full path
		fn.MakeAbsolute();		
		
		// Create a playlist item
		m_playlist->sm_nextIdMutex->Lock();
		xaplPlaylistItem *item = xaplCreatePlaylistItem( m_playlist->m_nextId++, wxEmptyString, fn.GetFullPath(), fn.GetFullName() );
		m_playlist->sm_nextIdMutex->Unlock();
		
		if ( item != NULL )
		{
			// File in the displayText property
			m_playlist->FormatItem( item );

			// Send event to GUI thread to add the item
			wxCommandEvent evt;
			evt.SetId( wxID_ANY );
			evt.SetEventType( xaplEVT_ADD_ITEM );
			evt.SetClientData( item );
			wxPostEvent( m_playlist, evt );
		}
	}
}
