#include "xaplKmpSearchLib.h"
#include "xaplPlaylistCtrl.h"

/* ====================================================================
 * ==== xaplKmpSearchLib Implementation ===============================*/
bool xaplKmpSearchLib::KmpContains( const wxString &haystack, const wxString &needle, const int *table, bool ignoreCase )
{
	return KmpSearch( haystack, needle, table, ignoreCase ) != xaplNOT_FOUND;
}

int xaplKmpSearchLib::KmpSearch( const wxString &haystack, const wxString &needle, const int *table, bool ignoreCase )
{
	if ( haystack.empty() || table == NULL )
		return xaplNOT_FOUND;

	if ( needle.empty() )
		return 0;

	int m = 0, i = 0, hl = (int)haystack.length(), nl = (int)needle.length();
	wxString workingHaystack = ignoreCase ? haystack.Lower() : haystack;
	wxString workingNeedle = ignoreCase ? needle.Lower() : needle;

	while ( m + i < hl )
	{
		if ( workingNeedle[i] == workingHaystack[m + i] )
		{
			++i;
			if ( i == nl )
				return m;
		}
		else
		{
			m = m + i - table[i];
			if ( i > 0 )
				i = table[i];
		}
	}

	return xaplNOT_FOUND;
}

int* xaplKmpSearchLib::BuildKmpTable( const wxString &haystack, bool ignoreCase )
{
	if ( haystack.empty() )
		return NULL;

	wxString workingHaystack = ignoreCase ? haystack.Lower() : haystack;
	int pos = 2, cnd = 0, lh = (int)workingHaystack.length();
	int *table = new int[ workingHaystack.length() ];
	
	table[0] = -1;
	table[1] = 0;

	while ( pos < lh )
	{
		if ( workingHaystack[pos - 1] == workingHaystack[cnd] )
		{
			table[pos] = cnd + 1;
			++pos;
			++cnd;
		}
		else if ( cnd > 0 )
		{
			cnd = table[cnd];
		}
		else
		{
			table[pos] = 0;
			++pos;
		}
	}

	return table;
}

int** xaplKmpSearchLib::BuildKmpTable( const wxArrayString &haystack, bool ignoreCase )
{
	if ( haystack.empty() )
		return NULL;

	int **table = new int*[haystack.size()];

	for ( size_t i = 0; i < haystack.size(); ++i )
	{
		wxString current = haystack[i];
		table[i] = BuildKmpTable( current, ignoreCase );
	}

	return table;
}

// Define events
DEFINE_EVENT_TYPE(xaplEVT_KMP_TABLE_REBUILD_COMPLETE)

/* ====================================================================
 * ==== xaplKmpTableBuilderThread Implementation ======================*/
xaplKmpTableBuilderThread::xaplKmpTableBuilderThread( xaplPlaylistCtrl* playlist )
	: m_doRebuild( false ),
	  m_playlistCtrl( playlist ),
	  m_table( NULL )
{}

xaplKmpTableBuilderThread::~xaplKmpTableBuilderThread( void )
{
	// Clean up old table
	if ( m_table != NULL )
	{
		for ( size_t i = 0; i < sizeof(m_table) / sizeof(int); ++i )
		{
			delete [] m_table[i];
		}
		delete [] m_table;
	}
}

void xaplKmpTableBuilderThread::RebuildTable( void )
{
	m_doRebuild = true;
}

bool xaplKmpTableBuilderThread::IsRebuilding( void )
{
	return m_doRebuild;
}

int** xaplKmpTableBuilderThread::GetTable()
{
	return IsRebuilding() ? NULL : m_table;
}

void* xaplKmpTableBuilderThread::Entry( void )
{
	while ( IsAlive() && !TestDestroy() && !IsPaused() )
	{
		if ( m_doRebuild )
		{
			// Get strings to search
			m_playlistCtrl->sm_plDataMutex->Lock();
			wxArrayString paths;
			for ( size_t i = 0; i < m_playlistCtrl->GetPlaylistItemCount(); ++i )
			{
				paths.Add( m_playlistCtrl->GetItemByListIndex( i )->path );
			}
			m_playlistCtrl->sm_plDataMutex->Unlock();

			// Clean up old table
			if ( m_table != NULL )
			{
				for ( size_t i = 0; i < sizeof(m_table) / sizeof(int); ++i )
				{
					delete [] m_table[i];
				}
				delete [] m_table;
			}

			// Build new table
			m_table = xaplKmpSearchLib::BuildKmpTable( paths );
			paths.Clear();

			// Reset rebuild flag
			m_doRebuild = false;

			// Notify main thread that the rebuild is complete
			wxCommandEvent evt;
			evt.SetId( wxID_ANY );
			evt.SetEventType( xaplEVT_KMP_TABLE_REBUILD_COMPLETE );
			evt.SetClientData( m_table );
			wxPostEvent( m_playlistCtrl, evt );
		}

		wxThread::Sleep( 250 );
	}

	return NULL;
}