
/*************************************************
File:       		readADCDataWithIIC.ino
Description:      1.I2C is used to communicate with BMH23M002
                  2.hardware Serial (BAUDRATE 9600) is used to communicate with Serial port monitor.
Note:               
**************************************************/
#include "BMH23M002.h"

 BMH23M002 myADC(2,&Wire);  //Please uncomment out this line of code if you use Wire on BMduino
//BMH23M002 myADC(22,&Wire1); //Please uncomment out this line of code if you use Wire1 on BMduino
//BMH23M002 myADC(25,&Wire2); //Please uncomment out this line of code if you use Wire2 on BMduino

void setup()
{
   Serial.begin(9600); // start serial for output
   myADC.begin();//Module initialization
   myADC.setChannel(CHSP_AN0_N_AN1);
   myADC.beginADC(); 
}

void loop()
{
     Serial.println(myADC.readADCData());
   
}
