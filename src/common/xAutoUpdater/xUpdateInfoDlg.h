#ifndef __X_UPDATE_INFO_DLG_H
#define __X_UPDATE_INFO_DLG_H

#include <wx/wx.h>
#include <wx/wxhtml.h>
#include <wx/gbsizer.h>

#ifdef __WINDOWS__
   #include "resource.h"
#else
   #include "../../art/updater.xpm"
#endif

typedef enum
{
	Button_InstallLater,
	Button_InstallNow
} xUpdateInfoDlgControlIds;

class xUpdateInfoDlg :
	public wxDialog
{
private:
	wxStaticBitmap *m_appIcon;
	wxStaticText *m_updateAvailable;
	wxTextCtrl *m_updateShortInfo;
	wxStaticText *m_releaseNotes;

	wxHtmlWindow *m_updateFullInfo;

	wxButton *m_installLater;
	wxButton *m_installNow;

	wxGridBagSizer *m_gridbag;

protected:
	void Initialize( const wxString &appIconPath, const wxString &appName, 
		const wxString &installedVersion, const wxString &newVersion, 
		const wxString &updateInfo );

public:
	xUpdateInfoDlg( const wxString &appIconPath, const wxString &appName, 
		const wxString &installedVersion, const wxString &newVersion, 
		const wxString &updateInfo );
	~xUpdateInfoDlg( void );

	void OnSize( wxSizeEvent &evt );
	void OnShortDescMouseDown( wxMouseEvent &evt );
	void OnButtonInstallLater( wxCommandEvent &WXUNUSED(evt) );
	void OnButtonInstallNow( wxCommandEvent &WXUNUSED(evt) );

	DECLARE_EVENT_TABLE();
};

#endif
