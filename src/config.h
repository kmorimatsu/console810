/*
   This program is provided under the LGPL license ver 2.1.
   Written by Katsumi.
   http://hp.vector.co.jp/authors/VA016157/
   kmorimatsu@users.sourceforge.jp
*/

/*
 * 'vsynctune' is set to get proper video signal construction frequency.
 * -4 <= vsynctune <= 4
 * After setting proper value, signal frequency of Pin #3 will be 16275 Hz.
 *
 */
#define vsynctune 0

/*
 * 101 and 106 keyboard are supported.
 * Define either KEYBOARD101 or KEYBOARD106
 *
 */

#define KEYBOARD101
//#define KEYBOARD106

/*
 * Baud rate definition
 * (experimental and not tested)
 *
 */

#define BAUDRATE 9600
