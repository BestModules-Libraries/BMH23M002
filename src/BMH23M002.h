/*************************************************
File:       			BMH23M002.h
Author:           BESTMODULES
Description:      Define classes and required variables
History：			    
V1.0.1	 -- initial version；2023-01-30；Arduino IDE : v1.8.13

**************************************************/ 

#ifndef  _BMH23M002_I2C_H__
#define  _BMH23M002_I2C_H__

#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>

class BMH23M002
{    
  public:
    BMH23M002(uint8_t intPin,TwoWire *thewire = &Wire);
    BMH23M002(uint8_t intPin, uint8_t csPin,SPIClass *theSPI = &SPI);
    
  
    void begin(uint8_t i2c_addr = 0x68);
    void beginADC();
    long readADCData();
    void endADC();
    void wakeup();
    void writeReg(uint8_t regAddr, uint8_t regData);
    uint8_t readReg(uint8_t regAddr);
    bool setChannel(uint8_t channel);
    bool setGain(uint8_t gain);
    bool setSampleRate ( uint8_t sampleRate);

  private:
    TwoWire *_wire = NULL;
    SPIClass *_theSPI = NULL;
    
    uint8_t _i2cAddress;
    uint16_t _intPin;
    uint16_t _csPin;
    uint8_t _HIRCCflag;
    void HIRCCstate();
    bool setTimeOut(uint8_t I2CTimeOut) ;
    static void DataReady();
};

#define     GAIN_1  0x00
#define     GAIN_2  0x01
#define     GAIN_4  0x02
#define     GAIN_8  0x03
#define     GAIN_16  0x04
#define     GAIN_32  0x05
#define     GAIN_64  0x06
#define     GAIN_128  0x46
  /*! Sampling Rate config */
#define     RATE_1280HZ  0x01
#define     RATE_640HZ  0x21
#define     RATE_320HZ  0x41
#define     RATE_160HZ  0x61
#define     RATE_80HZ  0x81
#define     RATE_40HZ  0xA1
#define     RATE_20HZ  0xC1
#define     RATE_10HZ  0xE1
  /*! I2C address config */
#define    ADDRESS_50  0x50
#define    ADDRESS_58  0x58
#define    ADDRESS_60  0x60
#define    ADDRESS_68  0x68
  /*! Adc channel negative config */  
#define    CHSP_AN0_N_AN1    (0x11)
#define    CHSP_AN2_N_AN3    (0x22)
#define    CHSP_VTSP_N_VTSN  (0x66)
    
#define REG0    (0x00) // 0x11 ----ADC Channel (0x11 AN0&AN1  0x22 AN2&AN3  0x66 TEMPSENSOR)
#define REG1    (0x01) // 0x   ----ADC GAIN (0 GAIN_1   1 GAIN_2  2 GAIN_4   3 GAIN_8   4 GAIN_16  5 GAIN_32   6 GAIN_64  70 GAIN_128)
#define REG2    (0x02) // 0x   ----ADC DATA RATE(0 1280Hz  1 640Hz  2 320Hz 3 160Hz  4 80Hz  5 40Hz  6 20Hz 7 10Hz)
#define REG3    (0x03) // 0x4F ----0X4F is recommended
#define REG4    (0x04) // 0x20 ----0x20 CHOP OFF  0x24 CHOP ON
#define REG5    (0x05) // 0xA0 ----0XA0 is recommended
#define REG6    (0x06) // 0x8F ----0X8F is recommended
#define REG7    (0x07) // 0x4F ----0x4F CHOP OFF  0xCF CHOP ON
#define REG8    (0x08) // 0x10 ----0x10 CHOP OFF  0x90 CHOP ON

#define REG16   (0x09) // ADC Data Low
#define REG17   (0x0A) // ADC Data Mid
#define REG18   (0x0B) // ADC Data High

#define REG19   (0x0C) // 0x01 ----0X01 is recommended

#define SIMC0   (0x0E)  // SPI  Bit0 SPI incomplete      I2C Unused
#define SIMC2   (0x0F)  // SPI Format                    I2C Unused
#define SIMTOC  (0x10) // I2C Bit7 TimeOut Disable/Enable    ----Bit6 TimeOut Flag    ----Bit5~0 timeout Time

#define HIRCC   (0x11) //  Bit2 HIRC Clock Output   ----Bit1 HIRC Clock stable   ----Bit0 HIRC Enable/Disable
#define HXTC    (0x12)  // Bit3 HXT  Clock Output ----Bit2 HXT Clock freq      ----Bit1 HXT Clock Stable    ----Bit0 HXT Enable/Disable


 #endif
