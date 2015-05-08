/*
 * Scheduler.h
 *
 *  Created on: May 4, 2015
 *      Author: Mikhail Kyraha
 */

/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2011. All Rights Reserved.							  */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/

#ifndef __SCHEDULER_H__
#define __SCHEDULER_H__

#include "Arduino.h"
#include "Command.h"

class ButtonScheduler;
class Subsystem;

class Scheduler
{
public:
	static Scheduler *GetInstance();

	void AddCommand(Command* command);
	void RegisterSubsystem(Subsystem *subsystem);
	void Run();
	void Remove(Command *command);
	void RemoveAll();
	void ResetAll();
	void SetEnabled(bool enabled);

	String GetName();
	String GetType();

private:
	Scheduler();
	virtual ~Scheduler();

	void ProcessCommandAddition(Command *command);

	static Scheduler *_instance;
	Command::SubsystemSet m_subsystems;
	typedef AVector<Command *> CommandVector;
	CommandVector m_additions;
	CommandVector m_commands;
	bool m_adding;
	bool m_enabled;
	bool m_runningCommandsChanged;
};


#endif /* SCHEDULER_H_ */
