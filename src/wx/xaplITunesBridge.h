#ifndef __XAPL_ITUNES_BRIDGE_H
#define __XAPL_ITUNES_BRIDGE_H

#include "../../lib/include/iTunes/iTunesCOMInterface.h"

class xaplITunesBridge
{
private:
	IiTunes* iITunes;

	static xaplITunesBridge *m_instance;
	xaplITunesBridge( void );

public:
	static xaplITunesBridge *GetInstance( void );

	bool ExportPlaylistToITunes( void );
	bool ExportAllPlaylistsToITunes( void );
	bool ImportPlaylistFromITunes( void );
	bool ImportAllPlaylistsFromITunes( void );
};

#endif
