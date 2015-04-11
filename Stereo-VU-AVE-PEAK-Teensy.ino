/*
AVERAGE AND PEAK READING STEREO VU METER FOR THE TEENSY 3.1
Created by Cort Buffington & Keith Neufeld
March 2015

This program uses a mathematica implementation of a low-pass filter (integrator) for both
the average value display, and to generate decay for the peak display. The formula is:

value += (new value - value)/FILTER CONSTANT

MAX72 class for the MAX7221/MAX7219 display drivers available at:
https://github.com/n0mjs710/MAX7219

This work is licensed under the Creative Commons Attribution-ShareAlike
3.0 Unported License.To view a copy of this license, visit
http://creativecommons.org/licenses/by-sa/3.0/ or send a letter to
Creative Commons, 444 Castro Street, Suite 900, Mountain View,
California, 94041, USA.
*/

#include <SPI.h>
#include <MAX72.h>
#include <ADC.h>
#include "VU_Meters.h"

// TEENSY ADC ADVANCED CONTROLS
// ALLOWS READING BOTH ADC INPUTS SIMULTANEOUSLY
ADC *adc = new ADC();
ADC::Sync_result adc_value;

// CREATE OBJECT FOR THE MAX7221 DISPLAY DRIVER
MAX72 vu(10, 8);

// VARIABLES NEEDED FOR THE AVERAGE LPF (INTEGRATOR)
float left_average = 0, right_average = 0, left_peak_decay = 0, right_peak_decay = 0;


void setup() {
  adc->setAveraging(1, ADC_0);
  adc->setAveraging(1, ADC_1);
  adc->setResolution(10, ADC_0);
  adc->setResolution(10, ADC_1);
  adc->setConversionSpeed(ADC_HIGH_SPEED, ADC_0);
  adc->setConversionSpeed(ADC_HIGH_SPEED, ADC_1);
  adc->setSamplingSpeed(ADC_HIGH_SPEED, ADC_0);
  adc->setSamplingSpeed(ADC_HIGH_SPEED, ADC_1);
  
  Serial.begin(57600);
  test_display();
}


//#define TIMING

void loop() {
#ifdef TIMING
  long start_time, stop_time;
  float delta;
  start_time = micros();
#endif // TIMING
    
  // READ ADC INPUTS
  adc_value = adc->analogSynchronizedRead(LEFT, RIGHT);
  uint16_t adc_left = adc_value.result_adc0;
  uint16_t adc_right = adc_value.result_adc1; 
  
  // UPDATE AVERAGE VALUES
  // LEFT
  left_average += (adc_left - left_average)/AVE_LPF_CONST;
  // RIGHT
  right_average += (adc_right - right_average)/AVE_LPF_CONST;
  
  // UPDATE PEAK VALUES
  // LEFT
  if (adc_left > left_peak_decay) {
    left_peak_decay = adc_left;
  } else {
    left_peak_decay += (adc_left - left_peak_decay)/DECAY_LPF_CONST;
  }
  // RIGHT
  if (adc_right > right_peak_decay) {
    right_peak_decay = adc_right;
  } else {
    right_peak_decay += (adc_right - right_peak_decay)/DECAY_LPF_CONST;
  }
 
  // BUILD LEFT/RIGHT CHANNEL BITMAPS TO UPDATE THE DISPLAY
  uint32_t left_bitmap = pos_to_barmap(adc_to_position(left_average)) ^ pos_to_dotmap(adc_to_position(left_peak_decay));
  uint32_t right_bitmap = pos_to_barmap(adc_to_position(right_average)) ^ pos_to_dotmap(adc_to_position(right_peak_decay));
 
  // UPDATE THE DISPLAY (THIS IS MAX7221 SPECIFIC)
  for (uint8_t i=0; i<4; i++) {
    vu.setDigit(i, ((uint8_t*) (&left_bitmap))[i]);
    vu.setDigit(i+4, ((uint8_t*) (&right_bitmap))[i]);
  }
  
#ifdef TIMING
  stop_time = micros();
  delta = (stop_time - start_time) / 1000.00;
  Serial.println(delta);
#endif // TIMING
}


void test_display() {
  uint8_t temp = 0;
  for (uint8_t i=0; i<8; i++) {
    temp |= 1 << i;
    vu.setDigit(0, temp);
    vu.setDigit(1, temp);
    vu.setDigit(2, temp);
    vu.setDigit(3, temp);
    vu.setDigit(4, temp);
    vu.setDigit(5, temp);
    vu.setDigit(6, temp);
    vu.setDigit(7, temp);
    delay(50);
  }
  delay(200);
  vu.dispClear();
  delay(200);
}
