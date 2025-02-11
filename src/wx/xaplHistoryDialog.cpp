#include "../../lib/include/taglib/tag.h"
#include "../../lib/include/taglib/fileref.h"
#include <wx/clipbrd.h>
#include "xaplPlaylistWindow.h"
#include "xaplAudio.h"
#include "xaplHistoryDialog.h"

BEGIN_EVENT_TABLE( xaplHistoryDialog, wxFrame )
	EVT_MENU( HistContextMenuItem_Play, xaplHistoryDialog::OnContextMenuPlay )
	EVT_MENU( HistContextMenuItem_Queue, xaplHistoryDialog::OnContextMenuQueue )
	EVT_MENU( HistContextMenuItem_CopyStrings, xaplHistoryDialog::OnContextMenuCopyStrings )
	EVT_MENU( HistContextMenuItem_CopyNames, xaplHistoryDialog::OnContextMenuCopyNames )
	EVT_MENU( HistContextMenuItem_CopyPaths, xaplHistoryDialog::OnContextMenuCopyPaths )

	EVT_LISTBOX_DCLICK( ListBox_History, xaplHistoryDialog::OnItemActivated )

	EVT_CLOSE( xaplHistoryDialog::OnClose )
END_EVENT_TABLE()

xaplHistoryDialog::xaplHistoryDialog( xaplPlaylistWindow *mainWin, xaplPlaylistCtrl *playlist )
	: wxFrame( mainWin, wxID_ANY, wxT("Playback History"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE | wxFRAME_TOOL_WINDOW | wxFRAME_FLOAT_ON_PARENT ),
	m_mainWin( mainWin ),
	m_playlist( playlist )
{
	Initialize();
}

xaplHistoryDialog::~xaplHistoryDialog(void)
{
	Clear();
	delete m_histList;
	delete m_contextMenu;
}

void xaplHistoryDialog::Initialize( void )
{
	m_histList = new wxListBox( this, ListBox_History, wxDefaultPosition, wxDefaultSize, 0, NULL, wxLB_EXTENDED | wxLB_NEEDED_SB | wxBORDER_NONE );
	m_histList->Connect( ListBox_History, wxEVT_RIGHT_UP, wxMouseEventHandler(xaplHistoryDialog::OnRightClick), NULL, this );

	m_contextMenu = new wxMenu( wxEmptyString );
	m_contextMenu->Append( HistContextMenuItem_Play, wxT("Play &First Selection\tEnter") );
	m_contextMenu->Append( HistContextMenuItem_Queue, wxT("&Queue\tCtrl-Q") );
	m_contextMenu->AppendSeparator();
	m_contextMenu->Append( HistContextMenuItem_CopyStrings, wxT("&Copy \"<Artist> - <Title>\"\tCtrl-C") );
	m_contextMenu->Append( HistContextMenuItem_CopyNames, wxT("Copy File &Name(s)\tCtrl-Shift-C") );
	m_contextMenu->Append( HistContextMenuItem_CopyPaths, wxT("Copy &Path(s)\tCtrl-Alt-C") );

	wxFlexGridSizer *sizer = new wxFlexGridSizer( 1, 1, 0, 0 );
	sizer->Add( m_histList, 0, wxGROW );
	
	sizer->AddGrowableCol( 0 );
	sizer->AddGrowableRow( 0 );

	SetSizer( sizer );
}

void xaplHistoryDialog::Append( long id )
{
	long *pId = new long;
	*pId = id;

	wxString path = m_playlist->GetItemById( id )->path;
	wxString display;
	char str[2048];
#ifdef __WINDOWS__
	strcpy_s( str, 2048, (const char *)path.mb_str(wxConvUTF8) );
#else
	strcpy( str, (const char *)path.mb_str(wxConvUTF8) ); 
#endif
	TagLib::FileName fn( str );
	TagLib::FileRef file( fn );
	if ( !file.isNull() && file.tag() )
	{
		TagLib::Tag *tag = file.tag();
		wxString artist = wxString(tag->artist().toCString(true), wxConvLibc).Trim().Trim(false);
		wxString album = wxString(tag->album().toCString(true), wxConvLibc).Trim().Trim(false);
		wxString title = wxString(tag->title().toCString(true), wxConvLibc).Trim().Trim(false);
		
		if ( artist.Length() == 0 ) 
			display << wxT("Unknown Artist - ");
		else
			display << artist << wxT(" - ");

		if ( album.Length() == 0 ) 
			display << wxT("Unknown Album - ");
		else
			display << album << wxT(" - ");

		if ( title.Length() == 0 ) 
			display << wxT("Unknown Title\n");
		else
			display << title << wxT("\n");
	}

	m_histList->Append( display, pId );
	Refresh();
}

void xaplHistoryDialog::Clear( void )
{
	for ( size_t i = 0; i < m_histList->GetCount(); ++i )
		delete (long *)m_histList->GetClientData( (unsigned int) i );

	m_histList->DeselectAll();
	m_histList->Clear();
}

void xaplHistoryDialog::OnContextMenuPlay( wxCommandEvent &WXUNUSED(evt) )
{
	wxCommandEvent fEvent;
	fEvent.SetEventType( wxEVT_COMMAND_LISTBOX_SELECTED );
	fEvent.SetId( ListBox_History );
	wxPostEvent( this, fEvent );
}

void xaplHistoryDialog::OnContextMenuQueue( wxCommandEvent &WXUNUSED(evt) )
{
	wxArrayInt sels, ids;
	m_histList->GetSelections( sels );
	for ( size_t i = 0; i < sels.GetCount(); ++i )
		ids.Add( *( (long *) m_histList->GetClientData( sels[i] ) ) );

	m_playlist->QueueItems( ids );
}

void xaplHistoryDialog::OnContextMenuCopyStrings( wxCommandEvent &WXUNUSED(evt) )
{
	wxString fileList;
	wxArrayInt sels;
	m_histList->GetSelections( sels );

	for ( size_t i = 0; i < sels.GetCount(); ++i )
		fileList << m_histList->GetString( sels[i] ) << wxT("\n");

	if ( wxTheClipboard->Open() )
	{
		wxTheClipboard->SetData( new wxTextDataObject( fileList ) );
		wxTheClipboard->Close();
	}
}

void xaplHistoryDialog::OnContextMenuCopyNames( wxCommandEvent &WXUNUSED(evt) )
{
	wxString fileList;
	wxArrayInt sels;
	m_histList->GetSelections( sels );

	for ( size_t i = 0; i < sels.GetCount(); ++i )
		fileList << m_playlist->GetItemById( *( (long *)m_histList->GetClientData( sels[i] ) ) )->fileName << wxT("\n");

	if ( wxTheClipboard->Open() )
	{
		wxTheClipboard->SetData( new wxTextDataObject( fileList ) );
		wxTheClipboard->Close();
	}
}

void xaplHistoryDialog::OnContextMenuCopyPaths( wxCommandEvent &WXUNUSED(evt) )
{
	wxString pathList;
	wxArrayInt sels;
	m_histList->GetSelections( sels );

	for ( size_t i = 0; i < sels.GetCount(); ++i )
		pathList << m_playlist->GetItemById( *( (long *)m_histList->GetClientData( sels[i] ) ) )->path << wxT("\n");

	if ( wxTheClipboard->Open() )
	{
		wxTheClipboard->SetData( new wxTextDataObject( pathList ) );
		wxTheClipboard->Close();
	}
}

void xaplHistoryDialog::OnItemActivated( wxCommandEvent &WXUNUSED(evt) )
{
	wxArrayInt sels;
	m_histList->GetSelections( sels );

	if ( sels.GetCount() == 0 ) return;

	m_playlist->SetPlayingItem( *( (long *)m_histList->GetClientData( sels[0] ) ) );
	xaplAudio::GetInstance()->Play();
}

void xaplHistoryDialog::OnRightClick( wxMouseEvent &evt )
{
	if ( m_histList->GetCount() == 0 ) return;

	int idx = m_histList->HitTest( evt.GetPosition() );
	if ( idx >= 0 && idx < (int) m_histList->GetCount() )
	{
		wxArrayInt sels;
		m_histList->GetSelections( sels );

		if ( sels.GetCount() <= 1 )
		{
			m_histList->DeselectAll();
			m_histList->Select( idx );
			m_contextMenu->FindItemByPosition( 0 )->SetText( wxT("P&lay\tEnter") );
		}
		else
		{
			m_contextMenu->FindItemByPosition( 0 )->SetText( wxT("Play &First Selection\tEnter") );
		}

		m_histList->PopupMenu( m_contextMenu );
	}
}

void xaplHistoryDialog::OnClose( wxCloseEvent &evt )
{
	Hide();
}
