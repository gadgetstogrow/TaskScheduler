/*
 * Copyright 2011 Alan Burlison, alan@bleaklow.com.  All rights reserved.
 * Use is subject to license terms.
 */

#include "Task.h"

// Virtual.
bool TriggeredTask::canRun(uint32_t now) {
    return runFlag;
}

// Virtual.
bool TimedTask::canRun(uint32_t now) {
    /*
     * If millis() and runTime have not rolled over
     * then return now >= runTime. (As previously.)
     */

    /*
     * Has anything rolled over?
     */
    bool millisRollOver = (now < lastRunTime);
    bool runTimeRollover = (runTime < lastRunTime);

    /* 
     *  Assume no rollovers, most common scenario. If
     *  we are about to run a task, log its last run time.
     *  If both rolled over, we stay here too.
     */    
    if ((!millisRollOver && !runTimeRollover) ||
        (millisRollOver && runTimeRollover)) {
        if (now >= runTime) {
            lastRunTime = now;
            return true;
        }
    }

    // If runTime rolled over then we must wait for millis
    // to roll over as well. So as long as `millis()` is > than
    // next run, we return false;
    if (runTimeRollover) {
        if (now > runTime) {
            return false;
        }

        // Otherwise log the last run time
        // and return true.
        lastRunTime = now;
        return true;
    }

    // Anything else is false. `millis()` can't rollover and
    // runTime not. Only the other way around as dealt with
    // above.
    return false;    
}
