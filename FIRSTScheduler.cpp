/*
 * Scheduler.cpp
 *
 *  Created on: May 4, 2015
 *      Author: Mikhail Kyraha
 */

/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2011. All Rights Reserved.							  */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/

#include "FIRSTScheduler.h"
#include "FIRSTSubsystem.h"

FIRSTScheduler *FIRSTScheduler::_instance = NULL;

FIRSTScheduler::FIRSTScheduler() :
	m_adding(false) {
	m_enabled = true;
	m_runningCommandsChanged = false;
}

FIRSTScheduler::~FIRSTScheduler() {
}

/**
 * Returns the {@link Scheduler}, creating it if one does not exist.
 * @return the {@link Scheduler}
 */
FIRSTScheduler *FIRSTScheduler::GetInstance() {
	if (_instance == NULL)
		_instance = new FIRSTScheduler();
	return _instance;
}

void FIRSTScheduler::SetEnabled(bool enabled) {
	m_enabled = enabled;
}

/**
 * Add a command to be scheduled later.
 * In any pass through the scheduler, all commands are added to the additions list, then
 * at the end of the pass, they are all scheduled.
 * @param command The command to be scheduled
 */
void FIRSTScheduler::AddCommand(FIRSTCommand *command) {
	if (m_additions.find(command) != m_additions.end())
		return;
	m_additions.push_back(command);
}

void FIRSTScheduler::ProcessCommandAddition(FIRSTCommand *command) {
	if (command == NULL)
		return;

	// Check to make sure no adding during adding
	if (m_adding) {
		//wpi_setWPIErrorWithContext(IncompatibleState, "Can not start command from cancel method");
		return;
	}

	// Only add if not already in
	CommandVector::iterator found = m_commands.find(command);
	if (found == m_commands.end()) {
		// Check that the requirements can be had
		FIRSTCommand::SubsystemSet requirements = command->GetRequirements();
		FIRSTCommand::SubsystemSet::iterator iter;
		for (iter = requirements.begin(); iter != requirements.end(); iter++) {
			FIRSTSubsystem *lock = *iter;
			if (lock->GetCurrentCommand() != NULL
					&& !lock->GetCurrentCommand()->IsInterruptible())
				return;
		}

		// Give it the requirements
		m_adding = true;
		for (iter = requirements.begin(); iter != requirements.end(); iter++) {
			FIRSTSubsystem *lock = *iter;
			if (lock->GetCurrentCommand() != NULL) {
				lock->GetCurrentCommand()->Cancel();
				Remove(lock->GetCurrentCommand());
			}
			lock->SetCurrentCommand(command);
		}
		m_adding = false;

		m_commands.insert(command);

		command->StartRunning();
		m_runningCommandsChanged = true;
	}
}

/**
 * Runs a single iteration of the loop.  This method should be called often in order to have a functioning
 * {@link Command} system.  The loop has five stages:
 *
 * <ol>
 * <li> Poll the Buttons </li>
 * <li> Execute/Remove the Commands </li>
 * <li> Send values to SmartDashboard </li>
 * <li> Add Commands </li>
 * <li> Add Defaults </li>
 * </ol>
 */
void FIRSTScheduler::Run() {
/*	// Get button input (going backwards preserves button priority)
	{
		if (!m_enabled)
			return;

		Synchronized sync(m_buttonsLock);
		ButtonVector::reverse_iterator rButtonIter = m_buttons.rbegin();
		for (; rButtonIter != m_buttons.rend(); rButtonIter++) {
			(*rButtonIter)->Execute();
		}
	} */

	m_runningCommandsChanged = false;

	// Loop through the commands
	CommandVector::iterator commandIter = m_commands.begin();
	for (; commandIter != m_commands.end();) {
		FIRSTCommand *command = *commandIter;
		// Increment before potentially removing to keep the iterator valid
		commandIter++;
		if (!command->Run()) {
			Remove(command);
			m_runningCommandsChanged = true;
		}
	}

	// Add the new things
	{
		//Synchronized sync(m_additionsLock);
		CommandVector::iterator additionsIter = m_additions.begin();
		for (; additionsIter != m_additions.end(); additionsIter++) {
			ProcessCommandAddition(*additionsIter);
		}
		m_additions.clear();
	}

	// Add in the defaults
	FIRSTCommand::SubsystemSet::iterator subsystemIter = m_subsystems.begin();
	for (; subsystemIter != m_subsystems.end(); subsystemIter++) {
		FIRSTSubsystem *lock = *subsystemIter;
		if (lock->GetCurrentCommand() == NULL) {
			ProcessCommandAddition(lock->GetDefaultCommand());
		}
		lock->ConfirmCommand();
	}
}

/**
 * Registers a {@link Subsystem} to this {@link Scheduler}, so that the {@link Scheduler} might know
 * if a default {@link Command} needs to be run.  All {@link Subsystem Subsystems} should call this.
 * @param system the system
 */
void FIRSTScheduler::RegisterSubsystem(FIRSTSubsystem *subsystem) {
	if (subsystem == NULL) {
		//wpi_setWPIErrorWithContext(NullParameter, "subsystem");
		return;
	}
	m_subsystems.insert(subsystem);
}

/**
 * Removes the {@link Command} from the {@link Scheduler}.
 * @param command the command to remove
 */
void FIRSTScheduler::Remove(FIRSTCommand *command) {
	if (command == NULL) {
		//wpi_setWPIErrorWithContext(NullParameter, "command");
		return;
	}

	if (!m_commands.erase(command))
		return;

	FIRSTCommand::SubsystemSet requirements = command->GetRequirements();
	FIRSTCommand::SubsystemSet::iterator iter = requirements.begin();
	for (; iter != requirements.end(); iter++) {
		FIRSTSubsystem *lock = *iter;
		lock->SetCurrentCommand(NULL);
	}

	command->Removed();
}

void FIRSTScheduler::RemoveAll() {
	while (m_commands.size() > 0) {
		Remove(*m_commands.begin());
	}
}

/**
 * Completely resets the scheduler. Undefined behavior if running.
 */
void FIRSTScheduler::ResetAll()
{
	RemoveAll();
	m_subsystems.clear();
	m_additions.clear();
	m_commands.clear();
}

String FIRSTScheduler::GetName() {
	return "Scheduler";
}

String FIRSTScheduler::GetType() {
	return "Scheduler";
}
