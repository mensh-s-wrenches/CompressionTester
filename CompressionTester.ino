// Copyright (C) 2019 Mensh's Wrenches & OTRONICS.IO

// Includes
#include <Adafruit_ADS1015.h>
#include <Wire.h>

// Constants
#define PIN_LED         17
#define VERSION         "Compression Tester V0"
#define MV_PER_BIT      0.1875
#define V_PER_BIT       0.0001875
#define RING_SIZE       8
#define PRESSURE_OFFSET 2.10

// Macros
#define sprint(msg)   Serial.print(msg)
#define sprint_f(msg)   Serial.print(F(msg))

#define sprintln(msg) Serial.println(msg)
#define sprintln_f(msg) Serial.println(F(msg))

// Globals
Adafruit_ADS1115 ads;  /* Use this for the 16-bit version */

int16_t ring[RING_SIZE];

void setup() {
  // put your setup code here, to run once:
  pinMode(PIN_LED, OUTPUT);
  for(int i = 0; i < 5; i++) {
    digitalWrite(PIN_LED, !digitalRead(PIN_LED));
    delay(100);
  }
  
  Serial.begin(9600);
  ads.begin();
  
  // The ADC input range (or gain) can be changed via the following
  // functions, but be careful never to exceed VDD +0.3V max, or to
  // exceed the upper and lower limits if you adjust the input range!
  // Setting these values incorrectly may destroy your ADC!
  //                                                                ADS1015  ADS1115
  //                                                                -------  -------
  // ads.setGain(GAIN_TWOTHIRDS);  // 2/3x gain +/- 6.144V  1 bit = 3mV      0.1875mV (default)
  // ads.setGain(GAIN_ONE);        // 1x gain   +/- 4.096V  1 bit = 2mV      0.125mV
  // ads.setGain(GAIN_TWO);        // 2x gain   +/- 2.048V  1 bit = 1mV      0.0625mV
  // ads.setGain(GAIN_FOUR);       // 4x gain   +/- 1.024V  1 bit = 0.5mV    0.03125mV
  // ads.setGain(GAIN_EIGHT);      // 8x gain   +/- 0.512V  1 bit = 0.25mV   0.015625mV
  // ads.setGain(GAIN_SIXTEEN);    // 16x gain  +/- 0.256V  1 bit = 0.125mV  0.0078125mV
  
  ads.setGain(GAIN_TWOTHIRDS);
  sprintln_f(VERSION);
  sprintln_f("Getting single-ended readings from AIN0");
  sprintln_f("ADC Range: +/- 6.144V (0.1875mV/ADS1115)");
}

void loop() {
  // put your main code here, to run repeatedly:
  int16_t adc0;

  // adc0 = ads.readADC_SingleEnded(0);
  addRingSample(ads.readADC_SingleEnded(0));

  // sprint_f("Raw:\t"); sprint(adc0); sprint_f("\tVolts:\t"); Serial.println(floatVoltage(adc0), 6); // sprintln(floatVoltage(adc0));
  // Serial.println(floatVoltage(adc0), 6);
  
  Serial.println(convertToPressure(floatVoltage(getRingAverage()), 200, PRESSURE_OFFSET));
  // Serial.println(convertToPressure(floatVoltage(adc0), 300, 2.05));
  
  // delay(25);
}

void addRingSample(int16_t sampleValue) {
  static int spot = 0;

  ring[spot++] = sampleValue;

  // Serial.println("------Buffer:");
  // for(int i = 0; i < RING_SIZE; i++) {
  //   Serial.println(ring[i]);
  // }

  // Serial.println("------End Buffer");

  if(spot >= RING_SIZE) spot = 0;
}

int16_t getRingAverage() {
  uint32_t ringSum = 0;
  for(int i = 0; i < RING_SIZE; i++) {
    ringSum += ring[i];
  }

  return (int16_t) (ringSum / (uint32_t) RING_SIZE);
}

inline float floatVoltage(int16_t adc) {
  return ((float)adc) * V_PER_BIT;
}

float convertToPressure(float voltage, float maxValue, float offset) {
  float tmp = voltage;
  tmp -= 0.5;
  tmp *= (maxValue / 4);
  tmp += offset;
  return tmp ;
}
