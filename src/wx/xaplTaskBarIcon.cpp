#include "xaplAudio.h"
#include "xaplTaskBarIcon.h"

#ifdef _DEBUG
#include <crtdbg.h>
#define DEBUG_NEW new(_NORMAL_BLOCK ,__FILE__, __LINE__)
#else
#define DEBUG_NEW new
#endif
#ifdef _DEBUG
#define new DEBUG_NEW
#endif 

BEGIN_EVENT_TABLE( xaplTaskBarIcon, wxTaskBarIcon )
	EVT_MENU( MenuItem_VolumeFull, xaplTaskBarIcon::OnMenuVolumeFull )
	EVT_MENU( MenuItem_Volume60, xaplTaskBarIcon::OnMenuVolume60 )
	EVT_MENU( MenuItem_Volume30, xaplTaskBarIcon::OnMenuVolume30 )
	EVT_MENU( MenuItem_Mute, xaplTaskBarIcon::OnMenuMute )

	EVT_MENU( MenuItem_Play, xaplTaskBarIcon::OnMenuPlay )
	EVT_MENU( MenuItem_Pause, xaplTaskBarIcon::OnMenuPause )
	EVT_MENU( MenuItem_Stop, xaplTaskBarIcon::OnMenuStop )
	EVT_MENU( MenuItem_Next, xaplTaskBarIcon::OnMenuNext )
	EVT_MENU( MenuItem_Previous, xaplTaskBarIcon::OnMenuPrevious )

	EVT_MENU( MenuItem_Open, xaplTaskBarIcon::OnMenuOpen )
	EVT_MENU( MenuItem_Exit, xaplTaskBarIcon::OnMenuExit )

	EVT_TASKBAR_LEFT_DCLICK( xaplTaskBarIcon::OnDoubleClick )
END_EVENT_TABLE()

xaplTaskBarIcon::xaplTaskBarIcon( wxTopLevelWindow *mainWin ) 
	: wxTaskBarIcon(), 
	  m_mainWindow( mainWin ), 
	  m_toolTip( wxEmptyString )
{ 
	#ifdef __WINDOWS__
	  m_icon = wxIcon( wxICON( APPICON ) );
	#else
	  m_icon = wxIcon( app_xpm );
	#endif
}

wxMenu *xaplTaskBarIcon::CreatePopupMenu( void )
{
	wxMenu *menu = new wxMenu;

	menu->Append( MenuItem_VolumeFull, wxT("100% Volume") );
	menu->Append( MenuItem_Volume60, wxT("60% Volume") );
	menu->Append( MenuItem_Volume30, wxT("30% Volume") );
	menu->Append( MenuItem_Mute, wxT("Muting") );
	menu->AppendSeparator();

	menu->Append( MenuItem_Play, wxT("Play") );
	menu->Append( MenuItem_Pause, wxT("Pause") );
	menu->Append( MenuItem_Stop, wxT("Stop") );
	menu->Append( MenuItem_Next, wxT("Next Track") );
	menu->Append( MenuItem_Previous, wxT("Previous Track") );
	menu->AppendSeparator();

	menu->Append( MenuItem_Open, wxT("Open") );
	menu->Append( MenuItem_Exit, wxT("Exit") );

#ifdef __WINDOWS__
	// Make the 'Open' menu item bolded
	SetMenuDefaultItem( (HMENU) menu->GetHMenu(), MenuItem_Open, FALSE );
#endif

	return menu;
}

void xaplTaskBarIcon::SetTooltip( const wxString &tt )
{
	m_toolTip = tt;

	if ( IsIconInstalled() )
		SetIcon( m_icon, tt );
}

void xaplTaskBarIcon::SetIconVisible( bool v )
{
	if ( v )
		SetIcon( m_icon, m_toolTip );
	else
		RemoveIcon();
}

// Directly change the volume with the audio interface
void xaplTaskBarIcon::OnMenuVolumeFull( wxCommandEvent &WXUNUSED(evt) )
{
	xaplAudio::GetInstance()->SetVolume( 100 );
}

void xaplTaskBarIcon::OnMenuVolume60( wxCommandEvent &WXUNUSED(evt) )
{
	xaplAudio::GetInstance()->SetVolume( 60 );
}

void xaplTaskBarIcon::OnMenuVolume30( wxCommandEvent &WXUNUSED(evt) )
{
	xaplAudio::GetInstance()->SetVolume( 30 );
}

void xaplTaskBarIcon::OnMenuMute( wxCommandEvent &WXUNUSED(evt) )
{
	xaplAudio::GetInstance()->SetVolume( 0 );
}

// For these next 5, we just repost events to the main window's menu for simplicity
void xaplTaskBarIcon::OnMenuPlay( wxCommandEvent &WXUNUSED(evt) )
{
	wxCommandEvent fEvent;
	fEvent.SetEventType( wxEVT_COMMAND_MENU_SELECTED );
	fEvent.SetId( MenuItem_Play_Play );
	::wxPostEvent( m_mainWindow, fEvent );
}

void xaplTaskBarIcon::OnMenuPause( wxCommandEvent &WXUNUSED(evt) )
{
	wxCommandEvent fEvent;
	fEvent.SetEventType( wxEVT_COMMAND_MENU_SELECTED );
	fEvent.SetId( MenuItem_Play_Pause );
	::wxPostEvent( m_mainWindow, fEvent );
}

void xaplTaskBarIcon::OnMenuStop( wxCommandEvent &WXUNUSED(evt) )
{
	wxCommandEvent fEvent;
	fEvent.SetEventType( wxEVT_COMMAND_MENU_SELECTED );
	fEvent.SetId( MenuItem_Play_Stop );
	::wxPostEvent( m_mainWindow, fEvent );
}

void xaplTaskBarIcon::OnMenuNext( wxCommandEvent &WXUNUSED(evt) )
{
	wxCommandEvent fEvent;
	fEvent.SetEventType( wxEVT_COMMAND_MENU_SELECTED );
	fEvent.SetId( MenuItem_Play_Next );
	::wxPostEvent( m_mainWindow, fEvent );
}

void xaplTaskBarIcon::OnMenuPrevious( wxCommandEvent &WXUNUSED(evt) )
{
	wxCommandEvent fEvent;
	fEvent.SetEventType( wxEVT_COMMAND_MENU_SELECTED );
	fEvent.SetId( MenuItem_Play_Prev );
	::wxPostEvent( m_mainWindow, fEvent );
}

void xaplTaskBarIcon::OnMenuOpen( wxCommandEvent &WXUNUSED(evt) )
{
	if ( m_mainWindow != NULL )
	{
		SetIconVisible( false );

		// A whole lot of crap just to make sure the window shows
		// up where it's supposed to...
		m_mainWindow->SetPosition( wxGetApp().GetConfig()->windowPosition );
		m_mainWindow->Iconize( false );
		m_mainWindow->SetFocus();
		m_mainWindow->Raise();
		m_mainWindow->Show( true );
		wxGetApp().GetConfig()->windowMinimized = false;
	}
}

void xaplTaskBarIcon::OnMenuExit( wxCommandEvent &WXUNUSED(evt) )
{
	if ( m_mainWindow != NULL )
		m_mainWindow->Close( true );
	else
		wxGetApp().Exit();
}

void xaplTaskBarIcon::OnDoubleClick( wxTaskBarIconEvent &evt )
{
	wxCommandEvent dummyCommand;
	OnMenuOpen( dummyCommand );
}
