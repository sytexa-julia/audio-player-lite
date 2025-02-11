#ifndef __XAPL_ABOUT_DIALOG_H
#define __XAPL_ABOUT_DIALOG_H

#include "wx/wx.h"
#include "wx/wxhtml.h"
#include "wx/gbsizer.h"

class xaplAboutDialog :
	protected wxDialog
{
private:
	wxGridBagSizer *gridbag;

	wxStaticBitmap *progIcon;

	wxStaticText *lblTitle;
	wxStaticText *lblVersion;
	wxStaticText *lblCopyright;
	wxStaticText *lblFreeSoftware;

	wxHtmlWindow *gplDisplay;

	wxButton *btnClose;

public:
	xaplAboutDialog( wxWindow *parent, wxWindowID id = wxID_ANY );
	~xaplAboutDialog( void );

	void Show( void );

	void OnClose( wxCommandEvent &WXUNUSED( evt ) );

	DECLARE_EVENT_TABLE()
};

enum AboutControls
{
	ID_Control_GplDisplay = 1,
	ID_Control_BtnClose
};

#endif
