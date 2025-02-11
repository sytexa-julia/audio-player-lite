#ifndef __XAPL_STATIC_TEXT_H
#define __XAPL_STATIC_TEXT_H

#include <wx/wx.h>

// To reduce flicker in the playback timer labels
class xaplStaticText
	: public wxStaticText
{
public:
	 xaplStaticText(wxWindow* parent, wxWindowID id, const wxString& label, const wxPoint& pos = wxDefaultPosition,
                                       const wxSize& size = wxDefaultSize, long style = 0, const wxString& name= wxT("staticText") )
									   : wxStaticText( parent, id, label, pos, size, style, name ) { };

	void OnEraseBackGround( wxEraseEvent &evt ) { };
    DECLARE_EVENT_TABLE()
};

BEGIN_EVENT_TABLE( xaplStaticText,wxStaticText )
    EVT_ERASE_BACKGROUND( xaplStaticText::OnEraseBackGround )
END_EVENT_TABLE()

#endif
