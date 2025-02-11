/* 
  Copyright (c) 2009, Xive Software
  All rights reserved.
  
  Redistribution and use in source and binary forms, with or without modification, 
  are permitted provided that the following conditions are met:

     * Redistributions of source code must retain the above copyright notice, this 
	   list of conditions and the following disclaimer.
     * Redistributions in binary form must reproduce the above copyright notice, 
	   this list of conditions and the following disclaimer in the documentation 
	   and/or other materials provided with the distribution.
     * Neither the name of Xive Software nor the names of its contributors 
	   may be used to endorse or promote products derived from this software 
	   without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY 
  EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES 
  OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT 
  SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, 
  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED 
  TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR 
  BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN 
  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH 
  DAMAGE.
*/

#include "xaplAboutDialog.h"

#ifdef _DEBUG
#include <crtdbg.h>
#define DEBUG_NEW new(_NORMAL_BLOCK ,__FILE__, __LINE__)
#else
#define DEBUG_NEW new
#endif
#ifdef _DEBUG
#define new DEBUG_NEW
#endif 

xaplAboutDialog::xaplAboutDialog( wxWindow* parent, 
								  wxWindowID id ) 
	: wxDialog( parent, 
				id, 
				wxT("About Xapl"), 
				wxDefaultPosition, 
				wxDefaultSize, 
				wxDEFAULT_DIALOG_STYLE ) 
{	
	Initialize();
}

xaplAboutDialog::~xaplAboutDialog( void )
{}

void xaplAboutDialog::Initialize( void )
{
	// Controls
	wxMemoryInputStream istream( xaplAppIcon, sizeof xaplAppIcon );
	wxImage icoImg( istream, wxBITMAP_TYPE_PNG );
	m_progIcon = new wxStaticBitmap( this, wxID_ANY, wxBitmap( icoImg ) );
	
	m_lblTitle = new wxStaticText( this, wxID_ANY, wxT("Xapl"), wxDefaultPosition, wxDefaultSize );
	m_lblTitle->SetFont( wxFont( 18, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD ) );
	m_lblVersion = new wxStaticText( this, wxID_ANY, wxT("Version ") + APP_VERSION + wxT(" for ") + APP_PLATFORM, wxDefaultPosition, wxDefaultSize );
	m_lblVersion->SetFont( wxFont( 13, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL ) );
	m_lblCopyright = new wxStaticText( this, wxID_ANY, wxT("\u00A9 2009"), wxDefaultPosition, wxDefaultSize );
	m_lblCopyright->SetFont( wxFont( 10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL ) );
	
	m_lblUses = new wxStaticText( this, wxID_ANY, wxT("Xapl makes use of:"), wxDefaultPosition, wxDefaultSize );
	m_lblUses->SetFont( wxFont( 10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL ) );
	m_lnkXive = new wxHyperlinkCtrl( this, wxID_ANY, wxT("Xive Software"), wxT("http://www.xivesoftware.com") );
	m_lnkXive->SetFont( wxFont( 10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL ) );
	m_lnkTango = new wxHyperlinkCtrl( this, wxID_ANY, wxT("Tango Icons"), wxT("http://tango.freedesktop.org/Tango_Icon_Gallery") );
	m_lnkTango->SetFont( wxFont( 10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL ) );
	m_lnkBass = new wxHyperlinkCtrl( this, wxID_ANY, wxT("Un4seen BASS"), wxT("http://www.un4seen.com") );
	m_lnkBass->SetFont( wxFont( 10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL ) );
	m_lnkTagLib = new wxHyperlinkCtrl( this, wxID_ANY, wxT("TagLib"), wxT("http://developer.kde.org/~wheeler/taglib.html") );
	m_lnkTagLib->SetFont( wxFont( 10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL ) );

	m_btnClose = new wxButton( this, wxID_CANCEL, wxT("&Close") );

	// Layout
	wxGridBagSizer* gridbag = new wxGridBagSizer();
	gridbag->AddGrowableCol( 0 );
	gridbag->SetEmptyCellSize( wxSize(10, 20) );

	wxFlexGridSizer* headerSizer = new wxFlexGridSizer( 6, 2, 0, 0 );
	wxFlexGridSizer* copyrightSizer = new wxFlexGridSizer( 1, 2, 0, 6 );
	wxFlexGridSizer* usesSizer = new wxFlexGridSizer( 3, 2, 0, 6 );

	// Icon and app name
	headerSizer->Add( m_progIcon, 0, wxALIGN_CENTER_VERTICAL );
	headerSizer->Add( m_lblTitle, 0, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL | wxLEFT, 12 );
	
	// Version
	headerSizer->AddSpacer( 1 );
	headerSizer->Add( m_lblVersion, 0, wxLEFT | wxALIGN_CENTER_VERTICAL, 14 );

	// Copyright notice
	headerSizer->AddSpacer( 1 );
	copyrightSizer->Add( m_lblCopyright, 1, wxALIGN_LEFT );
	copyrightSizer->Add( m_lnkXive, 1, wxALIGN_LEFT );
	headerSizer->Add( copyrightSizer, 0, wxALIGN_LEFT | wxLEFT, 14 );

	// Acknowledgements
	headerSizer->AddSpacer( 1 );	
	usesSizer->Add( m_lblUses, 0, wxALIGN_LEFT );
	usesSizer->Add( m_lnkBass, 0, wxALIGN_LEFT | wxRIGHT, 40 );	// pad out the right side of the dialog so it looks balanced
	usesSizer->AddSpacer( 1 );
	usesSizer->Add( m_lnkTango, 0, wxALIGN_LEFT );
	usesSizer->AddSpacer( 1 );
	usesSizer->Add( m_lnkTagLib, 0, wxALIGN_LEFT );
	headerSizer->Add( usesSizer, 0, wxALIGN_LEFT | wxLEFT | wxTOP, 14 );

	// Put everything together...
	gridbag->Add( headerSizer, wxGBPosition(0, 0), wxGBSpan(1, 2), wxLEFT | wxTOP, 8 );
	gridbag->Add( m_btnClose, wxGBPosition(2, 0), wxGBSpan(1, 2), wxALIGN_RIGHT | wxALL, 5 );

	gridbag->Fit( this );
	gridbag->SetSizeHints( this );
	SetSizer( gridbag );
}
