#include <SoftwareSerial.h>
#include <Mouse.h>
#include <Keyboard.h>

SoftwareSerial EEBlue(10, 11); // RX | TX
int pinPhoto = A0;
const byte safetyPin = A1;

void setup()
{
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }
  EEBlue.begin(9600);  //Default Baud for comm, it may be different for your Module. 
  while (!EEBlue) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }
  Serial.println("The bluetooth gates are open.\n Connect to HC-05 from any other bluetooth device with 1234 as pairing key!.");
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
        
        Serial.print(wireInStr);
        
        if (String(wireInStr) == "B")
        {
          //wakeup from sleep routine         
          Keyboard.press(KEY_BACKSPACE);
          delay(10);
          Keyboard.releaseAll();
          Keyboard.press(KEY_LEFT_GUI);
          delay(10);
          Keyboard.releaseAll();
        }
        else
        {
          if (String(wireInStr) == "P")
          {
            int samples = 0;
            for (int i =0; i< 10; i++)
            {
              samples += analogRead(pinPhoto);
              delay(100);
            }
            //Serial.println(samples/10);
            EEBlue.println(samples/10);
          }
          else
          {
            Keyboard.print(String(wireInStr));
          }
        }
    }
  }
  //Serial.println(analogRead(pinPhoto));                

                                                        
  
  // Feed all data from termial to bluetooth
//  if (Serial.available())
//    EEBlue.write(Serial.read());
}
