/**
  ADS1115.cpp

  Dr. James H. Bannock
  Imperial College London and Norges teknisk-naturvitenskapelige universitet
  Licence GNU 3.0

  Library cpp file for ADS1115 communication
*/

#include <Arduino.h>
#include <Wire.h>
#include "ADS1115.h"

/**
  ADS1115 class constructor
*/
ADS1115::ADS1115() {
  ADS1115(ADDRESS_GND); // DEFAULT ADDRESS
}

/**
  ADS1115 class constructor

  @param address Device Address
*/
ADS1115::ADS1115(uint8_t address) {
  _address = address;
  Wire.begin();
}

/**
  Set the multiplexer (MUX) option in the configuration

  @param mux_option Multiplexer configuration option
*/
void ADS1115::setMUX(adsMUX_t mux_option) {
  _mux = mux_option;
}

/**
  Set the programmable gain amplifier (PGA) option in the configuration

  @param pga_option PGA configuration option
*/
void ADS1115::setPGA(adsPGA_t pga_option) {
  _pga = pga_option;

  if(pga_option == GAIN_TWOTHIRDS) _multiplier = MULTIPLIER_PGA_6_144V;
  else if (pga_option == GAIN_ONE) _multiplier = MULTIPLIER_PGA_4_096V;
  else if (pga_option == GAIN_TWO) _multiplier = MULTIPLIER_PGA_2_048V;
  else if (pga_option == GAIN_FOUR) _multiplier = MULTIPLIER_PGA_1_024V;
  else if (pga_option == GAIN_EIGHT) _multiplier = MULTIPLIER_PGA_0_512V;
  else if (pga_option == GAIN_SIXTEEN) _multiplier = MULTIPLIER_PGA_0_256V;
}

void ADS1115::autosetPGA() {

  // SET TO SINGLE MEASUREMENT MODE
  bool flag = LOW;
  if (_mode == CONTINUOUS) {
    setMode(SINGLE);
    flag = HIGH;
  }

  // SET PGA TO MAX SCALE
  setPGA(GAIN_TWOTHIRDS);

  // RECORD VOLTAGE
  float voltage = abs(recordSingleMeasurement());

  // ASSIGN HIGHEST RESOLUTION PGA AVAILABLE
  if (voltage < PGA_LIMIT_0_256V) setPGA(GAIN_SIXTEEN);
  else if (voltage < PGA_LIMIT_0_512V) setPGA(GAIN_EIGHT);
  else if (voltage < PGA_LIMIT_1_024V) setPGA(GAIN_FOUR);
  else if (voltage < PGA_LIMIT_2_048V) setPGA(GAIN_TWO);
  else if (voltage < PGA_LIMIT_4_096V) setPGA(GAIN_ONE);
  else if (voltage < PGA_LIMIT_6_144V) setPGA(GAIN_TWOTHIRDS);
  else setPGA(GAIN_TWOTHIRDS);

  // IF APPROPRIATE, RETURN TO CONTINUOUS MODE
  if (flag) setMode(CONTINUOUS);
}

/**
  Get the programmable gain amplifier (PGA) voltage limit (based on current
  _pga configuration)

  @return voltage limit
*/
float ADS1115::getPGALimit() {
  if (_pga == GAIN_TWOTHIRDS) return PGA_LIMIT_6_144V;
  else if (_pga == GAIN_ONE) return PGA_LIMIT_4_096V;
  else if (_pga == GAIN_TWO) return PGA_LIMIT_2_048V;
  else if (_pga == GAIN_FOUR) return PGA_LIMIT_1_024V;
  else if (_pga == GAIN_EIGHT) return PGA_LIMIT_0_512V;
  else if (_pga == GAIN_SIXTEEN) return PGA_LIMIT_0_256V;
  else return -1;
}

/**
  Set the mode in the configuration

  @param mode_option Acquisition mode
*/
void ADS1115::setMode(adsMode_t mode_option) {
  _mode = mode_option;
  if (_mode == CONTINUOUS) {
    _alarmThresholdRegisters();
    _os = CONFIG_REG_OS_W_NULL;
    _cq = CONFIG_REG_CQ_ASSERT_ONE;
  }
  else if (_mode == SINGLE) {
    _resetThresholdRegisters();
    _os = CONFIG_REG_OS_W_START_SNGL;
    _cq = CONFIG_REG_CQ_DISABLE;
  }
}

/**
  Get the mode stored in the configuration

  @return mode as CONTINUOUS or SINGLE
*/
adsMode_t ADS1115::getMode() {
  if (_mode == CONFIG_REG_MODE_CONT) return CONTINUOUS;
  else return SINGLE;
}

/**
  Set the samples per second (SPS) in the configuration

  @param dr_option Data rate configuration option
*/
void ADS1115::setSPS(adsSPS_t dr_option) {
  _dr = dr_option;
}

/**
  Get the samples per second (SPS) stored in the configuration

  @return integer value of the data rate
*/
uint16_t ADS1115::getSPS() {
  if (_dr == SPS_860) return 860;
  else if (_dr == SPS_475) return 475;
  else if (_dr == SPS_250) return 250;
  else if (_dr == SPS_128) return 128;
  else if (_dr == SPS_64) return 64;
  else if (_dr == SPS_32) return 32;
  else if (_dr == SPS_16) return 16;
  else if (_dr == SPS_8) return 8;
  else return 0;
}

/**
  Record a single-shot measurement on the ADS1115 based on stored configuration

  @return recorded voltage
*/
float ADS1115::recordSingleMeasurement() {
  if (_mode != SINGLE) setMode(SINGLE);
  _sendConfig();
  while(_isConverting());
  return _getVoltage();
}

/**
  Start continuous measurement recording on the ADS1115
*/
void ADS1115::startContinuousMeasurement() {
  if (_mode != CONTINUOUS) setMode(CONTINUOUS);
  _sendConfig();
}

/**
  Read a triggered measurement in continuous mode

  @return recorded voltage
*/
float ADS1115::readTriggeredMeasurement() {
  return _getVoltage();
}

/**
  Read the configuration stored on the ADS1115
*/
void ADS1115::readADS1115Config() {
  uint16_t config = _readRegister(_address, POINTER_REG_CONFIG);

  // MUX

  uint16_t mux = config & CONFIG_REG_MUX_MASK;
  Serial.print(F("MUX: "));
  switch(mux) {
    case AIN0_AIN1:
    Serial.println(F("AIN0_AIN1"));
    break;

    case AIN0_AIN3:
    Serial.println(F("AIN0_AIN3"));
    break;

    case AIN1_AIN3:
    Serial.println(F("AIN1_AIN3"));
    break;

    case AIN2_AIN3:
    Serial.println(F("AIN2_AIN3"));
    break;

    case AIN0_GND:
    Serial.println(F("AIN0_GND"));
    break;

    case AIN1_GND:
    Serial.println(F("AIN1_GND"));
    break;

    case AIN2_GND:
    Serial.println(F("AIN2_GND"));
    break;

    case AIN3_GND:
    Serial.println(F("AIN3_GND"));
    break;
  }

  uint16_t mode = config & CONFIG_REG_MODE_MASK;
  Serial.print(F("MODE: "));
  if (mode == CONTINUOUS) Serial.println(F("CONTINUOUS"));
  else if (mode == SINGLE) Serial.println(F("SINGLE"));

  uint16_t dr = config & CONFIG_REG_DR_MASK;
  Serial.print(F("SPS: "));
  uint16_t sps;
  switch(dr) {
    case SPS_860:
    sps = 860;
    break;

    case SPS_475:
    sps = 475;
    break;

    case SPS_250:
    sps = 250;
    break;

    case SPS_128:
    sps = 128;
    break;

    case SPS_64:
    sps = 64;
    break;

    case SPS_32:
    sps = 32;
    break;

    case SPS_16:
    sps = 16;
    break;

    case SPS_8:
    sps = 8;
    break;
  }
  Serial.println(sps);
}

/**
  I2C write to register on ADS1115

  @param address Device address
  @param reg Register to be overwritten
  @param value 16-bit register value to write
*/
void ADS1115::_writeRegister(uint8_t address, uint8_t reg, uint16_t value) {
  Wire.beginTransmission(address);
  Wire.write((uint8_t)reg); // REGISTER TO OVERWRITE
  Wire.write((uint8_t)(value >> 8)); // MSB
  Wire.write((uint8_t)(value & 0xFF)); // LSB
  Wire.endTransmission();
}

/**
  I2C read register on ADS1115

  @param address Device address
  @param reg Register to read
  @return 16-bit register value
*/
uint16_t ADS1115::_readRegister(uint8_t address, uint8_t reg) {
  // REQUEST REGISTER
  Wire.beginTransmission(address);
  Wire.write((uint8_t)reg); // REGISTER TO READ
  Wire.endTransmission();

  // RECIEVE REGISTER
  Wire.requestFrom(address, (uint8_t)2); // REQUEST MSB/LSB
  return ((Wire.read() << 8) | Wire.read());
}

/**
  Create 16-bit configuration register value using stored configurations

  @return configuration register value
*/
uint16_t ADS1115::_createConfig() {
  return (_os | _mux | _pga | _mode | _dr | _cm | _cp | _cl | _cq);
}

/**
  Send configuration to ADS1115
*/
void ADS1115::_sendConfig() {
  _writeRegister(_address, POINTER_REG_CONFIG, _createConfig());
}

/**
  Determine if ADS1115 is still running measurement

  @return boolean value indicating true/false
*/
bool ADS1115::_isConverting() {
  return !(_readRegister(_address, POINTER_REG_CONFIG) & CONFIG_REG_OS_R_INACTIVE);
}

/**
  Get measured voltage from conversion register (scaled using multiplier)

  @return recorded voltage
*/
float ADS1115::_getVoltage() {
  return (((int16_t)_readRegister(_address, POINTER_REG_CONVERSION) * _multiplier) / 1e6);
}

/**
  Reset threshold registers for single-shot acquisition
*/
void ADS1115::_resetThresholdRegisters() {
  _writeRegister(_address, POINTER_REG_HITHRESH, HITHRESH_RESET);
  _writeRegister(_address, POINTER_REG_LOTHRESH, LOTHRESH_RESET);
}

/**
  Set the threshold registers to issue alert when new measurement ready
*/
void ADS1115::_alarmThresholdRegisters() {
  _writeRegister(_address, POINTER_REG_HITHRESH, HITHRESH_ALRT_TRIGGER);
  _writeRegister(_address, POINTER_REG_LOTHRESH, LOTHRESH_ALRT_TRIGGER);
}
