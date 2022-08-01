#include <SoftwareSerial.h>
#include <Keyboard.h>

SoftwareSerial EEBlue(8, 9); // RX | TX
int pinPhoto = A0;
const byte safetyPin = A1;

void setup()
{
  EEBlue.begin(9600);  //Default Baud for comm, it may be different for your Module. 
  while (!EEBlue) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }
  //
  //Serial.println("The bluetooth gates are open.\n Connect to HC-05 from any other bluetooth device with 1234 as pairing key!.");
  Keyboard.begin();
  pinMode(pinPhoto,INPUT);  //photocell sensor for unlock/screen state detection
}
 
void loop()
{
  if (digitalRead(safetyPin) == LOW)
  {
    int step = 10;
    // Feed any data from bluetooth to Terminal.
    if (EEBlue.available())
    {
        char  wireInStr; 
        wireInStr = EEBlue.read();       // receive byte as a character
         
        if (String(wireInStr) == "B")
        {
          ////
          //BEGIN wakeup from sleep routine for Motorola E7 (double tap space)    
          Keyboard.press(0x20); //SPACE
          delay(100);
          Keyboard.releaseAll();
          Keyboard.press(0x20); //SPACE
          delay(100);
          Keyboard.releaseAll();        
          // END wakeup from sleep routine for Motorola E7 (double tap space)
        }
        else
        {
          if (String(wireInStr) == "P")
          {
            int samples = 0;
            for (int i =0; i< 10; i++)
            {
              samples += analogRead(pinPhoto);
              delay(10);
            }
            EEBlue.println(samples/10);
          }
          else
          {
            if (wireInStr != 10)    //chr(10) == \n
            {
              Keyboard.print(String(wireInStr));
              delay(10);
            }
            else
            {
              Keyboard.println();
            }
                        
          }
        }
    }    
  }
}
