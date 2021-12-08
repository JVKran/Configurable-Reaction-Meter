# Qsys
This folder implements a response time meter that's completely based upon a Platform Desinger component that's controlled by an RTOS running on a custom NIOS-II softcore.

## Structure
In contrary to the RTOS-based implementation, the state-machine isn't entirely implemented in code; the hard-realtime parts (consisting of the measuring and detection of response key press) are implemented as a Platform Designer component. This boils down to there being two state-machines; one in VHDL and one in C++. The state-machine in C++ takes care of high-level application logic such as controlling leds when an interrupt occurs and reading the response time when the user pressed the button.