/*
 * Command.cpp
 *
 *  Created on: May 3, 2015
 *      Author: Mikhail Kyraha
 *
 *  Adopted from FIRST Command
 */


/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2011. All Rights Reserved.							  */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/

#include "FIRSTCommand.h"
#include "FIRSTScheduler.h"

int FIRSTCommand::m_commandCounter = 0;

void FIRSTCommand::InitCommand(const char *name, double timeout)
{
	m_commandID = m_commandCounter++;
	m_timeout = timeout;
	m_locked = false;
	m_startTime = -1;
	m_initialized = false;
	m_running = false;
	m_interruptible = true;
	m_canceled = false;
	m_parent = NULL;
	m_name = name == NULL? String() : name;
}

/**
 * Creates a new command.
 * The name of this command will be default.
 */
FIRSTCommand::FIRSTCommand()
{
	InitCommand(NULL, -1.0);
}

/**
 * Creates a new command with the given name and no timeout.
 * @param name the name for this command
 */
FIRSTCommand::FIRSTCommand(const char *name)
{
	InitCommand(name, -1.0);
}

/**
 * Creates a new command with the given timeout and a default name.
 * @param timeout the time (in seconds) before this command "times out"
 * @see Command#isTimedOut() isTimedOut()
 */
FIRSTCommand::FIRSTCommand(double timeout)
{
	InitCommand(NULL, timeout);
}

/**
 * Creates a new command with the given name and timeout.
 * @param name the name of the command
 * @param timeout the time (in seconds) before this command "times out"
 * @see Command#isTimedOut() isTimedOut()
 */
FIRSTCommand::FIRSTCommand(const char *name, double timeout)
{
	InitCommand(name, timeout);
}

FIRSTCommand::~FIRSTCommand()
{
}

/**
 * Get the ID (sequence number) for this command
 * The ID is a unique sequence number that is incremented for each command.
 * @return the ID of this command
 */
int FIRSTCommand::GetID() {
	return m_commandID;
}

/**
 * Sets the timeout of this command.
 * @param timeout the timeout (in seconds)
 * @see Command#isTimedOut() isTimedOut()
 */
void FIRSTCommand::SetTimeout(double timeout)
{
	m_timeout = timeout;
}

/**
 * Returns the time since this command was initialized (in seconds).
 * This function will work even if there is no specified timeout.
 * @return the time since this command was initialized (in seconds).
 */
double FIRSTCommand::TimeSinceInitialized()
{
	if (m_startTime < 0.0)
		return 0.0;
	else
		return 0.001 * millis() - m_startTime;
}

/**
 * This method specifies that the given {@link Subsystem} is used by this command.
 * This method is crucial to the functioning of the Command System in general.
 *
 * <p>Note that the recommended way to call this method is in the constructor.</p>
 *
 * @param subsystem the {@link Subsystem} required
 * @see Subsystem
 */
void FIRSTCommand::Requires(FIRSTSubsystem *subsystem)
{
	if (!AssertUnlocked("Can not add new requirement to command"))
		return;

	if (subsystem != NULL)
		m_requirements.insert(subsystem);
}

/**
 * Called when the command has been removed.
 * This will call {@link Command#interrupted() interrupted()} or {@link Command#end() end()}.
 */
void FIRSTCommand::Removed()
{
	if (m_initialized)
	{
		if (IsCanceled())
		{
			Interrupted();
			_Interrupted();
		}
		else
		{
			End();
			_End();
		}
	}
	m_initialized = false;
	m_canceled = false;
	m_running = false;
}

/**
 * Starts up the command.  Gets the command ready to start.
 * <p>Note that the command will eventually start, however it will not necessarily
 * do so immediately, and may in fact be canceled before initialize is even called.</p>
 */
void FIRSTCommand::Start()
{
	LockChanges();
	if (m_parent != NULL)
		return;
		//wpi_setWPIErrorWithContext(CommandIllegalUse, "Can not start a command that is part of a command group");

	FIRSTScheduler::GetInstance()->AddCommand(this);
}

/**
 * The run method is used internally to actually run the commands.
 * @return whether or not the command should stay within the {@link Scheduler}.
 */
bool FIRSTCommand::Run()
{
	if (IsCanceled())
		return false;

	if (!m_initialized)
	{
		m_initialized = true;
		StartTiming();
		_Initialize();
		Initialize();
	}
	_Execute();
	Execute();
	return !IsFinished();
}

void FIRSTCommand::_Initialize()
{
}

void FIRSTCommand::_Interrupted()
{
}

void FIRSTCommand::_Execute()
{
}

void FIRSTCommand::_End()
{
}

/**
 * Called to indicate that the timer should start.
 * This is called right before {@link Command#initialize() initialize()} is, inside the
 * {@link Command#run() run()} method.
 */
void FIRSTCommand::StartTiming()
{
	m_startTime = 0.001 * millis();
}

/**
 * Returns whether or not the {@link Command#timeSinceInitialized() timeSinceInitialized()}
 * method returns a number which is greater than or equal to the timeout for the command.
 * If there is no timeout, this will always return false.
 * @return whether the time has expired
 */
bool FIRSTCommand::IsTimedOut()
{
	return m_timeout != -1 && TimeSinceInitialized() >= m_timeout;
}

/**
 * Returns the requirements (as an std::set of {@link Subsystem Subsystems} pointers) of this command
 * @return the requirements (as an std::set of {@link Subsystem Subsystems} pointers) of this command
 */
FIRSTCommand::SubsystemSet FIRSTCommand::GetRequirements()
{
	return m_requirements;
}

/**
 * Prevents further changes from being made
 */
void FIRSTCommand::LockChanges()
{
	m_locked = true;
}

/**
 * If changes are locked, then this will generate a CommandIllegalUse error.
 * @param message the message to report on error (it is appended by a default message)
 * @return true if assert passed, false if assert failed
 */
bool FIRSTCommand::AssertUnlocked(const char *message)
{
	if (m_locked)
	{
//		char buf[128];
//		snprintf(buf, 128, "%s after being started or being added to a command group", message);
//		wpi_setWPIErrorWithContext(CommandIllegalUse, buf);
		return false;
	}
	return true;
}

/**
 * Sets the parent of this command.  No actual change is made to the group.
 * @param parent the parent
 */
void FIRSTCommand::SetParent(CommandGroup *parent)
{
	if (parent == NULL)
	{
		//wpi_setWPIErrorWithContext(NullParameter, "parent");
		return;
	}
	else if (m_parent != NULL)
	{
		//wpi_setWPIErrorWithContext(CommandIllegalUse, "Can not give command to a command group after already being put in a command group");
		return;
	}
	else
	{
		LockChanges();
		m_parent = parent;
	}
}

/**
 * This is used internally to mark that the command has been started.
 * The lifecycle of a command is:
 *
 * startRunning() is called.
 * run() is called (multiple times potentially)
 * removed() is called
 *
 * It is very important that startRunning and removed be called in order or some assumptions
 * of the code will be broken.
 */
void FIRSTCommand::StartRunning()
{
	m_running = true;
	m_startTime = -1;
}

/**
 * Returns whether or not the command is running.
 * This may return true even if the command has just been canceled, as it may
 * not have yet called {@link Command#interrupted()}.
 * @return whether or not the command is running
 */
bool FIRSTCommand::IsRunning()
{
	return m_running;
}

/**
 * This will cancel the current command.
 * <p>This will cancel the current command eventually.  It can be called multiple times.
 * And it can be called when the command is not running.  If the command is running though,
 * then the command will be marked as canceled and eventually removed.</p>
 * <p>A command can not be canceled
 * if it is a part of a command group, you must cancel the command group instead.</p>
 */
void FIRSTCommand::Cancel()
{
	if (m_parent != NULL)
		return;
		//wpi_setWPIErrorWithContext(CommandIllegalUse, "Can not cancel a command that is part of a command group");

	_Cancel();
}

/**
 * This works like cancel(), except that it doesn't throw an exception if it is a part
 * of a command group.  Should only be called by the parent command group.
 */
void FIRSTCommand::_Cancel()
{
	if (IsRunning())
		m_canceled = true;
}

/**
 * Returns whether or not this has been canceled.
 * @return whether or not this has been canceled
 */
bool FIRSTCommand::IsCanceled()
{
	return m_canceled;
}

/**
 * Returns whether or not this command can be interrupted.
 * @return whether or not this command can be interrupted
 */
bool FIRSTCommand::IsInterruptible()
{
	return m_interruptible;
}

/**
 * Sets whether or not this command can be interrupted.
 * @param interruptible whether or not this command can be interrupted
 */
void FIRSTCommand::SetInterruptible(bool interruptible)
{
	m_interruptible = interruptible;
}

/**
 * Checks if the command requires the given {@link Subsystem}.
 * @param system the system
 * @return whether or not the subsystem is required (false if given NULL)
 */
bool FIRSTCommand::DoesRequire(FIRSTSubsystem *system)
{
	return m_requirements.count(system) > 0;
}

/**
 * Returns the {@link CommandGroup} that this command is a part of.
 * Will return null if this {@link Command} is not in a group.
 * @return the {@link CommandGroup} that this command is a part of (or null if not in group)
 */
CommandGroup *FIRSTCommand::GetGroup()
{
	return m_parent;
}

String FIRSTCommand::GetName()
{
	if (m_name.length() == 0)
	{
		m_name = String("Command_") + String((unsigned long)this);
	}
	return m_name;
}

