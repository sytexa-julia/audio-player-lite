#ifndef __XAPL_SETTINGS_UI_H
#define __XAPL_SETTINGS_UI_H

#include <wx/wx.h>
#include <wx/spinctrl.h>
#include <wx/gbsizer.h>
#include <wx/notebook.h>
#include "xaplPlaylistCtrl.h"

class xaplListColoursPreview
	: public wxPanel
{
protected:
	const wxString m_label;

public:
	xaplListColoursPreview( wxWindow *parent, const wxString &text );
	xaplListColoursPreview( wxWindow *parent, const wxString &text, const wxFont &font, const wxColour &bg, const wxColour &fg );

	wxSize GetClientSize( void ) const;
	wxSize GetSize( void ) const;

	void PaintBackground( wxDC &dc );

	void OnPaint( wxPaintEvent &evt );
	void OnEraseBackground( wxEraseEvent &evt );

	DECLARE_EVENT_TABLE()
};

class xaplSettingsDialog :
	public wxDialog
{
private:
	wxNotebook *nb;
	wxPanel *generalPage;
	wxPanel *audioPage;
	wxPanel *appearancePage;

	wxGridBagSizer *gridbag;
	wxGridBagSizer *generalGB;
	wxGridBagSizer *audioGB;
	wxGridBagSizer *appearanceGB;

	// Header
	wxStaticBitmap *imgSettings;
	wxStaticText *lblTitle;

	// General
	wxStaticText *lblDefaultForTypes;
	wxCheckBox *chkDefaultMP3;
	wxCheckBox *chkDefaultOGG;
	wxCheckBox *chkDefaultWAV;
	wxCheckBox *chkDefaultWMA;
	wxCheckBox *chkDefaultAAC;
	wxCheckBox *chkDefaultFLAC;
	wxCheckBox *chkSingleInstance;
	wxCheckBox *chkAlwaysOnTop;
	wxCheckBox *chkMinimizeToTray;
	wxCheckBox *chkAutoRemoveInvalid;

	// Audio
	wxTextCtrl *lblReqRestart;
	wxStaticText *lblBuffer;
	wxSpinCtrl *spnBuffer;
	wxCheckBox *chkSoftwareMixing;

	// Appearance
#ifdef __WINDOWS__
	wxStaticText *lblTransparency;
#endif
	xaplListColoursPreview *lblNormalItem;
	xaplListColoursPreview *lblSelectedItem;
	xaplListColoursPreview *lblQueuedItem;
	xaplListColoursPreview *lblNowPlayingItem;
#ifdef __WINDOWS__
	wxSlider *sldTransparency;
#endif
	wxButton *btnBgColor;
	wxButton *btnFgColor;
	wxButton *btnSelBgColor;
	wxButton *btnSelFgColor;
	wxButton *btnQBgColor;
	wxButton *btnQFgColor;
	wxButton *btnNpBgColor;
	wxButton *btnNpFgColor;

	wxButton *btnOk;
	wxButton *btnCancel;

	int m_buffer;
	bool m_forceSoftware;
	long m_transparency;
	xaplPlaylistStyle m_plStyle;

protected:
	void Initialize( void );

public:
	xaplSettingsDialog( wxWindow *parent );
	~xaplSettingsDialog(void);

	void Commit( void );

	void OnButtonBgColor( wxCommandEvent &WXUNUSED(evt) );
	void OnButtonFgColor( wxCommandEvent &WXUNUSED(evt) );
	void OnButtonSelBgColor( wxCommandEvent &WXUNUSED(evt) );
	void OnButtonSelFgColor( wxCommandEvent &WXUNUSED(evt) );
	void OnButtonQBgColor( wxCommandEvent &WXUNUSED(evt) );
	void OnButtonQFgColor( wxCommandEvent &WXUNUSED(evt) );
	void OnButtonNpBgColor( wxCommandEvent &WXUNUSED(evt) );
	void OnButtonNpFgColor( wxCommandEvent &WXUNUSED(evt) );

	DECLARE_EVENT_TABLE();
};

#endif
