#include <cstdlib>
#include <ctime>
#include <cmath>
#include <wx/colour.h>
#ifdef __WINDOWS__
  #include <wx/msw/registry.h>
#endif
#include "xaplConstants.h"
#include "xaplUtilities.h"
#include "xaplApplication.h"

#ifdef _DEBUG
#include <crtdbg.h>
#define DEBUG_NEW new(_NORMAL_BLOCK ,__FILE__, __LINE__)
#else
#define DEBUG_NEW new
#endif
#ifdef _DEBUG
#define new DEBUG_NEW
#endif 

wxString xaplUtilities::GetFormattedTime( int inputSecnds )
{
	wxString retStr = wxEmptyString;

	int minutes = inputSecnds / 60;
	int seconds = inputSecnds % 60;

	wxString extraZero = (seconds >= 0 && seconds < 10) || (seconds < 0 && seconds > -10) ? wxT("0") : wxEmptyString;

	retStr << minutes << wxT(":") << extraZero << abs(seconds);
	return retStr;
}

wxString xaplUtilities::FormatThousands( const wxString &s )
{
    static wxString thousandssep = wxT(",");
    wxString in = s, out;
    while ( in.Length() > 3 ) 
	{
        out.Prepend( thousandssep + in.Right(3) );
        in.RemoveLast( 3 );
    }

    if ( !in.IsEmpty() )
        out.Prepend( in );

    return out;
} 

int xaplUtilities::RandomInt( int start, int end )
{
	return start + rand() / ( RAND_MAX / ( end - start ) + 1 );
}

int xaplUtilities::IntSortFunc( int *first, int *second )
{
	if ( *first < *second ) 
		return -1;
	else if ( *first == *second ) 
		return 0;

	return 1;
}

unsigned int xaplUtilities::GetTimeSeed( void )
{
	time_t now = time ( 0 );
	unsigned char *p = (unsigned char *)&now;
	unsigned seed = 0;
	size_t i;

	for ( i = 0; i < sizeof now; i++ )
		seed = seed * ( UCHAR_MAX + 2U ) + p[i];

	return seed;
}

int xaplUtilities::GetColorBrightness( const wxColour &color )
{
	return (int) sqrt( color.Red() * color.Red() * .241 
						+ color.Green() * color.Green() * .691 
						+ color.Blue() * color.Blue() * .068 );
}

// Stores the previous association and then associates Xapl with the specified extension
void xaplUtilities::SetTypeAssociation( const wxString &ext, const wxString &mimeType, const wxString &formatName )
{
#ifdef __WINDOWS__
	// Use registry
	wxRegKey *key = new wxRegKey( wxString::Format( wxT("HKEY_CLASSES_ROOT\\%s"), ext ) );

	// Back up old entry
	if ( key->Exists() )
	{
		wxString value = key->QueryDefaultValue();
		if ( value != wxEmptyString )
		{
			// Don't save if we are already the default
			if ( value.Cmp( wxString::Format( wxT("Xapl - %s"), formatName ) ) != 0 )
			{
				xaplConfiguration *config = wxGetApp().GetConfig();

				if ( ext.Cmp( wxT(".mp3") ) == 0 )
					config->prevMp3Assoc = value;
				else if ( ext.Cmp( wxT(".ogg") ) == 0 )
					config->prevOggAssoc = value;
				else if ( ext.Cmp( wxT(".wav") ) == 0 )
					config->prevWavAssoc = value;
				else if ( ext.Cmp( wxT(".wma") ) == 0 )
					config->prevWmaAssoc = value;
				else if ( ext.Cmp( wxT(".aac") ) == 0 )
					config->prevAacAssoc = value;
				else if ( ext.Cmp( wxT(".flac") ) == 0 )
					config->prevFlacAssoc = value;
				
				wxGetApp().SaveConfig();
			}
		}
	}

	// Write new entry
	key->Create();
	wxString index = wxString::Format( wxT("Xapl - %s"), formatName );
	key->SetValue( wxEmptyString, index );
	key->SetValue( wxT("Content Type"), mimeType );
	key->SetValue( wxT("PerceivedType"), wxT("audio") );
	key->Close();
	delete key;

	wxRegKey *defIcoKey = new wxRegKey( wxString::Format( wxT("HKEY_CLASSES_ROOT\\%s\\DefaultIcon"), index ) );
	defIcoKey->Create();
	defIcoKey->SetValue( wxEmptyString, wxString::Format( wxT("%sXapl.exe,0"), EXECUTABLE_DIR ) );
	defIcoKey->Close();
	delete defIcoKey;

	wxRegKey *idxKey = new wxRegKey( wxString::Format( wxT("HKEY_CLASSES_ROOT\\%s\\shell\\open\\command"), index ) );
	idxKey->Create();
	idxKey->SetValue( wxEmptyString, wxString::Format( wxT("\"%sXapl.exe\" \"%%1\""), EXECUTABLE_DIR ) );
	idxKey->Close();
	delete idxKey;
#elif __LINUX__

#endif
}

// Removes Xapl's association and restores the previous association, if there was one
void xaplUtilities::UnSetTypeAssociation( const wxString &ext )
{
#ifdef __WINDOWS__
	// Use registry
	wxRegKey *key = new wxRegKey( wxString::Format( wxT("HKEY_CLASSES_ROOT\\%s"), ext ) );
	xaplConfiguration *config = wxGetApp().GetConfig();
	wxString oldAssoc, currentAssoc;

	if ( ext.Cmp( wxT(".mp3") ) == 0 )
		oldAssoc = config->prevMp3Assoc;
	else if ( ext.Cmp( wxT(".ogg") ) == 0 )
		oldAssoc = config->prevOggAssoc;
	else if ( ext.Cmp( wxT(".wav") ) == 0 )
		oldAssoc = config->prevWavAssoc;
	else if ( ext.Cmp( wxT(".wma") ) == 0 )
		oldAssoc = config->prevWmaAssoc;
	else if ( ext.Cmp( wxT(".aac") ) == 0 )
		oldAssoc = config->prevAacAssoc;
	else if ( ext.Cmp( wxT(".flac") ) == 0 )
		oldAssoc = config->prevFlacAssoc;
	else 
		return;

	if ( oldAssoc == wxEmptyString ) return;

	if ( key->Exists() )
	{
		currentAssoc = key->QueryDefaultValue();
		key->SetValue( wxEmptyString, oldAssoc );
	}

	key->Close();
	delete key;

	// Clean up the details
	wxRegKey *detailsKey = new wxRegKey( wxString::Format( wxT("HKEY_CLASSES_ROOT\\%s"), currentAssoc ) );
	detailsKey->DeleteSelf();
	detailsKey->Close();
	delete detailsKey;

#elif __LINUX__

#endif
}
