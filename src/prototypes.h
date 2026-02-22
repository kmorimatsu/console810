
extern volatile unsigned char vram[768];
extern const unsigned char cgrom[];

void ntsc_init(void);
volatile int g_videoline;

inline void check_ps2(void);
inline void completed_ps2(void);
inline void allow_ps2(void);
inline void disallow_ps2(void);

void uart_init(void);
inline void check_rxdata(void);
inline void UARTSend(unsigned char code);
inline void check_txdata(void);

inline void blink_cursor(void);
inline void print_char(unsigned char ascii);
