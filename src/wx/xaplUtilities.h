#ifndef __XAPL_UTILITIES_H
#define __XAPL_UTILITIES_H

#include <wx/string.h>

class xaplUtilities
{
public:
	static wxString		GetFormattedTime( int inputSecnds );
	static wxString		FormatThousands( const wxString &s );
	static int			RandomInt( int start, int end );
	static int			IntSortFunc( int *first, int *second );
	static unsigned int GetTimeSeed( void );
	static int			GetColorBrightness( const wxColour &color );
	static void			SetTypeAssociation( const wxString &ext, const wxString &mimeType, const wxString &formatName );
	static void			UnSetTypeAssociation( const wxString &ext );
};

#endif
