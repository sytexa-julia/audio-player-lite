#ifndef __X_UPDATE_CHECK_H
#define __X_UPDATE_CHECK_H

#include <wx/wx.h>
#include <wx/sstream.h>
#include <wx/protocol/http.h>
#include "xUpdateInfoDlg.h"
#include "xAutoUpdater.h"

#if defined(__WINDOWS__)
  #define OS_NAME wxString( wxT("Windows") )
#elif defined(__LINUX__)
  #define OS_NAME wxString("Linux")
#elif defined(__MACH__)
  #define OS_NAME wxString("OS X")
#else
  #define OS_NAME wxString("Other")
#endif

typedef enum
{
	UpdateMode_LaunchBrowser = 0,
	UpdateMode_NotifyOnly,
	UpdateMode_AutoInstall
} xUpdateCheckMode;

typedef struct
{
	wxString latestVersion;
	wxString productHomepage;
	wxString updateType;
	wxString updateArchiveLocation;
	wxString updateArchiveFormat;
	wxString updateDescription;
} xBroadcastInfo;

class xUpdateCheck
{
private:
	wxString m_productName;
	wxString m_localVersion;
	wxString m_broadcastPage;
	wxString m_installLocation;
	wxString m_mainExecutable;
	int m_updateMode;

protected:
	bool ParseBroadcastUrl( const wxString &url );
	xBroadcastInfo *ParseBroadcastData( const wxString &data );

public:
	xUpdateCheck( const wxString &productName, const wxString &localVersion, const wxString &broadcastPage, const wxString &installLocation, const wxString &mainExe );
	xUpdateCheck( const wxString &productName, const wxString &localVersion, const wxString &broadcastPage, const wxString &installLocation, const wxString &mainExe, int updateMode );
	~xUpdateCheck( void );

	void SetUpdateMode( int mode );
	int  GetUpdateMode( void );

	bool PerformCheck( void );
};

#endif
