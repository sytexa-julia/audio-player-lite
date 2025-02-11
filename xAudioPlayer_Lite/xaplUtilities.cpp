#include "xaplUtilities.h"

wxString xaplUtilities::GetFormattedTime( int inputSecnds )
{
	wxString retStr = wxEmptyString;

	int minutes = inputSecnds / 60;
	int seconds = inputSecnds % 60;

	wxString extraZero = (seconds >= 0 && seconds < 10) || (seconds < 0 && seconds > -10) ? wxT("0") : wxEmptyString;

	retStr << minutes << wxT(":") << extraZero << abs(seconds);
	return retStr;
}
