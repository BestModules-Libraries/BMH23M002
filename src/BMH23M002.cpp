/*************************************************
File:       			BMH23M002.cpp
Author:           BESTMODULES
Description:      I2C communication with the BMH23M002 and obtain the corresponding value 
History：			    
V1.0.1	 -- initial version；2023-01-30；Arduino IDE :v1.8.13

**************************************************/
#include "BMH23M002.h"

static volatile uint8_t _dataReady;
/*************************************************
Description:  Constructor
Parameters:        *thewire: Wire object if your board has more than one I2C interface   
              intpPin:INT Output pin connection with Arduino 
              the INT will be pulled down when data ready 
              address : Module IIC address      
Return:           
Others:            
*************************************************/
BMH23M002::BMH23M002(uint8_t intPin,TwoWire *thewire)
{
  _wire = thewire;
  _intPin = intPin;
}
/*************************************************
Description:  Constructor
Parameters:   *theSPI: Wire object if your board has more than one SPI interface   
              intpPin:INT Output pin connection with Arduino 
                      the INT will be pulled down when data ready    
              csPin:SPI communication chip select pin 
Return:           
Others:     
*************************************************/
BMH23M002::BMH23M002(uint8_t intPin,uint8_t csPin,SPIClass *theSPI)
{
  _intPin = intPin;
  _csPin = csPin;
  _theSPI = theSPI;
}
/*************************************************
Description:   Module Initial
Parameters:
Return:                    
Others:   
*************************************************/
void BMH23M002::begin(uint8_t address)
{
  _i2cAddress = address;
  _dataReady = 0;
  pinMode(_intPin, INPUT_PULLUP); // BMH23M002 Data Ready pin
  if (_wire != NULL)
  {
    _wire->begin();
  }    
  else
  {
     digitalWrite(_csPin, HIGH); // Disabling slave Devices
     _theSPI->begin();
     _theSPI->beginTransaction(SPISettings(2000000,LSBFIRST,SPI_MODE3));
  }
    HIRCCstate();//Wait for the crystal to stabilize
    //setTimeOut(2);
    setSampleRate(RATE_10HZ);
    setGain(GAIN_32);
    writeReg(REG3, 0x4F);
    writeReg(REG4, 0x20);
    writeReg(REG5, 0xA0);
    writeReg(REG6, 0x8F);
    writeReg(REG7, 0x4F);
    writeReg(REG8, 0x10);
    writeReg(REG19, 0x01);
    writeReg(SIMC0, 0x00);
}
/*************************************************
Description:  Start ADC conversion
Parameters:                  
Return:            
Others:            
*************************************************/
void  BMH23M002::beginADC()
{
  attachInterrupt(digitalPinToInterrupt(_intPin), DataReady, FALLING);
  writeReg(REG3, 0x4F | 0x90);
  writeReg(REG4, 0x20 | 0x80);
  writeReg(REG3, 0x4F);
  writeReg(REG4, 0x20);
}
/*************************************************
Description:  read ADC convert data
Parameters:            
Return:       N:data is updated 
              0：No data updated
Others:    
*************************************************/
long  BMH23M002::readADCData()
{
  long adc24Data = 0;
  uint16_t count = 1000;
  for (uint8_t i = 0; i < count; i++)
  {
      if (_dataReady)
      {
      break;
      }
      if (i == count - 1)
      {
        return 0;
      }
  }
  if (_dataReady)
  {
    _dataReady = 0;
    uint32_t dataL = readReg(REG16);
    uint32_t dataM = readReg(REG17);
    uint32_t dataH = readReg(REG18);
    int32_t adcdata = (dataH << 24) | (dataM << 16) | (dataL << 8);
    adcdata /= 256;
    adc24Data = adcdata;
    return adc24Data;
  }
  else
  {
     return 0;
  }
}
/*************************************************
Description:  Disable ADC function
Parameters:            
Return:       Put the module to sleep
Others:     
*************************************************/
void BMH23M002::endADC()
{
  writeReg(REG3, 0xF1);
  writeReg(REG4, 0xA6);
  writeReg(REG8, 0xE4);
  writeReg(HIRCC, 0x00);
  writeReg(HXTC, 0x00);
}
/*************************************************
Description:  Wake up the module
Parameters:           
Return:            
Others:     
*************************************************/
void BMH23M002::wakeup()
{
     writeReg(REG3, 0x4F);
     writeReg(REG4, 0x20);
     writeReg(REG8, 0x10);
     HIRCCstate();
}
/*************************************************
Description:  write register
Parameters:   regAdd:Register address 
              regData:Data written to registers      
Return:            
Others:       
*************************************************/
void  BMH23M002::writeReg(uint8_t regAddr,uint8_t regData)
{
  if(_wire != NULL)
  {
     while (_wire->available() > 0)
    {
      _wire->read();
    }
    _wire->beginTransmission(_i2cAddress);
    _wire->write(regAddr);
    _wire->write(regData);
    _wire->endTransmission();
  }
  else
  {
     digitalWrite(_csPin, LOW);  //Enabling slave Devices
     _theSPI->transfer(regAddr);
     _theSPI->transfer(regData);
     digitalWrite(_csPin, HIGH); 
  }
  delay(1);
}
/*************************************************
Description:  Read register value
Parameters:   regAdd:Register address        
Return:       Register value     
Others:  
*************************************************/
uint8_t  BMH23M002::readReg(uint8_t regAddr)
{
  uint8_t regData;
     if(_wire != NULL)
  {
       _wire->beginTransmission(_i2cAddress);
       _wire->write(regAddr);
       _wire->endTransmission();
       _wire->requestFrom((int)_i2cAddress,1);
       delay(1);
      if( _wire->available()== 1) 
      {
        regData =  _wire->read();
        return regData;
      }
      else
      {
        return 0;
      }
  }
  else
  {
      digitalWrite(_csPin, LOW);
      _theSPI->transfer(regAddr | 0x80);
      regData =  _theSPI->transfer(0x00);
      digitalWrite(_csPin, HIGH);
      return regData;
  }
  delay(1);
}
/*************************************************
Description:  set channel
Parameters:   channel: Adc channel negative config
                      CHSP_AN0_N_AN1  0x00
                      CHSP_AN2_N_AN3  0x09
                      CHSP_VDACO_N_VDACO  0x24
                      CHSP_VCM_N_VCM  0x36
                      CHSP_VTSP_N_VTSN  0x3F    
Return:       1：Set success 0：Set fail
Others: 
*************************************************/
bool BMH23M002::setChannel(uint8_t channel)
{
    if(_HIRCCflag)
    {
        writeReg(REG0, channel);
        return 1;
    }
    return 0;
}
/*************************************************
Description:  set GAIN
Parameters:   gain: GAIN_1  0x00
                    GAIN_2  0x01
                    GAIN_4  0x02
                    GAIN_8  0x03
                    GAIN_16  0x04
                    GAIN_32  0x05
                    GAIN_64  0x0D
                    GAIN_128  0x0E                  
Return:   1：Set success 0：Set fail
Others:   
*************************************************/
bool BMH23M002::setGain (uint8_t gain )
{
    if(_HIRCCflag)
    {
        writeReg(REG1, gain);
        return 1;
    }
    return 0;    
}
/*************************************************
Description:   set sampleRate
Parameters:    sampleRate:RATE_5HZ  0x00  
                          RATE_10HZ  0x01 
                          RATE_20HZ  0x02
                          RATE_40HZ  0x03
                          RATE_80HZ  0x04
                          RATE_160HZ 0x05
                          RATE_320HZ 0x06
                          RATE_640HZ 0x07                   
Return:     1：Set success 0：Set fail
Others:   
*************************************************/
bool BMH23M002::setSampleRate (uint8_t sampleRate)
{
    if(_HIRCCflag)
    {
        writeReg(REG2, sampleRate);
        return 1;
    }
    return 0;     
}
/*************************************************
Description:  set timeOut
Parameters:   I2CTimeOut:I2C timeout config                
Return:       1：Set success 0：Set fail
Others:   
*************************************************/
bool BMH23M002::setTimeOut(uint8_t I2CTimeOut)
{
    if(_HIRCCflag)
    {
        writeReg(SIMTOC,(1 << 7) + I2CTimeOut);
        return 1;
    }
    return 0;
}
/*************************************************
Description:  HIRCC Enable
Parameters:            
Return:       
Others:    
*************************************************/
void BMH23M002::HIRCCstate()
{
    writeReg(HIRCC, 0x01);
    uint16_t count = 1000;
    uint8_t temp;
    for (uint8_t i = 0; i < count; i++)
    {
        temp = readReg(HIRCC);
        if (temp == 0x03)
        {
        _HIRCCflag = 1;//success
        break;
        }
        if (i == count - 1)
        {
        _HIRCCflag = 0;//fail
        }
    }
}
/*************************************************
Description:  ISR
Parameters:           
Return:            
Others:             
*************************************************/
void  BMH23M002::DataReady()
{
  _dataReady = 1;
}
