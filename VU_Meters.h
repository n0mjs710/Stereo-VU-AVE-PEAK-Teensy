/*
AVERAGE AND PEAK READING STEREO VU METER FOR THE TEENSY 3.1
Created by Cort Buffington & Keith Neufeld
March 2015

This work is licensed under the Creative Commons Attribution-ShareAlike
3.0 Unported License.To view a copy of this license, visit
http://creativecommons.org/licenses/by-sa/3.0/ or send a letter to
Creative Commons, 444 Castro Street, Suite 900, Mountain View,
California, 94041, USA.
*/

#define LEFT A9
#define RIGHT A3

// AVERAGE LOW-PASS FILTER (INTEGRATOR) CONSTANT
#define AVE_LPF_CONST (400)

// PEAK DECAY LOW-PASS FILTER (INTEGRATOR) CONSTANT
#define DECAY_LPF_CONST (300)

// DISPLAY MACROS (THEY JUST DON'T WARRANT FUNCTIONS)
// CREATE A DOT AT THE POSITION INDICATED
#define pos_to_dotmap(p) ( (p) == 0 ? 0 : (uint32_t) 0b1 << (p) )
// CREATE A BAR AT THE POSITION INDICATED
#define pos_to_barmap(p) ( (uint32_t) 0b11111111111111111111111111111111 >> (32 - (p)) )


// ADC TRESHOLDS FOR THE LED THAT SHOULD BE ON
//
// EVENLY DIVIDED LINEAR DISPLAY
//#define LINEARDISPLAY
//
// 1 DB PER PIXEL LOG DISPLAY
#define DB1DISPLAY

#ifdef LINEARDISPLAY
#define THRESH01 (16)
#define THRESH02 (48)
#define THRESH03 (80)
#define THRESH04 (112)
#define THRESH05 (144)
#define THRESH06 (176)
#define THRESH07 (208)
#define THRESH08 (240)
#define THRESH09 (272)
#define THRESH10 (304)
#define THRESH11 (336)
#define THRESH12 (368)
#define THRESH13 (400)
#define THRESH14 (432)
#define THRESH15 (464)
#define THRESH16 (496)
#define THRESH17 (528)
#define THRESH18 (560)
#define THRESH19 (592)
#define THRESH20 (624)
#define THRESH21 (656)
#define THRESH22 (688)
#define THRESH23 (720)
#define THRESH24 (752)
#define THRESH25 (784)
#define THRESH26 (816)
#define THRESH27 (848)
#define THRESH28 (880)
#define THRESH29 (912)
#define THRESH30 (944)
#define THRESH31 (976)
#define THRESH32 (1008)
#endif // LINEARDISPLAY


#ifdef DB1DISPLAY
#define THRESH01 (27)	  // Segment# 1, -17dB	(Actual Value:28.8)
#define THRESH02 (30)	  // Segment# 2, -16dB	(Actual Value:32.4)
#define THRESH03 (34)	  // Segment# 3, -15dB	(Actual Value:36.3)
#define THRESH04 (38)	  // Segment# 4, -14dB	(Actual Value:40.7)
#define THRESH05 (42)	  // Segment# 5, -13dB	(Actual Value:45.7)
#define THRESH06 (47)	  // Segment# 6, -12dB	(Actual Value:51.3)
#define THRESH07 (53)	  // Segment# 7, -11dB	(Actual Value:57.5)
#define THRESH08 (60)	  // Segment# 8, -10dB	(Actual Value:64.5)
#define THRESH09 (67)	  // Segment# 9, -9dB	(Actual Value:72.4)
#define THRESH10 (75)	  // Segment# 10, -8dB	(Actual Value:81.3)
#define THRESH11 (84)	  // Segment# 11, -7dB	(Actual Value:91.2)
#define THRESH12 (95)   // Segment# 12, -6dB	(Actual Value:102.3)
#define THRESH13 (106)	// Segment# 13, -5dB	(Actual Value:114.8)
#define THRESH14 (119)	// Segment# 14, -4dB	(Actual Value:128.8)
#define THRESH15 (134)	// Segment# 15, -3dB	(Actual Value:144.5)
#define THRESH16 (150)	// Segment# 16, -2dB	(Actual Value:162.1)
#define THRESH17 (168)	// Segment# 17, -1dB	(Actual Value:181.9)
#define THRESH18 (189)	// Segment# 18, 0dB	(Actual Value:204.1)
#define THRESH19 (212)	// Segment# 19, 1dB	(Actual Value:229.0)
#define THRESH20 (238)	// Segment# 20, 2dB	(Actual Value:257.0)
#define THRESH21 (267)	// Segment# 21, 3dB	(Actual Value:288.3)
#define THRESH22 (300)	// Segment# 22, 4dB	(Actual Value:323.5)
#define THRESH23 (336)	// Segment# 23, 5dB	(Actual Value:363.0)
#define THRESH24 (377)	// Segment# 24, 6dB	(Actual Value:407.3)
#define THRESH25 (423)	// Segment# 25, 7dB	(Actual Value:457.0)
#define THRESH26 (475)	// Segment# 26, 8dB	(Actual Value:512.7)
#define THRESH27 (533)	// Segment# 27, 9dB	(Actual Value:575.3)
#define THRESH28 (598)	// Segment# 28, 10dB	(Actual Value:645.5)
#define THRESH29 (671)	// Segment# 29, 11dB	(Actual Value:724.2)
#define THRESH30 (753)	// Segment# 30, 12dB	(Actual Value:812.6)
#define THRESH31 (844)	// Segment# 31, 13dB	(Actual Value:911.7)
#define THRESH32 (947)	// Segment# 32, 14dB	(Actual Value:1023.0)
#endif // DB1DISPLAY


// FINDS THE METER POSITION FROM THE ADC VALUE WITH THE SAME
// AND FEWEST NUMBER OF DECISIONS POSSIBLE, WHICH MEANS THAT
// THERE IS NO VARABLE DELAY BASED ON POSITION
#define adc_to_position(a) (                  \
  ((a) < int(THRESH16))                       \
    ? ((a) < int(THRESH08))                   \
      ? ((a) < int(THRESH04))                 \
	? ((a) < int(THRESH02))               \
	  ? ((a) < int(THRESH01)) ? 0 : 1     \
	  : ((a) < int(THRESH03)) ? 2 : 3     \
	: ((a) < int(THRESH06))               \
	  ? ((a) < int(THRESH05)) ? 4 : 5     \
	  : ((a) < int(THRESH07)) ? 6 : 7     \
      : ((a) < int(THRESH12))                 \
	? ((a) < int(THRESH10))               \
	  ? ((a) < int(THRESH09)) ? 8 : 9     \
	  : ((a) < int(THRESH11)) ? 10 : 11   \
	: ((a) < int(THRESH14))               \
	  ? ((a) < int(THRESH13)) ? 12 : 13   \
	  : ((a) < int(THRESH15)) ? 14 : 15   \
    : ((a) < int(THRESH24))                   \
      ? ((a) < int(THRESH20))                 \
	? ((a) < int(THRESH18))               \
	  ? ((a) < int(THRESH17)) ? 16 : 17   \
	  : ((a) < int(THRESH19)) ? 18 : 19   \
	: ((a) < int(THRESH22))               \
	  ? ((a) < int(THRESH21)) ? 20 : 21   \
	  : ((a) < int(THRESH23)) ? 22 : 23   \
      : ((a) < int(THRESH28))                 \
	? ((a) < int(THRESH26))               \
	  ? ((a) < int(THRESH25)) ? 24 : 25   \
	  : ((a) < int(THRESH27)) ? 26 : 27   \
	: ((a) < int(THRESH30))               \
	  ? ((a) < int(THRESH29)) ? 28 : 29   \
	  : ((a) < int(THRESH31))             \
	    ? 30                              \
	    : ((a) < int(THRESH32)) ? 31 : 32 \
)
