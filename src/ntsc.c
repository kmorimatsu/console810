/*
   This program is provided under the LGPL license ver 2.1.
   Written by Katsumi.
   http://hp.vector.co.jp/authors/VA016157/
   kmorimatsu@users.sourceforge.jp
*/

#include "LPC8xx.h"

#include "./sct_fsm.h"

#include "./prototypes.h"
#include "./config.h"

#define vsync1523 (1523+(vsynctune)*4)
#define vsync54 (54)
#define vsync761 (761+(vsynctune)*2)
#define vsync816 (816+(vsynctune)*2)
#define vsync648 (648+(vsynctune)*2)
#define vsync1410 (1410+(vsynctune)*4)
#define vsync112 (112)

volatile int g_videoline=0;


/* Video sync signal construction
 * 1 line: 1524 (+/- 16) clocks (15748 Hz; 63.5 usec)
 *
 * line #0-#2
 *     ________    ________
 * |__|        |__|
 * 2.3 usec    2.3 usec
 *    |        |  |
 *    55     761  816
 *
 *
 * line #3-#5
 *          ___         ___
 * |_______|   |_______|
 *          4.7 usec    4.7 usec
 *         |   |       |
 *        648 761     1410
 *
 * line #6-#8
 *     ________    ________
 * |__|        |__|
 * 2.3 usec    2.3 usec
 *    |        |  |
 *    55     761  816
 *
 * line #9-#261
 *      ___________________
 * |___|
 * 4.7 usec
 *     |
 *    112
 *
 * This must produces 16275 Hz sync signal.
 *
 */

void ntsc_init(void) {
	// enable the SCT and SPI clocks
	LPC_SYSCON->SYSAHBCLKCTRL |= (1 << 8) | (1<<11);

    // Initialize video sync routines using SCT
	sct_fsm_init ();
	LPC_SCT->MATCH_L[1]    = vsync54;
	LPC_SCT->MATCHREL_L[1] = vsync54;
	LPC_SCT->MATCH_L[2]    = vsync761;
	LPC_SCT->MATCHREL_L[2] = vsync761;
	LPC_SCT->MATCH_L[3]    = vsync1410;
	LPC_SCT->MATCHREL_L[3] = vsync1410;
	LPC_SCT->MATCH_L[0]    = vsync1410+1;
	LPC_SCT->MATCHREL_L[0] = vsync1410+1;
	LPC_SCT->MATCH_H[0]    = vsync1523;
	LPC_SCT->MATCHREL_H[0] = vsync1523;

	// Enable SCT interrupt
	NVIC_EnableIRQ(SCT_IRQn);

	// unhalt the SCT by clearing bit 2 and bit 18 of the unified CTRL register
	LPC_SCT->CTRL_U &= ~((1<<2) | (1<<18));

	// SPI initialization
	LPC_SPI0->DIV = 4-1;											//SPI bitrate = system clock/4 (6 MHz)
	LPC_SPI0->DLY = 0;												//no added delays
	LPC_SPI0->INTENCLR = 0x3F;										//disable all interrupts
	LPC_SPI0->TXCTRL = (16-1)<<24 | 1<<22; 							//16 bit frame,ignore RX data
	LPC_SPI0->CFG = 0<<8 | 0<<7 | 0<<5 | 0<<4 | 0<<3 | 1<<2 | 1<<0;	//SSEL=0,no loop,CPOL=0,CPHA=0,MSB,master,enable
	//NVIC_EnableIRQ(SPI0_IRQn);                                    //Don't use SPI0 interrupts

	// Initialize VRAM
	int i;
	for (i=0;i<768;i++){
		//vram[i]=i%0x60;
		vram[i]=0;
	}

}

void SCT_IRQHandler (void) {
	uint32_t status = LPC_SCT->EVFLAG;

	// Control sync signal timing.
	switch (g_videoline) {
	case 0:
		LPC_SCT->MATCH_L[1]    = vsync54;
		LPC_SCT->MATCHREL_L[1] = vsync54;
		LPC_SCT->MATCH_L[0]    = vsync1410+1;
		LPC_SCT->MATCHREL_L[0] = vsync1410+1;
		break;
	case 3:
		LPC_SCT->MATCH_L[1]    = vsync648;
		LPC_SCT->MATCHREL_L[1] = vsync648;
		LPC_SCT->MATCH_L[3]    = vsync1410;
		LPC_SCT->MATCHREL_L[3] = vsync1410;
		break;
	case 6:
		LPC_SCT->MATCH_L[1]    = vsync54;
		LPC_SCT->MATCHREL_L[1] = vsync54;
		LPC_SCT->MATCH_L[3]    = vsync816;
		LPC_SCT->MATCHREL_L[3] = vsync816;
		break;
	case 9:
		LPC_SCT->MATCH_L[1]    = vsync112;
		LPC_SCT->MATCHREL_L[1] = vsync112;
		LPC_SCT->MATCH_L[0]    = vsync112+80; // Can be between 112 and 761
		LPC_SCT->MATCHREL_L[0] = vsync112+80; // Can be tuned for the timing of video signal
		break;
	case 10:
		// Initialize video interrupt routine here if something is required.
		// Video signal will start from line #45
		break;
	}

	// Video signal construction.
	if (45<=g_videoline && g_videoline<=236) {
		int videoline=g_videoline-45;
		volatile unsigned char* vrampos=vram+((videoline>>3)<<5);
		const unsigned char* cgromline=cgrom+((videoline%8)*0x60);
		int i;
		for(i=0;i<16;i++){
			LPC_SPI0->TXDAT = ((cgromline[vrampos[0]])<<8) | cgromline[vrampos[1]];
			vrampos+=2;
			// Wait until buffer will be available.
			while (!(LPC_SPI0->STAT&2)){}
		}
		// Send blank data after last character.
		LPC_SPI0->TXDAT =0;
	}

	// Increment line number and exit function.
	g_videoline++;
	if (262<=g_videoline) g_videoline=0;
	/* Acknowledge interrupts */
	LPC_SCT->EVFLAG = status;
}

