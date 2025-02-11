/* 
  Copyright (c) 2009, Xive Software
  All rights reserved.
  
  Redistribution and use in source and binary forms, with or without modification, 
  are permitted provided that the following conditions are met:

     * Redistributions of source code must retain the above copyright notice, this 
	   list of conditions and the following disclaimer.
     * Redistributions in binary form must reproduce the above copyright notice, 
	   this list of conditions and the following disclaimer in the documentation 
	   and/or other materials provided with the distribution.
     * Neither the name of Xive Software nor the names of its contributors 
	   may be used to endorse or promote products derived from this software 
	   without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY 
  EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES 
  OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT 
  SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, 
  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED 
  TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR 
  BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN 
  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH 
  DAMAGE.
*/

#ifndef __XAPL_SLEEP_TIMER_H
#define __XAPL_SLEEP_TIMER_H

#include <wx/wx.h>
#include <wx/gbsizer.h>
#include <wx/spinctrl.h>
#include "xaplConstants.h"

// UI for showing timer progress or aborting the timer
class xaplSleepTimer;

class xaplSleepTimerCountdownUI :
	public wxDialog
{
	friend class xaplSleepTimer;

private:
	xaplSleepTimer *m_timer;

	wxStaticBitmap *m_imgSleepTimer;
	wxStaticText *m_lblTitle;
	wxStaticText *m_lblInstructions;
	wxStaticText *m_lblElapsedTime;

	wxStaticText *m_lblAction;
	wxRadioButton *m_rdoCloseApp;
	wxRadioButton *m_rdoSleepComputer;
	wxRadioButton *m_rdoShutdownComputer;

	wxButton *m_btnAbort;
	wxButton *m_btnHide;

protected:
	void Initialize( void );

public:
	xaplSleepTimerCountdownUI( wxWindow *parent, int id );
	~xaplSleepTimerCountdownUI( void );

	void InitSleepAction( int action );
	void SetTimer( xaplSleepTimer *timer );

	void OnSleepActionChanged( wxCommandEvent &WXUNUSED(evt) );
	void OnButtonAbort( wxCommandEvent &WXUNUSED(evt) );
	void OnButtonHide( wxCommandEvent &WXUNUSED(evt) );

	DECLARE_EVENT_TABLE();
};

// Actions that may be taken when the sleep timer finishes
typedef enum
{
	SA_CLOSEAPP,
	SA_SHUTDOWN,
	SA_SLEEP,
	SA_HIBERNATE
} xaplSleepAction;

// wxTimer class for keeping track of time
class xaplSleepTimer :
	public wxTimer
{
private:
	xaplSleepTimerCountdownUI *m_ui;
	unsigned int m_minutes;
	unsigned int m_elapsed;
	int m_sleepAction;

public:
	xaplSleepTimer( xaplSleepTimerCountdownUI *ui );
	xaplSleepTimer( xaplSleepTimerCountdownUI *ui, unsigned int minutes, int action = SA_CLOSEAPP );
	bool Start( void );
	void SetMinutes( unsigned int minutes );
	unsigned int GetMinutes( void );
	unsigned int GetElapsedSeconds( void );
	void SetSleepAction( int action );
	void Notify( void );
};

// UI for starting a sleep timer
class xaplSleepTimerUI :
	public wxDialog
{
private:
	xaplSleepTimer *m_timer;
	xaplSleepTimerCountdownUI *m_countdown;

	wxStaticBitmap *m_imgSleepTimer;
	wxStaticText *m_lblTitle;
	wxStaticText *m_lblInstructions;

	wxSpinCtrl *m_spnMinutes;
	wxStaticText *m_lblAction;
	wxRadioButton *m_rdoCloseApp;
	wxRadioButton *m_rdoSleepComputer;
	wxRadioButton *m_rdoShutdownComputer;
	
	wxButton *m_btnStart;
	wxButton *m_btnCancel;

protected:
	void Initialize( void );

public:
	xaplSleepTimerUI( wxWindow *parent, int id );
	~xaplSleepTimerUI( void );

	int Show( void );
	void OnButtonStart( wxCommandEvent &WXUNUSED(evt) );
	void OnButtonCancel( wxCommandEvent &WXUNUSED(evt) );
	void OnEnter( wxCommandEvent &WXUNUSED(evt) );

	bool GetTimerRunning( void );

	DECLARE_EVENT_TABLE();
};

#endif
