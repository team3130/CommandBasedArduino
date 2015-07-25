/*
 * MooseLights.hpp
 *
 *  Created on: May 1, 2015
 *      Author: Mikhail Kyraha
 */

#ifndef MOOSELIGHTS_HPP_
#define MOOSELIGHTS_HPP_

#include <Arduino.h>

#ifndef NULL
#define NULL 0
#endif

class CommandGroup;
class FIRSTSubsystem;

template<typename T> class AVector {
private:
	typedef struct sItem {
		T payload;
		sItem *next;
	} Item;
	Item *root;
	Item *last;
public:
	class iterator {
		Item *m_item;
	public:
		iterator(Item *item=NULL) {m_item = item;};
		iterator &operator++() {m_item=m_item->next; return *this;};
		iterator operator++(int) {
		        iterator tmp(*this); // copy
		        operator++(); // pre-increment
		        return tmp;   // return old value
		    };
		const T &operator*() { return m_item->payload; };
		bool operator==(const iterator& other){return this->m_item == other.m_item;}
		bool operator!=(const iterator& other){return !(*this == other);}
	};
	AVector() { root=NULL; last=NULL; };
	~AVector() { clear(); };
	AVector& operator= (const AVector& x) {
		clear();
		for( iterator iter = x.begin(); iter != x.end(); iter++)
			push_back(*iter);
	};
	void clear() {
		Item *p = root;
		while(p) {
			Item *t = p;
			p = p->next;
			delete t;
		}
	};
	iterator begin() { return iterator(root); };
	iterator end() { return iterator(NULL); };
	int size() const {int size=0; for(Item *p = root; p; p = p->next) size++; return size; };
	void insert(const T val) {
		Item *newItem = new Item;
		newItem->payload = val;
		newItem->next = root;
		root = newItem;
		if(last==NULL) last = newItem;
	};
	void push_back (const T val) {
		if(last==NULL) {
			insert(val);
		}
		else {
			Item *newItem = new Item;
			newItem->payload = val;
			newItem->next = NULL;
			last->next = newItem;
			last = newItem;
		}
	};
	int count(const T val) {
		int total = 0;
		for(Item *p = root; p; p = p->next) {
			if(p->payload == val) total++;
		}
		return total;
	};
	iterator find (const T val) const {
		for(Item *p = root; p; p = p->next) {
			if(p->payload == val) return iterator(p);
		}
		return iterator(NULL);
	};
	int erase (const T val) {
		int total = 0;
		Item *p = root;
		while(p) {
			Item *t = p;
			Item **s = &p;
			p = p->next;
			if(t->payload == val) {
				*s = t->next;
				delete t;
				total++;
			}
		}
		return total;
	}
};


class FIRSTCommand
{
        friend class CommandGroup;
        friend class FIRSTScheduler;
public:
        FIRSTCommand();
        FIRSTCommand(const char *name);
        FIRSTCommand(double timeout);
        FIRSTCommand(const char *name, double timeout);
        virtual ~FIRSTCommand();
        double TimeSinceInitialized();
        void Requires(FIRSTSubsystem *s);
        bool IsCanceled();
        void Start();
        bool Run();
        void Cancel();
        bool IsRunning();
        bool IsInterruptible();
        void SetInterruptible(bool interruptible);
        bool DoesRequire(FIRSTSubsystem *subsystem);
        typedef AVector<FIRSTSubsystem *> SubsystemSet;
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
