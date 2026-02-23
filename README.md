# console810
A simple console with LPC 810

## Schematic
![schematic.png](schematic.png)

## About Console 810

Console 810 is a serial console that uses the LPC810, an NXP 8‑pin DIP–package 32‑bit microcontroller.It has the following features:

- A character display capable of showing 32 columns × 24 rows of alphanumeric characters  
- Text output to a monitor via an NTSC video signal  
- Support for connecting a PS/2 keyboard  
- Serial input/output at 9600 bps  
- Space‑saving design using an 8‑pin DIP microcontroller  
- Only a bypass capacitor and five resistors required besides power and connectors  
- Free software licensed under the LGPL

## How to Use

First, transfer the firmware binary contained in **con810_10x.hex** or **con810_10x.axf** to the LPC810.  
I verified operation by transferring the HEX file using Flash Magic.  
Use **con810_101** for a 101‑key keyboard, and **con810_106** for a 106‑key keyboard.

Connect the serial I/O terminals to the device you want to interface with.  
Set the baud rate to **9600 bps**.

Connect a PS/2 keyboard.  
The included binaries support both **101‑key (US)** and **106‑key (Japanese)** keyboards.

Connect a monitor with an NTSC video input.  
In most cases it should work as is, but if the display is unstable, recompile the firmware using the method described below and transfer the rebuilt binary to the LPC810.

## How to compile

The program is compiled using **LPCXpresso ver. 6.1.0**.  
If you encounter issues when compiling with a newer version, try using this version instead.  
If you need a HEX file, add the following command at the beginning of  
**“Property → Settings → Build Steps → Command:”**:

`arm-none-eabi-objcopy -O ihex "${BuildArtifactFileName}" "${BuildArtifactFileBaseName}.hex";`

Edit the **config.h** file as needed before compiling.  
To use a Japanese 106‑key keyboard, comment out `#define KEYBOARD101`  
and uncomment `#define KEYBOARD106`.

To adjust the monitor display, modify the value in  
`#define vsynctune 0` within the range **–4 to +4**.  
If you have a frequency counter, measure the signal output from pin 3  
and adjust the value so that it gets as close as possible to **16275 Hz**.

You can probably change the serial communication baud rate by modifying  
`#define BAUDRATE 9600`, but this has not been tested.
