// ***************************************************************
//  EQCException   �  date: 21/12/2007
//  -------------------------------------------------------------
//  Copyright (C) 2007 - All Rights Reserved
// ***************************************************************
// 
// ***************************************************************

#ifndef WIN32
	#include <sys/time.h>
#else
	#include <sys/timeb.h>
#endif

#include <iostream>
using namespace std;

#include "timer.h"

uint32 current_time = 0;
uint32 last_time = 0;

Timer::Timer() {
	timer_time = 0;
	start_time = current_time;
	set_at_trigger = timer_time;
	pUseAcurateTiming = false;
	enabled = false;
}

Timer::Timer(uint32 in_timer_time, bool iUseAcurateTiming) {
	timer_time = in_timer_time;
	start_time = current_time;
	set_at_trigger = timer_time;
	pUseAcurateTiming = iUseAcurateTiming;
	if (timer_time == 0) {
		enabled = false;
	}
	else {
		enabled = true;
	}
}

Timer::Timer(uint32 start, uint32 timer, bool iUseAcurateTiming = false) {
	timer_time = timer;
	start_time = start;
	set_at_trigger = timer_time;
	pUseAcurateTiming = iUseAcurateTiming;
	if (timer_time == 0) {
		enabled = false;
	}
	else {
		enabled = true;
	}
}

/* Reimplemented for MSVC - Bounce */
#ifdef WIN32
int gettimeofday (timeval *tp, ...)
{
	timeb tb;

	ftime (&tb);

	tp->tv_sec  = tb.time;
	tp->tv_usec = tb.millitm * 1000;

	return 0;
}
#endif

/* This function checks if the timer triggered */
bool Timer::Check(bool iReset)
{
	//newage: what is all this? Looks bad.
	/*if(!enabled){
		return false;
	}*/
	
	_CP(Timer_Check);
    if (this==0) { 
		cerr << "Null timer during ->Check()!?\n"; 
		return true; 
	}
//	if (!current_time || !start_time || !timer_time) {cerr << "Timer::Check on a timer that does not have a vital member defined.";
//	return true;}
    if (enabled && current_time-start_time > timer_time) {
		if (iReset) {
			if (pUseAcurateTiming)
				start_time += timer_time;
			else
				start_time = current_time; // Reset timer
			timer_time = set_at_trigger;
		}
		return true;
    }
	
    return false;
}

/* This function disables the timer */
void Timer::Disable() {
	enabled = false;
}

void Timer::Enable() {
	enabled = true;
}

/* This function set the timer and restart it */
void Timer::Start(uint32 set_timer_time, bool ChangeResetTimer) {	
    start_time = current_time;
	enabled = true;
    if (set_timer_time != 0)
    {	
		timer_time = set_timer_time;
		if (ChangeResetTimer)
			set_at_trigger = set_timer_time;
    }
}

/* This timer updates the timer without restarting it */
void Timer::SetTimer(uint32 set_timer_time) {
    /* If we were disabled before => restart the timer */
    if (!enabled) {
		start_time = current_time;
		enabled = true;
    }
    if (set_timer_time != 0) {
		timer_time = set_timer_time;
		set_at_trigger = set_timer_time;
    }
}

uint32 Timer::GetRemainingTime() {
    if (enabled) {
	    if (current_time-start_time > timer_time)
			return 0;
		else
			return (start_time + timer_time) - current_time;
    }
	else {
		return 0xFFFFFFFF;
	}
}

void Timer::SetAtTrigger(uint32 in_set_at_trigger, bool iEnableIfDisabled) {
	set_at_trigger = in_set_at_trigger;
	if (!Enabled() && iEnableIfDisabled) {
		Enable();
	}
}

void Timer::Trigger()
{
	enabled = true;

	timer_time = set_at_trigger;
	start_time = current_time-timer_time-1;
}

const uint32 Timer::GetCurrentTime()
{	
    return current_time;
}

//just to keep all time related crap in one place... not really related to timers.
const uint32 Timer::GetTimeSeconds() {
    struct timeval read_time;

    gettimeofday(&read_time,0);
    return(read_time.tv_sec);
}

const uint32 Timer::SetCurrentTime()
{
    struct timeval read_time;	
    uint32 this_time;

    gettimeofday(&read_time,0);
    this_time = read_time.tv_sec * 1000 + read_time.tv_usec / 1000;

    if (last_time == 0)
    {
		current_time = 0;
    }
    else
    {
		current_time += this_time - last_time;
    }
    
	last_time = this_time;

//	cerr << "Current time:" << current_time << endl;
	return current_time;
}