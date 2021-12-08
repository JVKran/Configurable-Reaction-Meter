The RTOS can't reach an accuracy higher than 1ms as this would increase the scheduler overhead to such an extent that it wouldn't be able to do any work anymore.

Using another interval timer would of course enable us to measure with an accuracy of 1us, but this would remove the neccesity of the assignment to perform measurements.