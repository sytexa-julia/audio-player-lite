#ifndef __XAPL_PROPERTIES_DIALOG_H
#define __XAPL_PROPERTIES_DIALOG_H

#include <wx/wx.h>
#include <wx/gbsizer.h>
#include <wx/statline.h>
#include "xaplPlaylistCtrl.h"

class xaplPropertiesDialog :
	public wxDialog
{
private:
	wxStaticText *m_lblRenameFile;
	wxStaticText *m_lblChangeM3uTitle;

	wxTextCtrl *m_txtFileName;
	wxTextCtrl *m_txtM3uTitle;
	wxTextCtrl *m_txtData;

	wxButton *m_btnOk;
	wxButton *m_btnCancel;

	xaplPlaylistItem *m_plItem;

protected:
	void Initialize( void );

public:
	xaplPropertiesDialog( wxWindow *parent, xaplPlaylistItem *plItem );
	~xaplPropertiesDialog( void );

	int ShowModal( void );

	bool LoadData( void );
	int CommitChanges( void );
};

#endif
