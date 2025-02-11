#include <wx/wx.h>
#include <wx/ipc.h>
#include <wx/tokenzr.h>
#include "xaplApplication.h"
#include "xaplPlaylistWindow.h"

DECLARE_APP( xaplApplication )

/* Taken from the wxBook, pp. 507-10 */

// Server class, for listening to connection requests
class xaplServer
	: public wxServer
{
public:
    wxConnectionBase *OnAcceptConnection(const wxString& topic);
};

// Connection class, for use by both communicating instances
class xaplConnection 
	: public wxConnection
{
public:
    xaplConnection() {}
    ~xaplConnection() {}

    bool OnExecute(const wxString& topic, wxChar*data, int size, wxIPCFormat format);
};

// Client class, to be used by subsequent instances in OnInit
class xaplClient
	: public wxClient
{
public:
    xaplClient() {};
    wxConnectionBase *OnMakeConnection() { return new xaplConnection; }
};


// Accepts a connection from another instance
wxConnectionBase *xaplServer::OnAcceptConnection(const wxString& topic)
{
    if (topic.Lower() == wxT("xapl"))
    {
        // Check that there are no modal dialogs active
        wxWindowList::Node* node = wxTopLevelWindows.GetFirst();
        while (node)
        {
            wxDialog* dialog = wxDynamicCast(node->GetData(), wxDialog);
            if (dialog && dialog->IsModal())
            {
                return false;
            }

            node = node->GetNext();
        }
        return new xaplConnection();
    }
    else
        return NULL;
}

// Opens a file passed from another instance
bool xaplConnection::OnExecute(const wxString& WXUNUSED(topic),
                             wxChar *data,
                             int WXUNUSED(size),
                             wxIPCFormat WXUNUSED(format))
{
	xaplPlaylistWindow *frame = wxDynamicCast( wxGetApp().GetTopWindow(), xaplPlaylistWindow );
	if ( frame )
	{
		wxString sData( data );
		if ( data != wxEmptyString )
		{
			sData = sData.Trim().Trim( false );

			// Request from the updater to close the app
			if ( sData.Cmp( wxT("CLOSE") ) == 0 )
			{
				frame->Hide();
				frame->Close();
			}
			// Files to add
			else
			{
				wxArrayString files;
				wxStringTokenizer tok( sData, wxT("?") );
				while ( tok.HasMoreTokens() )
					files.Add( tok.GetNextToken() );

				frame->playlist->AddItems( files );
			}
		}

		if ( frame->IsShown() )
		{
			frame->Raise();
		}
		else // minimized to system tray...
		{
			frame->SetPosition( wxGetApp().GetConfig()->windowPosition );
			frame->Iconize( false );
			frame->SetFocus();
			frame->Raise();
			frame->Show( true );
			wxGetApp().GetConfig()->windowMinimized = false;

			frame->taskBarIcon->SetIconVisible( false );
		}

	}

    return true;
}
