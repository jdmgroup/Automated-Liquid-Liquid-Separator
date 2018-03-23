/**
  ADS1115.h
  
  Dr. James H. Bannock
  Imperial College London and Norges teknisk-naturvitenskapelige universitet
  Licence GNU 3.0

  Library header file for ADS1115 communication
*/

#include "Arduino.h"
#include "Wire.h"

#ifndef ADS1115_h
#define ADS1115_h

// ADDRESSES
#define ADDRESS_GND                 (0x48)
#define ADDRESS_VDD                 (0x49)
#define ADDRESS_SDA                 (0x4A)
#define ADDRESS_SCL                 (0x4B)

// pointer register - see page 18 of TI data sheet
#define POINTER_REG_CONVERSION      (0x00)
#define POINTER_REG_CONFIG          (0x01)
#define POINTER_REG_LOTHRESH        (0x02)
#define POINTER_REG_HITHRESH        (0x03)

// OPERATIONAL STATUS (OS)
#define CONFIG_REG_OS_MASK          (0x8000)
#define CONFIG_REG_OS_W_NULL        (0x0000)
#define CONFIG_REG_OS_W_START_SNGL  (0x8000)
#define CONFIG_REG_OS_R_ACTIVE      (0x0000)
#define CONFIG_REG_OS_R_INACTIVE    (0x8000)

// MULTIPLEXER CONFIGURATION (MUX)
#define CONFIG_REG_MUX_MASK         (0x7000)
#define CONFIG_REG_MUX_DIFF_0_1     (0x0000)
#define CONFIG_REG_MUX_DIFF_0_3     (0x1000)
#define CONFIG_REG_MUX_DIFF_1_3     (0x2000)
#define CONFIG_REG_MUX_DIFF_2_3     (0x3000)
#define CONFIG_REG_MUX_SNGL_0       (0x4000)
#define CONFIG_REG_MUX_SNGL_1       (0x5000)
#define CONFIG_REG_MUX_SNGL_2       (0x6000)
#define CONFIG_REG_MUX_SNGL_3       (0x7000)

// PROGRAMMABLE GAIN AMPLIFIER (PGA)
#define CONFIG_REG_PGA_MASK         (0x0E00)
#define CONFIG_REG_PGA_6_144V       (0x0000)
#define CONFIG_REG_PGA_4_096V       (0x0200)
#define CONFIG_REG_PGA_2_048V       (0x0400)
#define CONFIG_REG_PGA_1_024V       (0x0600)
#define CONFIG_REG_PGA_0_512V       (0x0800)
#define CONFIG_REG_PGA_0_256V       (0x0A00)

// MODE
#define CONFIG_REG_MODE_MASK        (0x0100)
#define CONFIG_REG_MODE_SNGL        (0x0100)
#define CONFIG_REG_MODE_CONT        (0x0000)

// DATA RATE (DR)
#define CONFIG_REG_DR_MASK          (0x00E0)
#define CONFIG_REG_DR_8SPS          (0x0000)
#define CONFIG_REG_DR_16SPS         (0x0020)
#define CONFIG_REG_DR_32SPS         (0x0040)
#define CONFIG_REG_DR_64SPS         (0x0060)
#define CONFIG_REG_DR_128SPS        (0x0080)
#define CONFIG_REG_DR_250SPS        (0x00A0)
#define CONFIG_REG_DR_475SPS        (0x00C0)
#define CONFIG_REG_DR_860SPS        (0x00E0)

// COMPARATOR MODE (CM)
#define CONFIG_REG_CM_MASK          (0x0010)
#define CONFIG_REG_CM_TRAD          (0x0000)
#define CONFIG_REG_CM_WIND          (0x0010)

// COMPARATOR POLARITY (CP)
#define CONFIG_REG_CP_MASK          (0x0008)
#define CONFIG_REG_CP_ACTIVE_LOW    (0x0000)
#define CONFIG_REG_CP_ACTIVE_HIGH   (0x0008)

// COMPARATOR LATCH (CL)
#define CONFIG_REG_CL_MASK          (0x0004)
#define CONFIG_REG_CL_NONLATCH      (0x0000)
#define CONFIG_REG_CL_LATCH         (0x0004)

// COMPARATOR QUEUE (CQ)
#define CONFIG_REG_CQ_MASK          (0x0003)
#define CONFIG_REG_CQ_ASSERT_ONE    (0x0000)
#define CONFIG_REG_CQ_ASSERT_TWO    (0x0001)
#define CONFIG_REG_CQ_ASSERT_FOUR   (0x0002)
#define CONFIG_REG_CQ_DISABLE       (0x0003)

// COMPARATOR THRESHOLD SETTINGS
#define HITHRESH_RESET              (0x7FFF)
#define LOTHRESH_RESET              (0x8000)
#define HITHRESH_ALRT_TRIGGER       (0x8000)
#define LOTHRESH_ALRT_TRIGGER       (0x0000)

// MULTIPLIER (RESOLUTION) VALUES FOR EACH PGA SETTING
#define MULTIPLIER_PGA_6_144V       187.5000
#define MULTIPLIER_PGA_4_096V       125.0000
#define MULTIPLIER_PGA_2_048V        62.5000
#define MULTIPLIER_PGA_1_024V        31.2500
#define MULTIPLIER_PGA_0_512V        15.6250
#define MULTIPLIER_PGA_0_256V         7.8125

// ABSOLUTE VOLTAGE LIMIT FOR EACH PGA SETTING
#define PGA_LIMIT_6_144V               6.144
#define PGA_LIMIT_4_096V               4.096
#define PGA_LIMIT_2_048V               2.048
#define PGA_LIMIT_1_024V               1.024
#define PGA_LIMIT_0_512V               0.512
#define PGA_LIMIT_0_256V               0.256

enum adsMUX_t {
  AIN0_AIN1 = CONFIG_REG_MUX_DIFF_0_1,
  AIN0_AIN3 = CONFIG_REG_MUX_DIFF_0_3,
  AIN1_AIN3 = CONFIG_REG_MUX_DIFF_1_3,
  AIN2_AIN3 = CONFIG_REG_MUX_DIFF_2_3,
  AIN0_GND  = CONFIG_REG_MUX_SNGL_0,
  AIN1_GND  = CONFIG_REG_MUX_SNGL_1,
  AIN2_GND  = CONFIG_REG_MUX_SNGL_2,
  AIN3_GND  = CONFIG_REG_MUX_SNGL_3
};

enum adsPGA_t {
  GAIN_TWOTHIRDS  = CONFIG_REG_PGA_6_144V,
  GAIN_ONE        = CONFIG_REG_PGA_4_096V,
  GAIN_TWO        = CONFIG_REG_PGA_2_048V,
  GAIN_FOUR       = CONFIG_REG_PGA_1_024V,
  GAIN_EIGHT      = CONFIG_REG_PGA_0_512V,
  GAIN_SIXTEEN    = CONFIG_REG_PGA_0_256V
};

enum adsSPS_t {
  SPS_860 = CONFIG_REG_DR_860SPS,
  SPS_475 = CONFIG_REG_DR_475SPS,
  SPS_250 = CONFIG_REG_DR_250SPS,
  SPS_128 = CONFIG_REG_DR_128SPS,
  SPS_64  = CONFIG_REG_DR_64SPS,
  SPS_32  = CONFIG_REG_DR_32SPS,
  SPS_16  = CONFIG_REG_DR_16SPS,
  SPS_8   = CONFIG_REG_DR_8SPS
};

enum adsMode_t {
  CONTINUOUS  = CONFIG_REG_MODE_CONT,
  SINGLE      = CONFIG_REG_MODE_SNGL
};

class ADS1115 {

public:

  // CONSTRUCTORS
  ADS1115();
  ADS1115(uint8_t address);

  // CONFIGURATION SETUP FUNCTIONS
  void setMUX(adsMUX_t mux_option);
  void setPGA(adsPGA_t pga_option);
  void autosetPGA();
  float getPGALimit();
  void setMode(adsMode_t mode_option);
  adsMode_t getMode();
  void setSPS(adsSPS_t dr_option);
  uint16_t getSPS();

  // SINGLE MEASUREMENT MODE
  float recordSingleMeasurement();

  // CONTINUOUS MEASUREMENT MODE
  void startContinuousMeasurement();
  float readTriggeredMeasurement();

  // READ SETUP FROM CHIP
  void readADS1115Config();

  // CLEAR LAST MEASUREMENT
  void clear();

private:

  // I2C COMMS FUNCTIONS
  void _writeRegister(uint8_t address, uint8_t reg, uint16_t value);
  uint16_t _readRegister(uint8_t address, uint8_t reg);

  // READ AND WRITE PROCESSING FUNCTIONS
  uint16_t _createConfig();
  void _sendConfig();
  bool _isConverting();
  float _getVoltage();

  // COMP THRESHOLD REGISTER FUNCTIONS
  void _resetThresholdRegisters();
  void _alarmThresholdRegisters();

  // ADDRESS
  uint8_t _address;

  // DEFAULT CONFIG: SINGLE DIFFERENTIAL MEASUREMENT BETWEEN AIN0 AND AIN1
  uint16_t _os = CONFIG_REG_OS_W_START_SNGL;
  uint16_t _mux = CONFIG_REG_MUX_DIFF_0_1;
  uint16_t _pga = CONFIG_REG_PGA_6_144V;
  uint16_t _mode = CONFIG_REG_MODE_SNGL;
  uint16_t _dr = CONFIG_REG_DR_860SPS;
  uint16_t _cm = CONFIG_REG_CM_TRAD;
  uint16_t _cp = CONFIG_REG_CP_ACTIVE_LOW;
  uint16_t _cl = CONFIG_REG_CL_NONLATCH;
  uint16_t _cq = CONFIG_REG_CQ_DISABLE;

  // CORRESPONDING ADC RESOLUTION
  float _multiplier = MULTIPLIER_PGA_6_144V;
};

#endif
