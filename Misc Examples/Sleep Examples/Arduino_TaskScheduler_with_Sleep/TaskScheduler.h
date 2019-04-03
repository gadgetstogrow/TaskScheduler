/*
 * Copyright 2011 Alan Burlison, alan@bleaklow.com.  All rights reserved.
 * Use is subject to license terms.
 */

/*
 * A simple priority-based task scheduler.
 */

#ifndef TaskScheduler_h
#define TaskScheduler_h

#include "Task.h"

// Calculate the number of tasks in the array, given the size.
#define NUM_TASKS(T) (sizeof(T) / sizeof(Task))

class TaskScheduler {

public:
    /*
     * Create a new task scheduler.  Tasks are scheduled in priority order,
     * where the highest priority task is first in the array, and the lowest
     * priority task is the last.
     * task - array of task pointers.
     * numTasks - number of tasks in the array.
     */
    TaskScheduler(Task **task, uint8_t numTasks);

    /*
     * Start the task scheduler running.  Never returns.
	 * Changed from run() to runTasks() - KG 3-20-2019
     */
    void runTasks();

private:
    Task **tasks;   // Array of task pointers.
    int numTasks;   // Number of tasks in the array.
};

#endif
