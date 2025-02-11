#ifndef __XAPL_APPLICATION_H
#define __XAPL_APPLICATION_H

#include <wx/wx.h>
#include "xaplMainWindow.h"

class xaplApplication : 
	public wxApp
{
    xaplMainWindow *ui;
	wxObject dummy;

	virtual bool OnInit( void );
	virtual int FilterEvent( wxEvent &evt );
};

DECLARE_APP( xaplApplication )

#endif
