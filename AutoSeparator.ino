/**
  AutoSeparator.ino
  
  Martin (Tsz) Lui, James H. Bannock, Simon T. Turner and John C. de Mello
  Imperial College London and Norges teknisk-naturvitenskapelige universitet
  Licence GNU 3.0

  Arduino code for Automated Liquid-Liquid Separator as described in:
  "Automated separation of immiscible liquids using an optically monitored porous capillary"
  in Reaction Chemistry and Engineering (Royal Society of Chemistry)
*/

#include <Arduino.h>
#include "ADS1115.h"

#define EASYDRIVER_STEP_PIN 8
#define EASYDRIVER_DIR_PIN 9
#define MOVING_LENGTH 10

float moving_throughchannel[MOVING_LENGTH];
float moving_sidechannel[MOVING_LENGTH];

float mean_throughchannel;
float mean_sidechannel;

float sum_throughchannel, sum_sidechannel;

ADS1115 adc(ADDRESS_GND);

void setup() {
  // Setup Serial port
  Serial.begin(250000); (!Serial);

  // Setup ADS1115
  adc.setSPS(SPS_860); // 860 samples/s
  adc.setMode(SINGLE); // single measurement
  adc.setMUX(AIN0_GND); // Single-ended measurement on Input 0
  adc.setPGA(GAIN_TWO); // max voltage 2.048 V

  // Setup Easydriver pins
  pinMode(EASYDRIVER_STEP_PIN, OUTPUT);
  pinMode(EASYDRIVER_DIR_PIN, OUTPUT);

  // Preload the moving average arrays
  for (uint8_t i = 0; i< MOVING_LENGTH; i++) {
    adc.setMUX(AIN0_GND); moving_throughchannel[i] = adc.recordSingleMeasurement();
    adc.setMUX(AIN1_GND); moving_sidechannel[i] = adc.recordSingleMeasurement();
  }
}

void serialEvent() {
  switch (Serial.read()) {
    case 'E': // print average eluent channel voltage
      Serial.println(mean_throughchannel, 8); // 8 s.f.
      break;

    case 'D': // print average depletant channel voltage
      Serial.println(mean_sidechannel, 8); // 8 s.f.
      break;

    case 'C': // turn the valve clockwise
      turncw(Serial.parseInt());
      break;

    case 'A': // turn the valve anti-clockwise
      turnatw(Serial.parseInt());
      break;
  }
}

void loop() {
  for (uint8_t i = 1; i < MOVING_LENGTH; i++) {
    moving_throughchannel[i] = moving_throughchannel[i-1];
    moving_sidechannel[i] = moving_sidechannel[i-1];
  }

  adc.setMUX(AIN0_GND); moving_throughchannel[0] = adc.recordSingleMeasurement();
  adc.setMUX(AIN1_GND); moving_sidechannel[0] = adc.recordSingleMeasurement();

  sum_throughchannel = 0; sum_sidechannel = 0;

  for (uint8_t i = 0; i < MOVING_LENGTH; i++) {
    sum_throughchannel += moving_throughchannel[i];
    sum_sidechannel += moving_sidechannel[i];
  }

  mean_throughchannel = sum_throughchannel / MOVING_LENGTH;
  mean_sidechannel = sum_sidechannel / MOVING_LENGTH;
}

void turncw(uint16_t steps) {
  rotate(steps, 0, 1); // dir = 0, speed = 1 rps
}
void turnatw(uint16_t steps) {
  rotate(steps, 1, 1);  // dir = 1, speed = 1 rps
}

void rotate(uint16_t steps, uint8_t dir, float speed) {

  int stepsPerRevolution = 400; // 360 / 0.9 -> 400

  digitalWrite(EASYDRIVER_DIR_PIN, dir);

  float usDelay = (500000 / stepsPerRevolution) / speed; // rps/half-step

  for (uint16_t i = 0; i < steps; i++) {
    digitalWrite(EASYDRIVER_STEP_PIN, HIGH); delayMicroseconds(usDelay);
    digitalWrite(EASYDRIVER_STEP_PIN, LOW); delayMicroseconds(usDelay);
  }
}
