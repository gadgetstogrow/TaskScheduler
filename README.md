# TaskScheduler

## Introduction

As my Arduino projects became more complex I started to realize the delay() function caused unforeseen problems. While in a delay, the Arduino/AVR can't process any other code (with a few exceptions). I came across this solution while researching another topic and I thought I would document it for my future projects. Alan Burlison created an eloquent Task Scheduler library that solves the delay issues ---but more importantly, it made me rethink how I design my Arduino/AVR projects. The original code was written some years back and still referenced obsolete things like WProgram.h. I have made some small changes and it is available on my GitHub repository along with Alan's original code. There are other Task Scheduler libraries available, but this one is extremely compact, straight forward, and does everything I need. The library uses pointers (eek!), but don't despair, if I can grasp how to use them, so can you. If you're like me, all of my Arduino projects seem to end up being "spaghetti" code by the time I'm finished. Using this library will enable you to structure your code more efficiently to make it more maintainable and easier to debug.

## Links
For more information checkout my Hackster.io page: 
https://www.hackster.io/GadgetsToGrow/

The Original Task Scheduler code by Alan Burlison can be found here: http://bleaklow.com/2010/07/20/a_very_simple_arduino_task_manager.html
