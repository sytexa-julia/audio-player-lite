#ifndef __X_AUTO_UPDATER_APP
#define __X_AUTO_UPDATER_APP

#include <wx/wx.h>
#include <wx/fs_zip.h>
#include "xUpdateCheck.h"

class xAutoUpdaterApp : 
	public wxApp
{
private:
	xUpdateCheck *m_updateCheck;

public:
	virtual bool OnInit( void );
};

DECLARE_APP( xAutoUpdaterApp )

#endif
