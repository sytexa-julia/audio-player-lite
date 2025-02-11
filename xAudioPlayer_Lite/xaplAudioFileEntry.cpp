#include <wx/file.h>
#include "xaplAudioFileEntry.h"

xaplAudioFileEntry::xaplAudioFileEntry( void )
{
	index = -1;
	pathToFile = wxEmptyString;
	artist = wxEmptyString;
	album = wxEmptyString;
	title = wxEmptyString;
	genre = wxEmptyString;
	label= wxEmptyString; 
	songNum = 0;
}

xaplAudioFileEntry::xaplAudioFileEntry( long idx, wxString pth, wxString art, wxString albm, wxString ttl, wxString gnre, wxString lbl, int song )
	: index( idx ), pathToFile( pth ), artist( art ), album( albm ), title( ttl ), genre( gnre ), label( lbl ), songNum( song )
{ }

bool xaplAudioFileEntry::FileExists( void ) const
{
	if ( pathToFile.Len() > 0 )
		return wxFile::Exists( pathToFile.c_str() );
	
	return false;
}

WX_DEFINE_OBJARRAY( xaplAudioFileArray );
