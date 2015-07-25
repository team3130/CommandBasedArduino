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

#include "FIRSTCommand.h"

class ButtonScheduler;
class FIRSTSubsystem;

class FIRSTScheduler
{
public:
	static FIRSTScheduler *GetInstance();

	void AddCommand(FIRSTCommand* command);
	void RegisterSubsystem(FIRSTSubsystem *subsystem);
	void Run();
	void Remove(FIRSTCommand *command);
	void RemoveAll();
	void ResetAll();
	void SetEnabled(bool enabled);

	String GetName();
	String GetType();

private:
	FIRSTScheduler();
	virtual ~FIRSTScheduler();

	void ProcessCommandAddition(FIRSTCommand *command);

	static FIRSTScheduler *_instance;
	FIRSTCommand::SubsystemSet m_subsystems;
	typedef AVector<FIRSTCommand *> CommandVector;
	CommandVector m_additions;
	CommandVector m_commands;
	bool m_adding;
	bool m_enabled;
	bool m_runningCommandsChanged;
};


#endif /* SCHEDULER_H_ */
