#ifndef __XAPL_FAVORITES_MANAGER_H
#define __XAPL_FAVORITES_MANAGER_H

#include <wx/wx.h>
#include <wx/gbsizer.h>
#include <vector>
#include "xaplMenuBar.h"
#include "xaplPlaylistCtrl.h"

using std::vector;

typedef struct
{
	size_t id;
	wxString path;
	wxString name;
} xaplFavorite;

// Favorites manager
class xaplFavoritesManager
{
private:
	wxString m_dataPath;
	xaplMenuBar *m_menu;
	wxMenu *m_addToMenu;
	xaplPlaylistCtrl *m_playlist;
	vector<xaplFavorite> m_favorites;
	int m_currId;
	
	short int m_accelId;

public:
	xaplFavoritesManager( xaplPlaylistCtrl *pl, xaplMenuBar *menu, wxMenu *addToMenu );
	~xaplFavoritesManager( void );

	void AddFavorite( const wxString &path );
	void RemoveFavorite( int id );
	void RemoveFavorite( const wxString &path );
	void RefreshFavorites( void );

	void ManageFavorites( void );
	wxString GetPath( int id );
};

// Add favorite GUI
class xaplAddFavoriteUI :
	public wxDialog
{
private:
	wxGridBagSizer *gridbag;

	wxButton *btnOK;
	wxButton *btnCancel;

	wxStaticBitmap *imgAddFav;
	wxStaticText *lblTitle;
	wxStaticText *lblInstructions;
	wxTextCtrl *txtFavName;

protected:
	void Initialize( void );

public:
	xaplAddFavoriteUI( wxWindow *parent );
	~xaplAddFavoriteUI( void );

	wxString GetPlaylistName( void ) const;
	wxString GetPlaylistPath( void ) const;
	void OnEnter( wxCommandEvent &WXUNUSED(evt) );

	DECLARE_EVENT_TABLE();
};

// Manage favorites GUI
class xaplManageFavoritesUI :
	public wxDialog
{
private:
	vector<xaplFavorite> *m_favorites;

	wxGridBagSizer *gridbag;

	wxListBox *lstFavsList;
	wxButton *btnRename;
	wxButton *btnDelete;
	wxButton *btnExplore;
	wxButton *btnClose;

protected:
	void Initialize( void );

public:
	xaplManageFavoritesUI( wxWindow *parent, vector<xaplFavorite> *favs );
	~xaplManageFavoritesUI();

	void OnRename( wxCommandEvent &WXUNUSED(evt) );
	void OnDelete( wxCommandEvent &WXUNUSED(evt) );
	void OnExplore( wxCommandEvent &WXUNUSED(evt) );
	void OnClose( wxCommandEvent &WXUNUSED(evt) );

	DECLARE_EVENT_TABLE();
};

#endif
