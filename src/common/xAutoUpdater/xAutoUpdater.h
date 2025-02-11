#ifndef __X_AUTO_UPDATER_H
#define __X_AUTO_UPDATER_H

#include <memory>
#include <wx/wx.h>
#include <wx/dir.h>
#include <wx/ipc.h>
#include <wx/filesys.h>
#include <wx/gbsizer.h>
#include <wx/zipstrm.h>
#include <wx/wfstream.h>
#include <wx/filename.h>

#ifdef __WINDOWS__
   #include <wx/msw/registry.h>
#endif

#include "../../../lib/include/curl/curl.h"

#ifdef __WINDOWS__
   #include "resource.h"
#else
   #include "../../art/updater.xpm"
#endif

using std::auto_ptr;

#if defined(__WINDOWS__)
   #define DIR_SEP wxT("\\")
#else
   #define DIR_SEP wxT("/");
#endif

class xAutoUpdater;

/****** xAutoUpdaterDownloadThread Definition ******/
class xAutoUpdaterDownloadThread :
	public wxThread
{
private:
	xAutoUpdater *m_updater;

public:
	xAutoUpdaterDownloadThread( xAutoUpdater *updater );
	~xAutoUpdaterDownloadThread( void );

	virtual void *Entry( void );

	xAutoUpdater *GetUpdater( void );
};

/****** xAutoUpdaterWorkerThread Definition ******/
class xAutoUpdaterWorkerThread :
	public wxThread
{
private:
	xAutoUpdater *m_updater;

protected:
	bool PerformUpdate( void );
	bool CleanUp( void );

	bool ExtractZipFile( const wxString &file, const wxString &destination );
	
	bool ProgressPulse( void );

public:
	xAutoUpdaterWorkerThread( xAutoUpdater *updater );
	~xAutoUpdaterWorkerThread( void );

	virtual void *Entry( void );

	xAutoUpdater *GetUpdater( void );
};

/****** Custom Event Definition ******/
BEGIN_DECLARE_EVENT_TYPES()
	DECLARE_EVENT_TYPE(wxEVT_UPDATE_PROGRESS, 331)
	DECLARE_EVENT_TYPE(wxEVT_NOTIFY_FILE_NOT_FOUND, 332)
	DECLARE_EVENT_TYPE(wxEVT_NOTIFY_CURL_ERROR, 333)
	DECLARE_EVENT_TYPE(wxEVT_NOTIFY_TRANSFER_COMPLETE, 334)
	DECLARE_EVENT_TYPE(wxEVT_NOTIFY_UPDATE_SUCCESS, 431)
	DECLARE_EVENT_TYPE(wxEVT_NOTIFY_UPDATE_FAILURE, 432)
END_DECLARE_EVENT_TYPES()

#define EVT_UPDATE_PROGRESS(id, fn) \
    DECLARE_EVENT_TABLE_ENTRY( \
        wxEVT_UPDATE_PROGRESS, id, wxID_ANY, \
        (wxObjectEventFunction)(wxEventFunction) wxStaticCastEvent( wxCommandEventFunction, &fn ), \
        (wxObject *) NULL \
    ),

#define EVT_NOTIFY_FILE_NOT_FOUND(id, fn) \
    DECLARE_EVENT_TABLE_ENTRY( \
        wxEVT_NOTIFY_FILE_NOT_FOUND, id, wxID_ANY, \
        (wxObjectEventFunction)(wxEventFunction) wxStaticCastEvent( wxCommandEventFunction, &fn ), \
        (wxObject *) NULL \
    ),

#define EVT_NOTIFY_CURL_ERROR(id, fn) \
    DECLARE_EVENT_TABLE_ENTRY( \
        wxEVT_NOTIFY_CURL_ERROR, id, wxID_ANY, \
        (wxObjectEventFunction)(wxEventFunction) wxStaticCastEvent( wxCommandEventFunction, &fn ), \
        (wxObject *) NULL \
    ),

#define EVT_NOTIFY_TRANSFER_COMPLETE(id, fn) \
    DECLARE_EVENT_TABLE_ENTRY( \
        wxEVT_NOTIFY_TRANSFER_COMPLETE, id, wxID_ANY, \
        (wxObjectEventFunction)(wxEventFunction) wxStaticCastEvent( wxCommandEventFunction, &fn ), \
        (wxObject *) NULL \
    ),

#define EVT_NOTIFY_UPDATE_SUCCESS(id, fn) \
    DECLARE_EVENT_TABLE_ENTRY( \
        wxEVT_NOTIFY_UPDATE_SUCCESS, id, wxID_ANY, \
        (wxObjectEventFunction)(wxEventFunction) wxStaticCastEvent( wxCommandEventFunction, &fn ), \
        (wxObject *) NULL \
    ),

#define EVT_NOTIFY_UPDATE_FAILURE(id, fn) \
    DECLARE_EVENT_TABLE_ENTRY( \
        wxEVT_NOTIFY_UPDATE_FAILURE, id, wxID_ANY, \
        (wxObjectEventFunction)(wxEventFunction) wxStaticCastEvent( wxCommandEventFunction, &fn ), \
        (wxObject *) NULL \
    ),

/****** xAutoUpdater Definition ******/
class xAutoUpdater :
	public wxDialog
{
private:
	wxString m_productName;
	wxString m_installLocation;
	wxString m_newVersion;
	wxString m_mainExecutable;
	wxString m_updateArchive;
	wxString m_tempArchive;
	wxString m_archiveName;
	wxString m_tempDir;

	xAutoUpdaterDownloadThread *m_downloader;
	xAutoUpdaterWorkerThread *m_updateWorker;

	wxGridBagSizer *m_gridbag;
	wxStaticText *m_downloading;
	wxGauge *m_progressBar;
	wxButton *m_gButton;
	wxButton *m_cancel;

	bool m_transferComplete;
	bool m_cancelled;

protected:
	bool Initialize( const wxString &updateArchive );
	bool DownloadFile( void );

public:
	xAutoUpdater( const wxString &updateArchive, const wxString &productName, const wxString &installLocation, const wxString &mainExe, const wxString &newVersion );
	~xAutoUpdater( void );

	int ShowModal( void );

	void OnButtonClicked( wxCommandEvent &WXUNUSED(evt) );
	void OnCancelClicked( wxCommandEvent &WXUNUSED(evt) );
	void OnUpdateProgress( wxCommandEvent &evt );
	void OnFileNotFound( wxCommandEvent &WXUNUSED(evt) );
	void OnCURLError( wxCommandEvent &WXUNUSED(evt) );
	void OnTransferComplete( wxCommandEvent &WXUNUSED(evt) );
	void OnUpdateSuccess( wxCommandEvent &WXUNUSED(evt) );
	void OnUpdateFailure( wxCommandEvent &WXUNUSED(evt) );

	wxString &GetNewVersion( void );
	wxString &GetProductName( void );
	wxString &GetInstallLocation( void );
	wxString &GetMainExecutableName( void );
	wxString &GetUpdateArchive( void );
	wxString &GetTempArchive( void );
	wxString &GetTempDirectory( void );
	wxString &GetArchiveName( void );

	DECLARE_EVENT_TABLE();
};

// Attempts to copy a complete directory tree from one location to another.
// 'unprocessed' is filled with any files or directories that could not be moved.
class xDirMover :
	public wxDirTraverser
{
private:
	wxString m_source;
	wxString m_dest;
	wxArrayString m_unprocessed;

public:
    xDirMover( const wxString &source, const wxString &dest, wxArrayString &unprocessed ) 
		: m_source(source), m_dest( dest ), m_unprocessed( unprocessed )
	{ }

    virtual wxDirTraverseResult OnFile( const wxString &fileName )
    {
		wxString dPath = m_dest + fileName.Right( fileName.Length() - m_source.Length() );
		if ( !wxRenameFile( fileName, dPath ) )
			m_unprocessed.Add( fileName );

        return wxDIR_CONTINUE;
    }

    virtual wxDirTraverseResult OnDir( const wxString &dirName )
    {
        wxString dPath = m_dest + dirName.Right( dirName.Length() - m_source.Length() );
		if ( !wxRenameFile( dirName, dPath ) )
			m_unprocessed.Add( dirName );

		return wxDIR_CONTINUE;
    }
};

#endif
