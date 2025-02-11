#include "xaplFavoritesManager.h"
#include "xaplBitmaps.h"
#include "xaplApplication.h"
#include "wx/image.h"
#include "wx/mstream.h"
#include <wx/dir.h>
#include <wx/arrstr.h>
#include <wx/filename.h>

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
 * ==== xaplFavoritesManager Definition ===============================*/

xaplFavoritesManager::xaplFavoritesManager( xaplPlaylistCtrl *pl, 
										    xaplMenuBar *menu, 
											wxMenu *addToMenu ) 
	: m_playlist( pl ), 
	  m_menu( menu ), 
	  m_addToMenu( addToMenu ), 
	  m_currId( MenuItem_Favorites_ListStart ), 
	  m_accelId( 1 )
{
	m_dataPath = FAVORITES_DIR;

	// Path doesn't exist? Try to mkdir.
	if ( !wxDirExists( m_dataPath ) )
	{
		wxMkdir( m_dataPath );
		return;
	}
}

xaplFavoritesManager::~xaplFavoritesManager( void )
{}

void xaplFavoritesManager::AddFavorite( const wxString &path )
{
	xaplAddFavoriteUI *add = new xaplAddFavoriteUI( m_menu->GetFrame() );
	int result = add->ShowModal();
	if ( result == wxID_OK )
	{
		xaplFavorite fav;
		fav.id = m_currId++;
		fav.name = add->GetPlaylistName();
		fav.path = add->GetPlaylistPath();

		m_favorites.push_back(fav);
		
		// Only show the predefined # of items in the menu (don't want 100 menu items...)
		if ( m_currId <= MenuItem_Favorites_ListEnd )
		{
			// First 10 favorites get an accelerator key
			if ( m_accelId == 0 ) m_accelId = -1;
			if ( m_accelId == 10 ) m_accelId = 0;
			if ( m_accelId >= 0 )
				m_menu->menuFavorites->Append( fav.id, wxString::Format( wxT("%s\tCtrl-%d"), fav.name.c_str(), m_accelId++ ) );
			else
				m_menu->menuFavorites->Append( fav.id, fav.name );
		}

		if ( m_playlist->GetPlaylistFilePath().Lower() != fav.path.Lower() )
		{
			size_t pos = m_addToMenu->FindItemByPosition( m_addToMenu->GetMenuItemCount() - 2 )->IsSeparator() ? m_addToMenu->GetMenuItemCount() - 2 : m_addToMenu->GetMenuItemCount() - 1;
			m_addToMenu->Insert( pos, fav.id + 100, fav.name );
		}

		m_playlist->SavePlaylistAs( fav.path );
	}

	if ( m_addToMenu->GetMenuItemCount() > 1 && !m_addToMenu->FindItemByPosition( m_addToMenu->GetMenuItemCount() - 2 )->IsSeparator() )
		m_addToMenu->InsertSeparator( m_addToMenu->GetMenuItemCount() - 1 );
}

void xaplFavoritesManager::RemoveFavorite( int id )
{
	for ( size_t i = 0; i < m_favorites.size(); ++i )
	{
		if ( m_favorites.at( i ).id == id )
		{
			wxRemoveFile( m_favorites.at( i ).path );
			m_favorites.erase( m_favorites.begin() + i );
			m_menu->Remove( id );
			m_addToMenu->Remove( id + 100 );
			return;
		}
	}
}
	
void xaplFavoritesManager::RemoveFavorite( const wxString &path )
{
	wxRemoveFile( path );

	for ( size_t i = 0; i < m_favorites.size(); ++i )
	{
		if ( m_favorites.at( i ).path.Lower() == path.Lower() )
		{
			size_t id = m_favorites.at( i ).id;
			m_favorites.erase( m_favorites.begin() + i );
			m_menu->Remove( id );
			m_addToMenu->Remove( i + 100 );
			return;
		}
	}
}

void xaplFavoritesManager::RefreshFavorites( void )
{
	// Reset state and clear menu
	m_favorites.clear();
	m_currId = MenuItem_Favorites_ListStart;
	m_accelId = 1;

	// Clear Favorites menu
	for ( size_t i = MenuItem_Favorites_ListStart; i <= MenuItem_Favorites_ListEnd; ++i )
	{
		if ( m_menu->menuFavorites->FindItem( i, NULL ) != NULL )
			m_menu->menuFavorites->Delete( i );
	}

	// Clear add-to menu
	if ( m_addToMenu->GetMenuItemCount() > 1 )
	{
		for ( int i = (int)m_addToMenu->GetMenuItemCount() - 1; i >= 0 ; --i )
		{
			wxMenuItem *menuItem = m_addToMenu->FindItemByPosition( i );
			if ( menuItem != NULL && menuItem->GetId() != ContextMenuItem_NewFavorite )
				m_addToMenu->Delete( m_addToMenu->FindItemByPosition( i ) );
		}
	}
	
	// Load existing favorites
	wxArrayString files;
	wxDir::GetAllFiles( m_dataPath, &files, wxT("*.fav.m3u"), wxDIR_FILES | wxDIR_DIRS );
	for ( size_t i = 0; i < files.Count(); ++i )
	{
		wxFileName fn( files[i] );
		xaplFavorite fav;
		fav.id = m_currId++;
		fav.path = files[i];
		fav.name = fn.GetName().Left( fn.GetName().size() - 4 );

		m_favorites.push_back(fav);

		// Only show the predefined # of items in the menu (don't want 100 menu items...)
		if ( m_currId <= MenuItem_Favorites_ListEnd )
		{
			// First 10 favorites get an accelerator key
			if ( m_accelId == 0 ) m_accelId = -1;
			if ( m_accelId == 10 ) m_accelId = 0;
			if ( m_accelId >= 0 )
				m_menu->menuFavorites->Append( fav.id, wxString::Format( wxT("%s\tCtrl-%d"), fav.name.c_str(), m_accelId++ ) );
			else
				m_menu->menuFavorites->Append( fav.id, fav.name);
		}

		if ( m_playlist->GetPlaylistFilePath().Lower() != fav.path.Lower() )			
			m_addToMenu->Insert( m_addToMenu->GetMenuItemCount() - 1, fav.id + 100, fav.name );
	}

	if ( m_addToMenu->GetMenuItemCount() > 1 )
		m_addToMenu->InsertSeparator( m_addToMenu->GetMenuItemCount() - 1 );
}

void xaplFavoritesManager::ManageFavorites( void )
{
	xaplManageFavoritesUI *mgr = new xaplManageFavoritesUI( m_menu->GetFrame(), &m_favorites );
	mgr->ShowModal();
	RefreshFavorites();
	delete mgr;
}

wxString xaplFavoritesManager::GetPath( int id )
{
	for ( size_t i = 0; i < m_favorites.size(); ++i )
	{
		if ( id == m_favorites.at(i).id || (id - 100) == m_favorites.at(i).id )
		{
			return m_favorites.at(i).path;
		}
	}

	return wxEmptyString;
}

/* ====================================================================
 * ==== xaplAddFavoriteUI Definition ==================================*/

BEGIN_EVENT_TABLE( xaplAddFavoriteUI, wxDialog )
	EVT_TEXT_ENTER( TextCtrl_FavAdd_Name, xaplAddFavoriteUI::OnEnter )
END_EVENT_TABLE()

xaplAddFavoriteUI::xaplAddFavoriteUI( wxWindow *parent ) 
	: wxDialog(  parent, wxID_ANY, wxT("Add Playlist to Favorites"), wxDefaultPosition, wxDefaultSize )
{
	Initialize();
}

xaplAddFavoriteUI::~xaplAddFavoriteUI( void )
{
	delete imgAddFav;
	delete lblTitle;
	delete lblInstructions;
	delete txtFavName;
	delete btnOK;
	delete btnCancel;
}

void xaplAddFavoriteUI::Initialize( void )
{
	// Instantiate the controls
	// Labels
	wxMemoryInputStream istream( xaplAddFavoriteIcon, sizeof xaplAddFavoriteIcon );
	wxImage icoImg( istream, wxBITMAP_TYPE_PNG );
	imgAddFav = new wxStaticBitmap( this, wxID_ANY, wxBitmap( icoImg ) );

	lblTitle = new wxStaticText( this, wxID_ANY, wxT( "Add Playlist to Favorites" ), wxDefaultPosition, wxDefaultSize );
	lblTitle->SetFont( wxFont( 15, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD ) );
	lblInstructions = new wxStaticText( this, wxID_ANY, wxT( "Playlist Name:" ), wxDefaultPosition, wxDefaultSize );
	
	// Controls
	txtFavName = new wxTextCtrl( this, TextCtrl_FavAdd_Name, wxT("Playlist 1"), wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER );
	btnOK = new wxButton( this, wxID_OK, wxEmptyString );
	btnCancel = new wxButton( this, wxID_CANCEL, wxEmptyString );

	// Layout
	gridbag = new wxGridBagSizer();

	wxFlexGridSizer *headerSizer = new wxFlexGridSizer( 2, 1 );
	headerSizer->Add( imgAddFav, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 4 );
	headerSizer->Add( lblTitle, 0, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL | wxRIGHT, 12 );

	gridbag->Add( headerSizer, wxGBPosition( 0, 0 ), wxGBSpan( 1, 2 ), wxLEFT | wxTOP, 8 );

	gridbag->Add( lblInstructions, wxGBPosition( 1, 0 ), wxDefaultSpan, wxLEFT | wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL, 15 );
	gridbag->Add( txtFavName, wxGBPosition( 1, 1 ), wxDefaultSpan, wxLEFT | wxRIGHT | wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL | wxGROW, 5 );
	
	wxFlexGridSizer *buttonSizer = new wxFlexGridSizer( 2, 1 );
	buttonSizer->Add( btnOK, 0 );
	buttonSizer->Add( btnCancel, 0, wxLEFT, 5 );

	gridbag->Add( buttonSizer, wxGBPosition( 4, 0 ), wxGBSpan( 1, 2 ), wxALIGN_RIGHT | wxALL, 5 );

	gridbag->AddGrowableCol( 1 );
	gridbag->SetEmptyCellSize( wxSize( 10, 10 ) );

	gridbag->Fit( this );
	gridbag->SetSizeHints( this );

	SetSizer( gridbag );

	// Default playlist name
	wxArrayString files;
	wxDir::GetAllFiles( FAVORITES_DIR, &files, wxT("*.fav.m3u"), wxDIR_FILES | wxDIR_DIRS );
	size_t number = 1;
	bool done = false;
	do
	{
		done = true;
		for ( size_t i = 0; i < files.Count(); ++i )
		{
			wxFileName fn( files[i] );
			wxString name = fn.GetName().Lower().Trim().Trim(false);
			wxString comp = wxString::Format( wxT("playlist %u.fav"), number );
			if ( name.Cmp( comp ) == 0 )
			{
				done = false;
				++number;
				break;
			}
		}
	} while ( !done );

	txtFavName->SetValue( wxString::Format( wxT("Playlist %u"), number ) );
	txtFavName->SetFocus();
	txtFavName->SelectAll();
}

wxString xaplAddFavoriteUI::GetPlaylistName( void ) const
{
	return txtFavName->GetValue();
}

wxString xaplAddFavoriteUI::GetPlaylistPath( void ) const
{
	if ( GetPlaylistName() == wxEmptyString )
		return wxEmptyString;
	else
		return wxString::Format( wxT("%s%s%s.fav.m3u"), FAVORITES_DIR, DIR_SEP_STR, GetPlaylistName() );
}

void xaplAddFavoriteUI::OnEnter( wxCommandEvent &WXUNUSED(evt) )
{
	EndModal( wxID_OK );
}

/* ====================================================================
 * ==== xaplManageFavoritesUI Definition ==============================*/

BEGIN_EVENT_TABLE( xaplManageFavoritesUI, wxDialog )
	EVT_BUTTON( Button_FavManage_Rename, xaplManageFavoritesUI::OnRename )
	EVT_BUTTON( Button_FavManage_Delete, xaplManageFavoritesUI::OnDelete )
	EVT_BUTTON( Button_FavManage_Explore, xaplManageFavoritesUI::OnExplore )
	EVT_BUTTON( wxID_CLOSE, xaplManageFavoritesUI::OnClose )
END_EVENT_TABLE()

xaplManageFavoritesUI::xaplManageFavoritesUI( wxWindow *parent, vector<xaplFavorite> *favs ) 
	: wxDialog(  parent, wxID_ANY, wxT("Manage Favorites"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER ),
	  m_favorites( favs )
{
	Initialize();
}

xaplManageFavoritesUI::~xaplManageFavoritesUI( void )
{
	for ( size_t i = 0; i < lstFavsList->GetCount(); ++i )
		delete (wxString *) lstFavsList->GetClientData( i );

	delete lstFavsList;
	delete btnRename;
	delete btnDelete;
	delete btnExplore;
	delete btnClose;
}

void xaplManageFavoritesUI::Initialize( void )
{
	// Instantiate the controls
	// Controls
	lstFavsList = new wxListBox( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, NULL, wxLB_SINGLE );

	btnRename = new wxButton( this, Button_FavManage_Rename, wxT( "&Rename..." ) );
	btnDelete = new wxButton( this, Button_FavManage_Delete, wxT( "&Delete" ) );
	btnExplore = new wxButton( this, Button_FavManage_Explore, wxT( "&Explore..." ) );
	btnClose = new wxButton( this, wxID_CLOSE, wxEmptyString );

	// Layout
	gridbag = new wxGridBagSizer();
	
	wxFlexGridSizer *buttonSizer = new wxFlexGridSizer( 1, 3 );
	buttonSizer->Add( btnRename, 0 );
	buttonSizer->Add( btnDelete, 0 );
	buttonSizer->Add( btnExplore, 0 );

	gridbag->Add( lstFavsList, wxGBPosition( 0, 0 ), wxGBSpan( 3, 1 ), wxALL | wxGROW, 5 );
	gridbag->Add( buttonSizer, wxGBPosition( 0, 1 ), wxDefaultSpan, wxALIGN_RIGHT | wxTOP | wxRIGHT | wxBOTTOM, 5 );
	gridbag->Add( btnClose, wxGBPosition( 2, 1 ), wxDefaultSpan, wxALIGN_RIGHT | wxALIGN_BOTTOM | wxRIGHT | wxBOTTOM, 5 );

	gridbag->AddGrowableCol( 0 );
	gridbag->AddGrowableRow( 0 );
	gridbag->SetEmptyCellSize( wxSize( 10, 20 ) );

	gridbag->Fit( this );
	gridbag->SetSizeHints( this );

	SetSizer( gridbag );

	SetMinSize( wxSize(300, 300) );
	SetSize( 300, 300 );

	// Set some keyboard shortcuts
	wxAcceleratorEntry entries[5];
	
	entries[0].Set( wxACCEL_CTRL, (int) 'R', Button_FavManage_Rename );
	entries[1].Set( wxACCEL_NORMAL, WXK_DELETE, Button_FavManage_Delete );
	entries[2].Set( wxACCEL_CTRL, (int) 'E', Button_FavManage_Explore );
	entries[3].Set( wxACCEL_NORMAL, WXK_RETURN, Button_FavManage_Close );
	entries[4].Set( wxACCEL_NORMAL, WXK_ESCAPE, Button_FavManage_Close );

	wxAcceleratorTable accel(5, entries);
	SetAcceleratorTable( accel );

	// Populate favorites list
	for ( size_t i = 0; i < m_favorites->size(); ++i )
	{
		xaplFavorite fav = m_favorites->at( i );
		wxString *path = new wxString( fav.path );
		lstFavsList->Append( fav.name, path );
	}

	if ( lstFavsList->GetCount() > 0 )
		lstFavsList->SetSelection( 0 );
}

void xaplManageFavoritesUI::OnRename( wxCommandEvent &WXUNUSED(evt) )
{
	if ( lstFavsList->GetSelection() == wxNOT_FOUND ) return;

	// Gather info
	size_t sel = lstFavsList->GetSelection();
	wxString *path = (wxString *)lstFavsList->GetClientData( sel );
	
	wxFileName fn( *path );
	wxString newName = wxGetTextFromUser( wxT("Enter a new name for the playlist:"), wxT("Rename Playlist"), wxT("New Playlist"), this );
	if ( newName.IsEmpty() ) return;
	wxString *newPath = new wxString( wxString::Format( wxT("%s%s.fav.m3u"), fn.GetPath( wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR ).c_str(), newName.c_str() ) );
	
	// Perform rename
	wxRenameFile( *path, *newPath );
	
	// Modify list
	lstFavsList->SetString( sel, newName );
	delete (wxString *) lstFavsList->GetClientData( sel );
	lstFavsList->SetClientData( sel, newPath );
}

void xaplManageFavoritesUI::OnDelete( wxCommandEvent &WXUNUSED(evt) )
{
	if ( lstFavsList->GetSelection() == wxNOT_FOUND ) return;

	wxString *path = (wxString *) lstFavsList->GetClientData( lstFavsList->GetSelection() );
	wxRemoveFile( *path );
	lstFavsList->Delete( lstFavsList->GetSelection() );
	delete path;

	if ( lstFavsList->GetCount() > 0 )
		lstFavsList->SetSelection( 0 );
}

void xaplManageFavoritesUI::OnExplore( wxCommandEvent &WXUNUSED(evt) )
{
	if ( lstFavsList->GetSelection() == wxNOT_FOUND ) return;

	wxString *path = (wxString *)lstFavsList->GetClientData( lstFavsList->GetSelection() );

#ifdef __WINDOWS__
	wxExecute( wxString::Format( EXPLORE_CMD, path->c_str() ) );
#else
	wxFileName fn( path->c_str() );
	wxExecute( wxString::Format( EXPLORE_CMD.c_str(), fn.GetPath().c_str() ) );
#endif

}

void xaplManageFavoritesUI::OnClose( wxCommandEvent &WXUNUSED(evt) )
{
	EndModal( 1 );
}
