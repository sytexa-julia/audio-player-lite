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

#include "xaplSleepTimer.h"
#include "xaplBitmaps.h"
#include "xaplPlaylistWindow.h"
#include <wx/mstream.h>

#ifdef __WINDOWS__
  #include <PowrProf.h>
#endif

#ifdef _DEBUG
#include <crtdbg.h>
#define DEBUG_NEW new(_NORMAL_BLOCK ,__FILE__, __LINE__)
#else
#define DEBUG_NEW new
#endif
#ifdef _DEBUG
#define new DEBUG_NEW
#endif 

/* ====================================================================
 * ==== xaplSleepTimerCountdownUI Definition ==========================*/

BEGIN_EVENT_TABLE( xaplSleepTimerCountdownUI, wxDialog )
	EVT_BUTTON( Button_TimerProgress_Abort, xaplSleepTimerCountdownUI::OnButtonAbort )
	EVT_BUTTON( wxID_CANCEL, xaplSleepTimerCountdownUI::OnButtonHide )

	EVT_RADIOBUTTON( Radio_SleepAction_CloseApp, xaplSleepTimerCountdownUI::OnSleepActionChanged )
	EVT_RADIOBUTTON( Radio_SleepAction_SleepComputer, xaplSleepTimerCountdownUI::OnSleepActionChanged )
	EVT_RADIOBUTTON( Radio_SleepAction_ShutdownComputer, xaplSleepTimerCountdownUI::OnSleepActionChanged )
END_EVENT_TABLE()

xaplSleepTimerCountdownUI::xaplSleepTimerCountdownUI( wxWindow *parent, int id ) 
	: wxDialog( parent, id, wxT("Sleep Timer Progress"), wxDefaultPosition, wxDefaultSize )
{
	Initialize();
}

xaplSleepTimerCountdownUI::~xaplSleepTimerCountdownUI( void )
{
	delete m_imgSleepTimer;
	delete m_lblTitle;
	delete m_lblInstructions;
	delete m_lblElapsedTime;
	delete m_btnAbort;
	delete m_btnHide;
}

void xaplSleepTimerCountdownUI::Initialize( void )
{
	// Instantiate the controls
	// Labels
	wxMemoryInputStream istream( xaplTimerIcon, sizeof xaplTimerIcon );
	wxImage icoImg( istream, wxBITMAP_TYPE_PNG );
	m_imgSleepTimer = new wxStaticBitmap( this, wxID_ANY, wxBitmap( icoImg ) );

	m_lblTitle = new wxStaticText( this, wxID_ANY, wxT( "Sleep Timer Progress" ), wxDefaultPosition, wxDefaultSize );
	m_lblTitle->SetFont( wxFont( 15, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD ) );
	m_lblInstructions = new wxStaticText( this, wxID_ANY, wxT( "Time left until Xapl exits:" ), wxDefaultPosition, wxDefaultSize );
	m_lblElapsedTime = new wxStaticText( this, wxID_ANY, wxT("00:00"), wxDefaultPosition, wxDefaultSize );
	m_lblElapsedTime->SetFont( wxFont( 12, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD ) );

	m_lblAction = new wxStaticText( this, wxID_ANY, wxT( "When done:" ), wxDefaultPosition, wxDefaultSize );

	// Controls
	m_rdoCloseApp = new wxRadioButton( this, Radio_SleepAction_CloseApp, wxT("Cl&ose Xapl"), wxDefaultPosition, wxDefaultSize, wxRB_GROUP );
	m_rdoSleepComputer = new wxRadioButton( this, Radio_SleepAction_SleepComputer, wxT("&Put the computer to sleep") );
	m_rdoShutdownComputer = new wxRadioButton( this, Radio_SleepAction_ShutdownComputer, wxT("S&hut down the computer") );

	m_btnAbort = new wxButton( this, Button_TimerProgress_Abort, wxT( "&Abort" ) );
	m_btnHide = new wxButton( this, wxID_CANCEL, wxT( "&Hide" ) );

	// Layout
	wxGridBagSizer *gridbag = new wxGridBagSizer;

	wxFlexGridSizer *headerSizer = new wxFlexGridSizer( 2, 1 );
	headerSizer->Add( m_imgSleepTimer, 0, wxALIGN_CENTER_VERTICAL );
	headerSizer->Add( m_lblTitle, 0, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT, 12 );

	gridbag->Add( headerSizer, wxGBPosition( 0, 0 ), wxGBSpan( 1, 2 ), wxLEFT | wxTOP, 8 );

	gridbag->Add( m_lblInstructions, wxGBPosition( 1, 0 ), wxDefaultSpan, wxLEFT | wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL, 15 );
	gridbag->Add( m_lblElapsedTime, wxGBPosition( 1, 1 ), wxDefaultSpan, wxLEFT | wxRIGHT | wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL, 5 );
	
	gridbag->Add( m_lblAction, wxGBPosition( 2, 0 ), wxDefaultSpan, wxALIGN_RIGHT | wxTOP, 5 );
	gridbag->Add( m_rdoCloseApp, wxGBPosition( 2, 1 ), wxDefaultSpan, wxLEFT | wxRIGHT | wxTOP | wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL, 5 );
	gridbag->Add( m_rdoSleepComputer, wxGBPosition( 3, 1 ), wxDefaultSpan, wxLEFT | wxRIGHT | wxTOP | wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL, 5 );
	gridbag->Add( m_rdoShutdownComputer, wxGBPosition( 4, 1 ), wxDefaultSpan, wxALL | wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL, 5 );
	

	wxFlexGridSizer *buttonSizer = new wxFlexGridSizer( 2, 1 );
	buttonSizer->Add( m_btnAbort, 0 );
	buttonSizer->Add( m_btnHide, 0, wxLEFT, 5 );

	gridbag->Add( buttonSizer, wxGBPosition( 5, 1 ), wxDefaultSpan, wxALIGN_RIGHT | wxALL, 5 );

	gridbag->AddGrowableCol( 0 );
	gridbag->SetEmptyCellSize( wxSize( 10, 20 ) );

	gridbag->Fit( this );
	gridbag->SetSizeHints( this );

	SetSizer( gridbag );
}

void xaplSleepTimerCountdownUI::InitSleepAction( int action )
{
	switch ( action )
	{
	case SA_CLOSEAPP:
		m_rdoCloseApp->SetValue( true );
		break;
	case SA_SLEEP:
		m_rdoSleepComputer->SetValue( true );
		break;
	case SA_SHUTDOWN:
		m_rdoShutdownComputer->SetValue( true );
		break;
	}
}

void xaplSleepTimerCountdownUI::SetTimer( xaplSleepTimer *timer )
{
	m_timer = timer;
	
	int totalSeconds = m_timer->GetMinutes() * 60;
	int seconds = totalSeconds % 60;
	int minutes = ( totalSeconds - seconds ) / 60;
	m_lblElapsedTime->SetLabel( wxString::Format( wxT("%d:%02d"), minutes, seconds ) );
	m_lblElapsedTime->Update();
}

void xaplSleepTimerCountdownUI::OnSleepActionChanged( wxCommandEvent &WXUNUSED(evt) )
{
	if ( m_rdoCloseApp->GetValue() )
		m_timer->SetSleepAction( SA_CLOSEAPP );
	else if ( m_rdoSleepComputer->GetValue() )
		m_timer->SetSleepAction( SA_SLEEP );
	else if ( m_rdoShutdownComputer->GetValue() )
		m_timer->SetSleepAction( SA_SHUTDOWN );
}

void xaplSleepTimerCountdownUI::OnButtonAbort( wxCommandEvent &WXUNUSED(evt) )
{
	SetReturnCode( 2 );
	m_timer->Stop();

	xaplPlaylistWindow *mainWnd = static_cast<xaplPlaylistWindow *>( this->GetParent() );
	mainWnd->RestoreTempStatusText();

	Hide();
}

void xaplSleepTimerCountdownUI::OnButtonHide( wxCommandEvent &WXUNUSED(evt) )
{
	SetReturnCode( 0 );
	Hide();
}

/* ====================================================================
 * ==== xaplSleepTimer Definition =====================================*/

xaplSleepTimer::xaplSleepTimer( xaplSleepTimerCountdownUI *ui ) 
	: wxTimer(), 
	m_ui( ui ), 
	m_minutes( 5 ), 
	m_elapsed( 0 ), 
	m_sleepAction( SA_CLOSEAPP )
{ }

xaplSleepTimer::xaplSleepTimer( xaplSleepTimerCountdownUI *ui, unsigned int minutes, int sleepAction ) 
	: wxTimer(), 
	m_ui( ui ), 
	m_minutes( minutes ), 
	m_elapsed( 0 ), 
	m_sleepAction( sleepAction )
{ }

bool xaplSleepTimer::Start( void )
{
	return wxTimer::Start( 1000 );
}

void xaplSleepTimer::SetMinutes( unsigned int minutes )
{
	m_elapsed = 0;
	m_minutes = minutes;
}

unsigned int xaplSleepTimer::GetMinutes( void )
{
	return m_minutes;
}

unsigned int xaplSleepTimer::GetElapsedSeconds( void )
{
	return m_elapsed;
}

void xaplSleepTimer::SetSleepAction( int action )
{
	m_sleepAction = action;
}

void xaplSleepTimer::Notify( void )
{
   	if ( ++m_elapsed == (m_minutes * 60) )
	{
		if ( m_ui != NULL )
			m_ui->m_lblElapsedTime->SetLabel( wxT("0:00") );

		if ( m_sleepAction != SA_CLOSEAPP )
		{
			// Stop playback
			xaplPlaylistWindow *mainWnd = static_cast<xaplPlaylistWindow *>( wxTheApp->GetTopWindow() );
			mainWnd->RestoreTempStatusText();
			mainWnd->OnButtonStop( wxCommandEvent() );

			// Stop the close the timer
			Stop();
			if ( m_ui != NULL )
				m_ui->Close();

			// Perform suspend or shutdown (platform dependent)
#ifdef __WINDOWS__
			HANDLE hToken; 
			TOKEN_PRIVILEGES tkp; 

			// Get a token for this process. 
			if ( OpenProcessToken( GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken ) ) 
			{
				// Get the LUID for the shutdown privilege. 
				LookupPrivilegeValue( NULL, SE_SHUTDOWN_NAME, &tkp.Privileges[0].Luid ); 

				tkp.PrivilegeCount = 1;  // one privilege to set    
				tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED; 

				// Get the shutdown privilege for this process. 
				AdjustTokenPrivileges( hToken, FALSE, &tkp, 0, (PTOKEN_PRIVILEGES) NULL, 0 ); 

				if ( GetLastError() == ERROR_SUCCESS ) 
				{
					switch ( m_sleepAction )
					{
					case SA_SLEEP:
						SetSuspendState( FALSE,	FALSE, FALSE );
						break;
					case SA_HIBERNATE:
						SetSuspendState( TRUE, FALSE, FALSE );
						break;
					case SA_SHUTDOWN:
						ExitWindowsEx( EWX_POWEROFF | EWX_FORCE, 
							SHTDN_REASON_MAJOR_APPLICATION | SHTDN_REASON_MINOR_MAINTENANCE | SHTDN_REASON_FLAG_PLANNED );
						break;
					}
				}
			}
#elif __LINUX__
			// TODO: Sleep and hibernate support. Also, test shutdown in Linux
			#ifdef BMAGIC_HARD
			  init_reboot( BMAGIC_HARD );
			#endif

			//Stop init; it is insensitive to the signals sent by the kernel
			kill( 1, SIGTSTP );

			// Halt or poweroff
			if ( m_sleepAction == SA_SHUTDOWN )
				init_reboot( BMAGIC_POWEROFF );
			
			// Fallthrough if failed
			init_reboot( BMAGIC_HALT );
#endif
		}
		else
		{
			// Just close Xapl
			if ( m_ui != NULL )
				m_ui->Hide();

			xaplPlaylistWindow *plWnd = static_cast<xaplPlaylistWindow *>( wxTheApp->GetTopWindow() );
			plWnd->Close();
		}

		return;
	}

	if ( m_ui != NULL )
	{
		int totalSeconds = (m_minutes * 60);
		totalSeconds -= m_elapsed;

		int seconds = totalSeconds % 60;
		int minutes = ( totalSeconds - seconds ) / 60;
		m_ui->m_lblElapsedTime->SetLabel( wxString::Format( wxT("%d:%02d"), minutes, seconds ) );
	}
}

/* ====================================================================
 * ==== xaplSleepTimerUI Definition ===================================*/

BEGIN_EVENT_TABLE( xaplSleepTimerUI, wxDialog )
	EVT_BUTTON( wxID_OK, xaplSleepTimerUI::OnButtonStart )
	EVT_BUTTON( wxID_CANCEL, xaplSleepTimerUI::OnButtonCancel )
END_EVENT_TABLE()

xaplSleepTimerUI::xaplSleepTimerUI( wxWindow *parent, int id ) 
	: wxDialog( parent, id, wxT("Sleep Timer"), wxDefaultPosition, wxDefaultSize )
{
	Initialize();
}

xaplSleepTimerUI::~xaplSleepTimerUI( void )
{
	delete m_imgSleepTimer;
	delete m_lblTitle;
	delete m_lblInstructions;
	delete m_btnStart;
	delete m_btnCancel;
	delete m_countdown;
	delete m_timer;
}

void xaplSleepTimerUI::Initialize( void )
{
	// Instantiate the controls
	// Labels
	wxMemoryInputStream istream( xaplTimerIcon, sizeof xaplTimerIcon );
	wxImage icoImg( istream, wxBITMAP_TYPE_PNG );
	m_imgSleepTimer = new wxStaticBitmap( this, wxID_ANY, wxBitmap( icoImg ) );

	m_lblTitle = new wxStaticText( this, wxID_ANY, wxT( "Sleep Timer" ), wxDefaultPosition, wxDefaultSize );
	m_lblTitle->SetFont( wxFont( 15, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD ) );
	m_lblInstructions = new wxStaticText( this, wxID_ANY, wxT( "Minutes until Xapl exits:" ), wxDefaultPosition, wxDefaultSize );
	
	m_lblAction = new wxStaticText( this, wxID_ANY, wxT( "When done:" ), wxDefaultPosition, wxDefaultSize );

	// Controls
	m_spnMinutes = new wxSpinCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS | wxTE_PROCESS_ENTER, 1, 1440, 15 );
	m_spnMinutes->Connect( wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler(xaplSleepTimerUI::OnEnter) );

	m_rdoCloseApp = new wxRadioButton( this, Radio_SleepAction_CloseApp, wxT("Cl&ose Xapl"), wxDefaultPosition, wxDefaultSize, wxRB_GROUP );
	m_rdoSleepComputer = new wxRadioButton( this, Radio_SleepAction_SleepComputer, wxT("&Put the computer to sleep") );
	m_rdoShutdownComputer = new wxRadioButton( this, Radio_SleepAction_ShutdownComputer, wxT("S&hut down the computer") );

	m_btnStart = new wxButton( this, wxID_OK, wxT( "&Start" ) );
	m_btnCancel = new wxButton( this, wxID_CANCEL, wxT( "&Cancel" ) );

	// Layout
	wxGridBagSizer *gridbag = new wxGridBagSizer;

	wxFlexGridSizer *headerSizer = new wxFlexGridSizer( 2, 1 );
	headerSizer->Add( m_imgSleepTimer, 0, wxALIGN_CENTER_VERTICAL );
	headerSizer->Add( m_lblTitle, 0, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL | wxLEFT, 12 );

	gridbag->Add( headerSizer, wxGBPosition( 0, 0 ), wxGBSpan( 1, 2 ), wxLEFT | wxTOP, 8 );

	gridbag->Add( m_lblInstructions, wxGBPosition( 1, 0 ), wxDefaultSpan, wxLEFT | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL, 15 );
	gridbag->Add( m_spnMinutes, wxGBPosition( 1, 1 ), wxDefaultSpan, wxLEFT | wxRIGHT | wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL, 5 );
	
	gridbag->Add( m_lblAction, wxGBPosition( 2, 0 ), wxDefaultSpan, wxALIGN_RIGHT | wxTOP, 5 );
	gridbag->Add( m_rdoCloseApp, wxGBPosition( 2, 1 ), wxDefaultSpan, wxLEFT | wxRIGHT | wxTOP | wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL, 5 );
	gridbag->Add( m_rdoSleepComputer, wxGBPosition( 3, 1 ), wxDefaultSpan, wxLEFT | wxRIGHT | wxTOP | wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL, 5 );
	gridbag->Add( m_rdoShutdownComputer, wxGBPosition( 4, 1 ), wxDefaultSpan, wxALL | wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL, 5 );
	
	wxFlexGridSizer *buttonSizer = new wxFlexGridSizer( 2, 1 );
	buttonSizer->Add( m_btnStart, 0 );
	buttonSizer->Add( m_btnCancel, 0, wxLEFT, 5 );

	gridbag->Add( buttonSizer, wxGBPosition( 5, 1 ), wxDefaultSpan, wxALIGN_RIGHT | wxALL, 5 );

	gridbag->AddGrowableCol( 0 );
	gridbag->SetEmptyCellSize( wxSize( 10, 20 ) );

	gridbag->Fit( this );
	gridbag->SetSizeHints( this );

	SetSizer( gridbag );

	// Init timer
	m_countdown = new xaplSleepTimerCountdownUI( this->GetParent(), wxID_ANY );
	m_timer = new xaplSleepTimer( m_countdown );
}

int xaplSleepTimerUI::Show( void )
{
	if ( GetTimerRunning() )
	{
		// Raise timer status if it is already shown
		if ( m_countdown->IsShown() ) m_countdown->Raise();

		// Display timer status
		m_countdown->Show();
	}
	else
	{
		// Display timer setup
		return wxDialog::Show();
	}

	return 0;
}

void xaplSleepTimerUI::OnButtonStart( wxCommandEvent &WXUNUSED(evt) )
{
	m_timer->SetMinutes( (unsigned int) m_spnMinutes->GetValue() );
	m_countdown->SetTimer( m_timer );
	

	// Set sleep action
	if ( m_rdoCloseApp->GetValue() )
	{
		m_timer->SetSleepAction( SA_CLOSEAPP );
		m_countdown->InitSleepAction( SA_CLOSEAPP );
	}
	else if ( m_rdoSleepComputer->GetValue() )
	{
		m_timer->SetSleepAction( SA_SLEEP );
		m_countdown->InitSleepAction( SA_SLEEP );
	}
	else if ( m_rdoShutdownComputer->GetValue() )
	{
		m_timer->SetSleepAction( SA_SHUTDOWN );
		m_countdown->InitSleepAction( SA_SHUTDOWN );
	}

	m_timer->Start();
	Hide();
	m_countdown->Show();

	xaplPlaylistWindow *mainWnd = static_cast<xaplPlaylistWindow *>( this->GetParent() );
	mainWnd->SetTempStatusText( wxT("ST") );
}

void xaplSleepTimerUI::OnButtonCancel( wxCommandEvent &WXUNUSED(evt) )
{
	SetReturnCode( 0 );
	Hide();
}

void xaplSleepTimerUI::OnEnter( wxCommandEvent &WXUNUSED(evt) )
{
	wxCommandEvent fEvent;
	fEvent.SetId( wxID_OK );
	fEvent.SetEventType( wxEVT_COMMAND_BUTTON_CLICKED );
	::wxPostEvent( this, fEvent );
}

bool xaplSleepTimerUI::GetTimerRunning( void )
{
	return m_timer->IsRunning();
}
