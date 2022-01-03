# Linux
This folder contains everything that's needed to run Linux on a custom platform. From the raw binary file, to the platform designer components and software.

## Structure
Make sure the raw binary file is placed on the SD-card at the corresponding partition in the root folder. Then, set the MSEL switches in the right position according to the datasheet. Last but not least, connect an ethernet cable and power it up. Connect to the vscode server and execute ```sudo ./start.sh``` to install all kernel modules. Then run ```./app/main``` to run the application which connects to ```https://thingsboard.jvkran.com```. 

The main contains the application with it's in- and output while the kernel modules take care of low-level hardware control and communication.