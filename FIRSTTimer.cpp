/*
 * Timer.cpp
 *
 *  Created on: Jun 6, 2015
 *      Author: Mikhail Kyraha
 */

/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008. All Rights Reserved.							  */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/

#include <Arduino.h>
#include "FIRSTTimer.h"

/**
 * Create a new timer object.
 *
 * Create a new timer object and reset the time to zero. The timer is initially not running and
 * must be started.
 */
FIRSTTimer::FIRSTTimer()
	: m_startTime (0.0)
	, m_accumulatedTime (0.0)
	, m_running (false)
{
	//Creates a semaphore to control access to critical regions.
	//Initially 'open'
	//m_semaphore = initializeMutexNormal();
	Reset();
}

FIRSTTimer::~FIRSTTimer()
{
	//deleteMutex(m_semaphore);
}

/**
 * Get the current time from the timer. If the clock is running it is derived from
 * the current system clock the start time stored in the timer class. If the clock
 * is not running, then return the time when it was last stopped.
 *
 * @return Current time value for this timer in seconds
 */
double FIRSTTimer::Get()
{
	double result;
	double currentTime = GetFPGATimestamp();

	//Synchronized sync(m_semaphore);
	if(m_running)
	{
		// If the current time is before the start time, then the FPGA clock
		// rolled over.  Compensate by adding the ~71 minutes that it takes
		// to roll over to the current time.
		if(currentTime < m_startTime) {
			currentTime += kRolloverTime;
		}

		result = (currentTime - m_startTime) + m_accumulatedTime;
	}
	else
	{
		result = m_accumulatedTime;
	}

	return result;
}

/**
 * Reset the timer by setting the time to 0.
 *
 * Make the timer startTime the current time so new requests will be relative to now
 */
void FIRSTTimer::Reset()
{
	//Synchronized sync(m_semaphore);
	m_accumulatedTime = 0;
	m_startTime = GetFPGATimestamp();
}

/**
 * Start the timer running.
 * Just set the running flag to true indicating that all time requests should be
 * relative to the system clock.
 */
void FIRSTTimer::Start()
{
	//Synchronized sync(m_semaphore);
	if (!m_running)
	{
		m_startTime = GetFPGATimestamp();
		m_running = true;
	}
}

/**
 * Stop the timer.
 * This computes the time as of now and clears the running flag, causing all
 * subsequent time requests to be read from the accumulated time rather than
 * looking at the system clock.
 */
void FIRSTTimer::Stop()
{
	double temp = Get();

	//Synchronized sync(m_semaphore);
	if (m_running)
	{
		m_accumulatedTime = temp;
		m_running = false;
	}
}

/**
 * Check if the period specified has passed and if it has, advance the start
 * time by that period. This is useful to decide if it's time to do periodic
 * work without drifting later by the time it took to get around to checking.
 *
 * @param period The period to check for (in seconds).
 * @return True if the period has passed.
 */
bool FIRSTTimer::HasPeriodPassed(double period)
{
	if (Get() > period)
	{
		//Synchronized sync(m_semaphore);
		// Advance the start time by the period.
		m_startTime += period;
		// Don't set it to the current time... we want to avoid drift.
		return true;
	}
	return false;
}

/**
 * Return the FPGA system clock time in seconds.
 *
 * Return the time from the FPGA hardware clock in seconds since the FPGA
 * started.
 * Rolls over after 71 minutes.
 * @returns Robot running time in seconds.
 */
double FIRSTTimer::GetFPGATimestamp()
{
	// FPGA returns the timestamp in microseconds
	// Call the helper GetFPGATime() in Utility.cpp
	return millis() * 1.0e-3;
}
