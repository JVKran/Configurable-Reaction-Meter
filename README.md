# Reaction meter
This repository features three possible implementations of a response time meter (also known as reaction meter). It has been made for a course of the Embedded Systems minor at the Hogeschool Rotterdam in which we learn how to use configurable SoC's. All learned methods have to be compared and shown to prove the provided knowledge has been understood. As a result, the three implementations are RTOS, Qsys and Linux based.

## Usage
On first boot, there are no tries and highscores yet. Hence, all digits default to ```0```. One must registers oneself after every three turns. This is done by entering your name. One can then try the meter out by pressing ```KEY_0``` of the DE1-SOC. There will then be random delay after which the seven segment displays will start counting and the leds connected to the ```LEDR``` port will start lighting up. The user then needs to press ```KEY_1``` as fast as possible.

When a new highscore has been set, the leds will start circling. Otherwise, nothings happens until the user lets go of ```KEY_1```; the user is brought back to the 'home-screen'. Here, the user is presented with the amount of tries on the two leftmost displays and the highest score on the rightmost four displays.

### Cheat-prevention
Responding times faster than a human is able to respond (80ms) are invalid and ignored. Furthermore, the button has to be unpressed for the counting to start. When this isn't the case, the random delay will start over again (and continues to do so if neccesary).

## Platforms

### RTOS
An interval timer component is used to provide the ticks for the UC/OS-II RTOS. This timer is able to provide ticks between 1us and several seconds, but the RTOS requires it to be configured at 1ms. Hence, we can't reach a measurement accuracy of more than 1ms. This can, of course, be solved by adding an extra interval timer that's not used by the RTOS, but this would invalidate our initiative to provide a Qsys implementation. The NIOS-II based embedded system, as well as the required code, can be found in the [RTOS-folder](./rtos).

The [application](./rtos/software/rtos_based/rtos_main.cpp) for this platform uses the NIOS-II softcore in combination with the [seven segment controller](components/seven_segment_controller) component to show the response time on the seven segment displays.

### Qsys
A custom platform designer (formerly known as Qsys) component has been made to perform the response time measurement with its corresponding edge-cases. The measured response time can, as well as the state of the response meter, be read from a register. The response time meter fires interrupts whenever a state-transition occurs. Depending on the state, it might be a good idea to read the response time register. As a result of this setup, the application doesn't directly interface with buttons.

The [application](./qsys/software/qsys_based/qsys_main.c) for this platform uses the NIOS-II softcore in combination with the [seven segment controller](components/seven_segment_controller) and [response time meter](components/reaction_meter) components to perform response time measuremens and show them on displays.

### Linux
The linux based implementation uses the same components as the Qsys based implementation. These components are, however, implemented in the form of kernel modules. These kernel modules can be used in user space applications to easily interface with the outside world and create a response time measuring application. The use of Linux also opens up capabilities that are outside of the scope for most microcontrollers like making POST requests to a remote server.

The [application](./hps/software/app/main.c) for this platform uses the Hardcore Processing System in combination with the [seven segment controller](components/seven_segment_controller) and [response time meter](components/reaction_meter) components in the form of kernel modules to easily create complex applications.
