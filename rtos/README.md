# US/OS-II Based
This folder implements a response time meter that's completely based upon an RTOS that's running on a custom NIOS-II softcore.

## Usage
On first boot, there are no tries and highscores yet. Hence, all digits default to ```0```. One can try the meter out by pressing ```KEY_0``` of the DE1-SOC. There will then be random delay after which the seven segment displays will start counting and the leds connected to the ```LEDR``` port will start lighting up. The user then needs to press ```KEY_1``` as fast as possible.

When a new highscore has been set, the leds will start circling. Otherwise, nothings happens until the user lets go of ```KEY_1```; the user is brought back to the 'home-screen'. Here, the user is presented with the amount of tries on the two leftmost displays and the highest score on the rightmost four displays.

## Cheat-prevention
Responding times faster than a human is able to respond (80ms) are invalid and ignored. Furthermore, the button has to be unpressed for the counting to start. When this isn't the case, the random delay will start over again (and continues to do so if neccesary).