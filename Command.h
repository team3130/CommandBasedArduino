/*
 * MooseLights.hpp
 *
 *  Created on: May 1, 2015
 *      Author: Mikhail Kyraha
 */

#ifndef MOOSELIGHTS_HPP_
#define MOOSELIGHTS_HPP_

#include "Arduino.h"

#ifndef NULL
#define NULL 0
#endif

class CommandGroup;
class Subsystem;

struct SubsystemSetItem {
	Subsystem *m_system;
	SubsystemSetItem *next;
};

class SubsystemSet {
private:
	SubsystemSetItem *root;
public:
	SubsystemSet();
	~SubsystemSet();
	void insert(Subsystem *pSystem);
	int count(Subsystem *pSystem);
};

class Command
{
        friend class CommandGroup;
        friend class Scheduler;
public:
        Command();
        Command(const char *name);
        Command(double timeout);
        Command(const char *name, double timeout);
        virtual ~Command();
        double TimeSinceInitialized();
        void Requires(Subsystem *s);
        bool IsCanceled();
        void Start();
        bool Run();
        void Cancel();
        bool IsRunning();
        bool IsInterruptible();
        void SetInterruptible(bool interruptible);
        bool DoesRequire(Subsystem *subsystem);
        /* typedef std::set<Subsystem *> SubsystemSet; */
        SubsystemSet GetRequirements();
        CommandGroup *GetGroup();
        int GetID();

protected:
        void SetTimeout(double timeout);
        bool IsTimedOut();
        bool AssertUnlocked(const char *message);
        void SetParent(CommandGroup *parent);
        virtual void Initialize() = 0;
        virtual void Execute() = 0;
        virtual bool IsFinished() = 0;
        virtual void End() = 0;
        virtual void Interrupted() = 0;
        virtual void _Initialize();
        virtual void _Interrupted();
        virtual void _Execute();
        virtual void _End();
        virtual void _Cancel();

private:
         void InitCommand(const char *name, double timeout);
         void LockChanges();
         /*synchronized*/ void Removed();
         void StartRunning();
         void StartTiming();

         String m_name;
         double m_startTime;
         double m_timeout;
         bool m_initialized;
         SubsystemSet m_requirements;
         bool m_running;
         bool m_interruptible;
         bool m_canceled;
         bool m_locked;
         bool m_runWhenDisabled;
         CommandGroup *m_parent;
         int m_commandID;
         static int m_commandCounter;

public:
         virtual String GetName();
};


#endif /* MOOSELIGHTS_HPP_ */
