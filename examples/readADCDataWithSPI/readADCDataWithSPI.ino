/*************************************************
File:       		readADCDataWithSPI.ino
Description:      1.SPI is used to communicate with BMH23M002
                  2.hardware Serial (BAUDRATE 9600) is used to communicate with Serial port monitor.
Note:               
**************************************************/
#include "BMH23M002.h"

//BMH23M002 myADC(2,10,&SPI);  //Please uncomment out this line of code if you use SPI on BMduino
 BMH23M002 myADC(2,29,&SPI1); //Please uncomment out this line of code if you use SPI1 on BMduino
// BMH23M002 myADC(2,4,&SPI2);  //Please uncomment out this line of code if you use SPI2 on BMduino

void setup()
{
   Serial.begin(9600); // start serial for output
   myADC.begin();
   myADC.setChannel(CHSP_AN0_N_AN1);
   myADC.beginADC();
}

void loop()
{
     Serial.println(myADC.readADCData());
}
