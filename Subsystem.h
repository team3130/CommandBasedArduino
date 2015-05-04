/*
 * Subsystem.h
 *
 *  Created on: May 3, 2015
 *      Author: U0172740
 */

#ifndef SUBSYSTEM_H_
#define SUBSYSTEM_H_

#include "Arduino.h"

class Command;

class Subsystem {
    friend class Scheduler;
public:
    Subsystem(const char *name);
    virtual ~Subsystem() {}

    void SetDefaultCommand(Command *command);
    Command *GetDefaultCommand();
    void SetCurrentCommand(Command *command);
    Command *GetCurrentCommand();
    virtual void InitDefaultCommand();

private:
    void ConfirmCommand();

    Command *m_currentCommand;
    bool m_currentCommandChanged;
    Command *m_defaultCommand;
    String m_name;
    bool m_initializedDefaultCommand;

public:
    virtual String GetName();
};




#endif /* SUBSYSTEM_H_ */
