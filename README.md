# mspa-wifi

This project aims to add WiFi support to my M-Spa Aurora Urban U-AU062.

# Initial findings

## Hardware

The control board inside the control box:
![image](./images/control-box-pcb.jpg)

The microcontroller is a PIC16F723A:

![image](./images/control-box-pcb-pic16f723a.jpg)

The connector to the remote control can be seen in the upper right corner. The pinout:

| Color | Description                        |
|-------|------------------------------------|
| White | GND                                |
| Green | Rx (remote control to control box) |
| Black | Tx (control box to remote control) |
| Red   | +5v                                |
