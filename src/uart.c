/*
   This program is provided under the LGPL license ver 2.1.
   Written by Katsumi.
   http://hp.vector.co.jp/authors/VA016157/
   kmorimatsu@users.sourceforge.jp
*/

#include "LPC8xx.h"
#include "./prototypes.h"

#define TXBUFF_SIZE 16
#define BAUDRATE 9600
#define SYSCLK 24000000
#define DATA_LENG_8 (1<<2)
#define PARITY_NONE (0<<4)
#define STOP_BIT_1 (0<<6)
#define CTS_DELTA (1<<5)
#define DELTA_RXBRK (1<<11)
#define TXRDY (1<<2)
#define RXRDY (1<<0)
#define UART_BRG (SYSCLK/16/BAUDRATE-1)
#define UART_FRGDIV (0xff)

void uart_init(){
	// Clock control
	LPC_SYSCON->UARTCLKDIV = 1;           // divided by 1
	NVIC_DisableIRQ(UART0_IRQn);          // Do not use interrupt
	LPC_SYSCON->SYSAHBCLKCTRL |= (1<<14); // Enable UART clock
	LPC_SYSCON->PRESETCTRL &= ~(0x1<<3);  // Reset UART
	LPC_SYSCON->PRESETCTRL |= (0x1<<3);   // Clear reset
	// Initialize UART
	LPC_USART0->CFG = DATA_LENG_8|PARITY_NONE|STOP_BIT_1; // 8 bits, no Parity, 1 Stop bit
	LPC_USART0->BRG = UART_BRG;
	LPC_SYSCON->UARTFRGDIV = UART_FRGDIV;
	LPC_SYSCON->UARTFRGMULT = (((SYSCLK / 16) * (UART_FRGDIV + 1)) / (BAUDRATE * (UART_BRG + 1))) - (UART_FRGDIV + 1);
	LPC_USART0->STAT = CTS_DELTA | DELTA_RXBRK;		// Clear all status bits.
	LPC_USART0->CFG |= (1<<0);
}

unsigned char g_rxbuff[TXBUFF_SIZE];
int g_rxbuff_read=0;
int g_rxbuff_write=0;

inline void send_txdata(unsigned char code){
	g_rxbuff[g_rxbuff_write]=code;
	g_rxbuff_write++;
	g_rxbuff_write &= TXBUFF_SIZE-1;
}

inline void check_txdata(void){
	if (g_rxbuff_read!=g_rxbuff_write) {
		if (LPC_USART0->STAT & TXRDY) {
			LPC_USART0->TXDATA = g_rxbuff[g_rxbuff_read];
			g_rxbuff_read++;
			g_rxbuff_read &= TXBUFF_SIZE-1;
		}
	}
}

inline void UARTSend(unsigned char code){
	// Convert ascii code to telnet code.
	switch(code){
	case 0x0a: // LF to CRLF conversion.
		send_txdata(0x0d);
		break;
	}
	send_txdata(code);
}

inline void check_rxdata(){
	if (LPC_USART0->STAT & RXRDY) {
		unsigned char rxdata=LPC_USART0->RXDATA;
		// TODO: conversion from telnet code to ascii code.
		print_char(rxdata);
	}
}
