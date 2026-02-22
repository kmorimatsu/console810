/*
   This program is provided under the LGPL license ver 2.1.
   Written by Katsumi.
   http://hp.vector.co.jp/authors/VA016157/
   kmorimatsu@users.sourceforge.jp
*/

#include "LPC8xx.h"
#include "./prototypes.h"

/* Configure for 24 MHz internal clock.
 * Copy and paste following lines to system_LPC8xx.c
#define CLOCK_SETUP           1
#define SYSOSCCTRL_Val        0x00000000              // Reset: 0x000
#define WDTOSCCTRL_Val        0x00000000              // Reset: 0x000
#define SYSPLLCTRL_Val        0x00000001              // Reset: 0x000
#define SYSPLLCLKSEL_Val      0x00000000              // Reset: 0x000
#define MAINCLKSEL_Val        0x00000003              // Reset: 0x000
#define SYSAHBCLKDIV_Val      0x00000001              // Reset: 0x001
 */

int main(void) {
    // Force the counter to be placed into memory
    volatile static int i = 0 ;
    // Wait for ~1 second for possible programming (is this required?).
    for(i=0;i<2097152;i++);

	// Switch matrix settings
    /* Enable SWM clock */
    LPC_SYSCON->SYSAHBCLKCTRL |= (1<<7);
    /* U0_TXD at P0.0 */
    /* U0_RXD at p0.4 */
    LPC_SWM->PINASSIGN0 = 0xffff0004;
    /* SPI0_MOSI at P0.2*/
    LPC_SWM->PINASSIGN4 = 0xffffff02;
    /* SCT CTOUT_0 at P0.3*/
    LPC_SWM->PINASSIGN6 = 0x03ffffff;
    /* Pin Assign 1 bit Configuration */
    LPC_SWM->PINENABLE0 = 0xffffffff;
    /* Switch Matrix can be disabled to save current */
	LPC_SYSCON->SYSAHBCLKCTRL &= ~(1<<7);

	// Note; P0.1 (pin #5) and P0.5 (pin #1) are input GPIO with pull-up in default.

	// Initialize NTSC
	ntsc_init();

	// Initialize UART
	uart_init();

	// Main loop
    while(1) {
    	int videoline=g_videoline;
		if (44<=videoline && videoline<=236) {
			// Disable input from PS/2 keyboard when generating video signal.
			// Note that video signal outputs in lines 45-236.
			disallow_ps2();
		} else {
			// Enable input from PS/2 keyboard when not generating video signal.
			allow_ps2();
		}
		switch(videoline & 3){
		case 0:
			// Serial output if PS/2 signal has been completed.
			// This happens every 11 lines in maximum PS/2 keyboard speed.
	    	completed_ps2();
	    	break;
		case 1:
	    	// Check serial output
	    	// This happens every 11 lines in maximum PS/2 keyboard speed.
	    	check_txdata();
	    	break;
		case 2:
			// Check serial input
	    	// This happens every 16 lines in maximum frequency of UART input.
	    	check_rxdata();
	    	break;
		case 3:
	    	// Blink cursor
	    	blink_cursor();
	    	break;
		}
    	// Remaining time is dedicated to receiving PS/2 signal.
    	// PS/2 signal comes every line in maximum PS/2 keyboard speed.
    	// Therefore, to receive correct data just after falling clock signal from PS/2 keyboard,
    	// the PS/2 signal must be checked more than twice in a line, i.e. before changing data signal.
		while(LPC_SCT->COUNT_H < 750){
			// Check PS/2 signal until the half of line
			check_ps2();
		}
		while(750<=LPC_SCT->COUNT_H){
			// Check PS/2 signal until the end of line
			check_ps2();
		}
    }
    return 0 ;
}

