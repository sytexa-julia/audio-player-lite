#include "xaplControlUI.h"
#include "xaplStaticText.h"

#ifdef _DEBUG
#include <crtdbg.h>
#define DEBUG_NEW new(_NORMAL_BLOCK ,__FILE__, __LINE__)
#else
#define DEBUG_NEW new
#endif
#ifdef _DEBUG
#define new DEBUG_NEW
#endif 

BEGIN_EVENT_TABLE( xaplControlUI, wxPanel )
	EVT_COMMAND_SCROLL_THUMBTRACK( ID_SongPosition, xaplControlUI::OnPositionChanging )
	EVT_COMMAND_SCROLL( ID_SongPosition, xaplControlUI::OnPositionChange ) 
	EVT_COMMAND_SCROLL( ID_Volume, xaplControlUI::OnVolumeChange )
END_EVENT_TABLE()

xaplControlUI::xaplControlUI( wxWindow *parent ) 
	: wxPanel( parent, wxID_ANY, wxDefaultPosition, wxDefaultSize ), 
	  m_songLength( 0 ), 
	  m_currentPlayPauseIcon( ICON_PLAY ), 
	  m_thumbActive( false )
{
	Initialize();
}

xaplControlUI::~xaplControlUI( void )
{
	delete slSongPosition;
	delete slVolume;
	delete lblVolPercent;

	delete btnPrevSong;
	delete btnNextSong;
	delete btnPlayPause;
	delete btnStop;

	delete playIcon;
	delete pauseIcon;

	delete lblSongCurrTime;
	delete lblSongTotalTime;
	delete lblSongCountdown;
	delete slash1;
	delete slash2;
}

void xaplControlUI::Initialize( void )
{
	// Instantiate the controls
	slSongPosition = new wxSlider( this, ID_SongPosition, 0, 0, 500, wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL );
	
	lblSongCurrTime = new xaplStaticText( this, ID_SongCurrTime, wxT("0:00") );
	lblSongTotalTime = new xaplStaticText( this, ID_SongTotalTime, wxT("0:00") );
	lblSongCountdown = new xaplStaticText( this, ID_SongCountdown, wxT("-0:00") );
	slash1 = new wxStaticText( this, wxID_ANY, wxT(" / ") );
	slash2 = new wxStaticText( this, wxID_ANY, wxT(" / ") );

	// Next
	wxMemoryInputStream istream( xaplNextIcon, sizeof xaplNextIcon );
	wxImage icoImg( istream, wxBITMAP_TYPE_PNG );
	btnNextSong = new wxBitmapButton( this, ID_NextSongButton, wxBitmap( icoImg ) );

	// Prev
	wxMemoryInputStream istream2( xaplPrevIcon, sizeof xaplPrevIcon );
	wxImage icoImg2( istream2, wxBITMAP_TYPE_PNG );
	btnPrevSong = new wxBitmapButton( this, ID_PrevSongButton, wxBitmap( icoImg2 ) );

	// Play icon
	wxMemoryInputStream istream3( xaplPlayIcon, sizeof xaplPlayIcon );
	wxImage icoImg3( istream3, wxBITMAP_TYPE_PNG );
	playIcon = new wxBitmap( icoImg3 );

	// Pause icon
	wxMemoryInputStream istream4( xaplPauseIcon, sizeof xaplPauseIcon );
	wxImage icoImg4( istream4, wxBITMAP_TYPE_PNG );
	pauseIcon = new wxBitmap( icoImg4 );

	// Play
	btnPlayPause = new wxBitmapButton( this, ID_PlayPauseButton, *playIcon );

	// Stop
	wxMemoryInputStream istream5( xaplStopIcon, sizeof xaplStopIcon );
	wxImage icoImg5( istream5, wxBITMAP_TYPE_PNG );
	btnStop = new wxBitmapButton( this, ID_StopButton, wxBitmap( icoImg5 ) );

	slVolume = new wxSlider( this, ID_Volume, 100, 0, 100, wxDefaultPosition, wxSize( 20, 50 ), wxSL_VERTICAL | wxSL_INVERSE );
	lblVolPercent = new wxStaticText( this, ID_VolPercent, wxT("100 %") );

	// Create the layout sizer and add the controls
	wxGridBagSizer *gridbag = new wxGridBagSizer;

	gridbag->Add( slSongPosition, wxGBPosition( 0, 0 ), wxGBSpan( 1, 3 ), wxALIGN_CENTER | wxALIGN_BOTTOM | wxGROW );
	gridbag->Add( slVolume, wxGBPosition( 0, 3 ), wxGBSpan( 2, 1 ), wxALIGN_LEFT | wxALIGN_TOP | wxRIGHT, 8 );

	wxFlexGridSizer *flexGrid = new wxFlexGridSizer( 9, 1 );
	flexGrid->Add( btnPrevSong, 0, wxALIGN_TOP );
	flexGrid->Add( btnPlayPause, 0, wxALIGN_TOP );
	flexGrid->Add( btnStop, 0, wxALIGN_TOP );
	flexGrid->Add( btnNextSong, 0, wxALIGN_TOP );
	flexGrid->Add( lblSongCurrTime, 0, wxALIGN_CENTER_VERTICAL | wxLEFT, 15 );
	flexGrid->Add( slash1, 0, wxALIGN_CENTER_VERTICAL );
	flexGrid->Add( lblSongTotalTime, 0, wxALIGN_CENTER_VERTICAL );
	flexGrid->Add( slash2, 0, wxALIGN_CENTER_VERTICAL );
	flexGrid->Add( lblSongCountdown, 0, wxALIGN_CENTER_VERTICAL );
	
	gridbag->Add( flexGrid, wxGBPosition( 1, 0 ), wxGBSpan( 1, 2 ), wxALIGN_LEFT | wxLEFT, 6 );
	gridbag->Add( lblVolPercent, wxGBPosition( 1, 2 ), wxDefaultSpan, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL | wxBOTTOM, 3 );
	gridbag->Add( new wxStaticLine( this ), wxGBPosition(2, 0), wxGBSpan(1, 4), wxGROW );

	gridbag->AddGrowableCol( 1 );

	gridbag->Fit( this );
	gridbag->SetSizeHints( this );

	SetSizer( gridbag );
}

void xaplControlUI::OnVolumeChange( wxScrollEvent &evt )
{
	int pos = evt.GetPosition();
	wxString strPos;
	strPos << pos << wxT( " %" );
	lblVolPercent->SetLabel( strPos );

	xaplAudio::GetInstance()->SetVolume( pos );
}

// This just updates the position label with the position the song
// would be in if the user released the thumb at this point
void xaplControlUI::OnPositionChanging( wxScrollEvent &evt )
{
	m_thumbActive = true;

	// Calculate new position in seconds
	double perc = (double) evt.GetPosition() / 500.0;
	float currentSeconds = perc * this->m_songLength;
	
	// Update counter labels
	int pos = (int) (((double) currentSeconds / this->m_songLength) * 500 );
	lblSongCurrTime->SetLabel( xaplUtilities::GetFormattedTime((int) currentSeconds) );
	
	wxString countdownLabel;
	countdownLabel << wxT("-") << xaplUtilities::GetFormattedTime( (int) abs(currentSeconds - this->m_songLength) );
	lblSongCountdown->SetLabel( countdownLabel );
}

// This actually performs a position change, for when the thumb is
// released
void xaplControlUI::OnPositionChange( wxScrollEvent &evt )
{	
	m_thumbActive = false;

	// Calculate new position in seconds
	double perc = (double) evt.GetPosition() / 500.0;
	float currentSeconds = perc * this->m_songLength;
	
	// Update counter labels
	int pos = (int) (((double) currentSeconds / this->m_songLength) * 500 );
	lblSongCurrTime->SetLabel( xaplUtilities::GetFormattedTime((int) currentSeconds) );
	
	wxString countdownLabel;
	countdownLabel << wxT("-") << xaplUtilities::GetFormattedTime( (int) abs(currentSeconds - this->m_songLength) );
	lblSongCountdown->SetLabel( countdownLabel );
	
	// Notify all position listeners
	xaplAudio::GetInstance()->Seek( currentSeconds );
}

void xaplControlUI::SetSongLength( int seconds )
{
	this->m_songLength = seconds;

	slSongPosition->SetValue( 0 );
	lblSongCurrTime->SetLabel( wxT("0:00") );
	lblSongTotalTime->SetLabel( xaplUtilities::GetFormattedTime(seconds) );
	
	wxString countdownLabel;
	countdownLabel << wxT("-") << xaplUtilities::GetFormattedTime(seconds);
	lblSongCountdown->SetLabel( countdownLabel );

	Layout();
}

void xaplControlUI::SetSongPosition( int currentSeconds, int totalSeconds )
{
	// If the thumb is being dragged, don't update the position labels
	// (as dragging the thumb will show the user where they are dragging to)
	if ( m_thumbActive )
		return;
	
	if ( this->m_songLength != totalSeconds )
		this->m_songLength = totalSeconds;
	
	int pos = (int) (((double) currentSeconds / (double) totalSeconds) * 500 );
	if ( !m_thumbActive )
		slSongPosition->SetValue( pos );
	lblSongCurrTime->SetLabel( xaplUtilities::GetFormattedTime(currentSeconds) );

	wxString countdownLabel;
	countdownLabel << wxT("-") << xaplUtilities::GetFormattedTime( abs(currentSeconds - totalSeconds) );
	lblSongCountdown->SetLabel( countdownLabel );

	Layout();
}

int xaplControlUI::GetSongPosition( void ) const
{
	return slSongPosition->GetValue();
}

void xaplControlUI::SetVolume( int vol )
{
	if ( vol < 0 || vol > 100 )
		return;

	int iVol = (int) ( vol * 100.0f );
	slVolume->SetValue( vol );

	lblVolPercent->SetLabel( wxString::Format( wxT("%2d %%"), vol ) );
}

int xaplControlUI::GetVolume( void ) const
{
	return slVolume->GetValue();
}

void xaplControlUI::SetPlaying( void )
{
	btnPlayPause->SetBitmapLabel( *pauseIcon );
	m_currentPlayPauseIcon = ICON_PAUSE;
}

void xaplControlUI::SetPaused( void )
{
	btnPlayPause->SetBitmapLabel( *playIcon );
	m_currentPlayPauseIcon = ICON_PLAY;
}

void xaplControlUI::TogglePlayPauseIcon( void )
{
	switch (m_currentPlayPauseIcon)
	{
		case ICON_PAUSE:
			btnPlayPause->SetBitmapLabel( *playIcon );
			m_currentPlayPauseIcon = ICON_PLAY;
			break;
		
		case ICON_PLAY:
		default:
			btnPlayPause->SetBitmapLabel( *pauseIcon );
			m_currentPlayPauseIcon = ICON_PAUSE;
			break;
	}
}
