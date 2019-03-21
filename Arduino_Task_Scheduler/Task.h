/*
 * Copyright 2011 Alan Burlison, alan@bleaklow.com.  All rights reserved.
 * Use is subject to license terms.
 */

/*
 * Simple task classes.
 */

#ifndef Task_h
#define Task_h

#include <stdint.h>

// Maximum time into the future - approximately 50 days.
#define MAX_TIME UINT32_MAX

/*
 * A simple (abstract) task - base class for all other tasks.
 */
class Task {
/*
*	Newbie Note: The "= 0;" following each virtual method 
*	in the Task Class makes it an "abstract" or "pure virtual"
*	method in C++. These methods *must* be defined/implemented in the
*	derived class. These are *only* Task Class. -KG 3-19-2019
*/

public:
    /*
     * Can the task currently run?
     * now - current time, in milliseconds.
     */
    virtual bool canRun(uint32_t now) = 0;		//<--ABSTRACT

    /*
     * Run the task,
     * now - current time, in milliseconds.
     */
    virtual void run(uint32_t now) = 0;			//<--ABSTRACT
};

/*
 * A task that is triggered by an external event.
 */
class TriggeredTask : public Task {

public:
    /*
     * Can the task currently run?
     * now - current time, in milliseconds.
     */
    virtual bool canRun(uint32_t now);

    /*
     * Mark the task as runnable.
     */
    inline void setRunnable() { runFlag = true; }

    /*
     * Mark the task as non-runnable.
     */
    inline void resetRunnable() { runFlag = false; }

protected:
    bool runFlag;   // True if the task is currently runnable.
};

/*
 * A task that is run on a periodic basis.
 */
class TimedTask : public Task {

public:
    /*
     * Create a periodically executed task.
     * when - the system clock tick when the task should run, in milliseconds.
     */
    inline TimedTask(uint32_t when) { runTime = when; }

    /*
     * Can the task currently run?
     * now - current system clock tick, in milliseconds.
     */
    virtual bool canRun(uint32_t now);

    /*
     * Set the system clock tick when the task can next run.
     * when - the system clock tick when the task should run, in milliseconds.
     */
    inline void setRunTime(uint32_t when) { runTime = when; }

    /*
     * Increment the system clock tick when the task can next run.
     * inc - system clock increment, in milliseconds.
     */
    inline void incRunTime(uint32_t inc) { runTime += inc; }

    /*
     * Get the system clock tick when the task can next run.
     * return - system clock tick when the task is next due to run.
     */
    inline uint32_t getRunTime() { return runTime; }

protected:
    
    uint32_t runTime;   // The  system clock tick when the task can next run.
};

#endif
