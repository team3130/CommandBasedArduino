/*
 * Subsystem.h
 *
 *  Created on: May 3, 2015
 *      Author: U0172740
 */

#ifndef FIRSTSUBSYSTEM_H_
#define FIRSTSUBSYSTEM_H_

#include <Arduino.h>

class FIRSTCommand;

class FIRSTSubsystem {
    friend class FIRSTScheduler;
public:
    FIRSTSubsystem(const char *name);
    virtual ~FIRSTSubsystem() {}

    void SetDefaultCommand(FIRSTCommand *command);
    FIRSTCommand *GetDefaultCommand();
    void SetCurrentCommand(FIRSTCommand *command);
    FIRSTCommand *GetCurrentCommand();
    virtual void InitDefaultCommand();

private:
    void ConfirmCommand();

    FIRSTCommand *m_currentCommand;
    bool m_currentCommandChanged;
    FIRSTCommand *m_defaultCommand;
    String m_name;
    bool m_initializedDefaultCommand;

public:
    virtual String GetName();
};




#endif /* FIRSTSUBSYSTEM_H_ */
