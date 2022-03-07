#include <SoftwareSerial.h>
#include <Mouse.h>
#include <MouseTo.h>
#include <Keyboard.h>

SoftwareSerial EEBlue(10, 11); // RX | TX
int pinPhoto = A0;
const byte safetyPin = A1;
 
int hScreen = 720;  //screen absolute resolution horizontal
int vScreen = 1520; //screen absolute resolution vertical

// Redmi M1908
// Dot screen locations
// 6,1 inches
// 1520x720 - calibra ponto inicial (ponto 1)
  int x1 = 78; 
  int y1 = 474;
//Step to locate another absolute dot in screen - calibra distancia entre pontos - usar funcao test
  int c = 98;
// Relative step - distancia diagonal
  int h = 98;
// Relative step to long diagonal
  int h1 = 282;

// Pause frame
int pause = 150; //Delay to the next pattern in milliseconds

// Define location of dots in the x and y axis
int x2 = x1 + c;
int y2 = y1;
int x3 = x1 + 2 * c;
int y3 = y1;
int x4 = x1;
int y4 = y1 + c;
int x5 = x1 + c;
int y5 = y1 + c;
int x6 = x1 + 2 * c;
int y6 = y1 + c;
int x7 = x1;
int y7 = y1 + 2 * c;
int x8 = x1 + c;
int y8 = y1 + 2 * c;
int x9 = x1 + 2 * c;
int y9 = y1 + 2 * c;

void mover(int num) {
  MouseTo.home();
  switch (num) {
    case 1:
      MouseTo.setTarget(x1, y1);
      break;
    case 2:
      MouseTo.setTarget(x2, y2);
      break;
    case 3:
      MouseTo.setTarget(x3, y3);
      break;
    case 4:
      MouseTo.setTarget(x4, y4);
      break;
    case 5:
      MouseTo.setTarget(x5, y5);
      break;
    case 6:
      MouseTo.setTarget(x6, y6);
      break;
    case 7:
      MouseTo.setTarget(x7, y7);
      break;
    case 8:
      MouseTo.setTarget(x8, y8);
      break;
    case 9:
      MouseTo.setTarget(x9, y9);
      break;
  }
 while (MouseTo.move() == false) {}
    delay(pause);
    Mouse.press();
}

    
void setup()
{
  //Serial.begin(9600);
//  while (!Serial) {
//    ; // wait for serial port to connect. Needed for Leonardo only
//  }
  EEBlue.begin(9600);  //Default Baud for comm, it may be different for your Module. 
  while (!EEBlue) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }
  //
  //Serial.println("The bluetooth gates are open.\n Connect to HC-05 from any other bluetooth device with 1234 as pairing key!.");
  Keyboard.begin();
  Mouse.begin();
  MouseTo.setCorrectionFactor(1);
  MouseTo.setScreenResolution(hScreen,vScreen);
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
        
//        Serial.print(wireInStr);
        
        if (String(wireInStr) == "B")
        {
          ////
          //BEGIN wakeup from sleep routine for Redmi M1908 (triple tap space)    
          Keyboard.press(0x20); //SPACE
          delay(300);
          Keyboard.releaseAll();
          Keyboard.press(0x20); //SPACE
          delay(300);
          Keyboard.releaseAll();
          Keyboard.press(0x20); //SPACE
          delay(300);
          Keyboard.releaseAll();
          // END wakeup from sleep routine for Redmi M1908 (triple tap space)
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
            //Serial.println(samples/10);
            EEBlue.println(samples/10);
          }
          else
          {
            if (String(wireInStr) == "C")
            {
              //Serial.println("Calibration");
              //MouseTo.home();
              delay(100);
//              checkh();
//              int sequencia0[5];
//              sequencia0[0] = 7;
//              sequencia0[1] = 5;
//              sequencia0[2] = 2;
//              sequencia0[3] = 1;
//              sequencia0[4] = 4;
//              sequencia0[5] = 3;
//              sequencia0[6] = 3;
//              sequencia0[7] = 2;
//              sequencia0[8] = 1;
//              movesequence(sequencia0,5);
            }
            else
            {
//            if (wireInStr != 10)    //chr(10) == \n
//            {
//              Keyboard.print(String(wireInStr));
//              delay(10);
//            }
//            else
//            {
//              Keyboard.println();
//            }

            }
                        
          }
        }
    }    
  }
  //Serial.println(analogRead(pinPhoto));                                                                   
  
  // Feed all data from termial to bluetooth
//  if (Serial.available())
//    EEBlue.write(Serial.read());
}

void validatec(){
for ( int walk = 1; walk <= 9; walk++ )
    {
      mover(walk);
      delay(2000);
      Mouse.release();
    }
}

//funcao para calibrar h
void checkh(){
for ( int centena = 1; centena <= 1; centena++ )
   {
    for ( int decena = 1; decena <= 2; decena++ )
       {           
        for ( int unidad = 1; unidad <= 9; unidad++ )
       {    
        mover(centena);
        delay(pause);
        moverrel(centena,decena);
        delay(pause);
        moverrel(decena,unidad);
        delay(pause);
        Mouse.release();
       }
      }
  }
}


//funcao que move o mouse entre os pontos
void moverrel(int origen, int destino) {
Mouse.begin();
if (origen == 1){
               if (destino == 2){
                Mouse.move(h,0);
               }
               if (destino == 3){//usado no caso de ponto 2 ja ter sido clicado
                Mouse.begin();
                Mouse.move(h,0);
                delay(pause);
                Mouse.begin();
                Mouse.move(h,0);
               }
              if (destino == 4){
                Mouse.move(0,h);
               }
              if (destino == 5){
                Mouse.move(h,h);
               }
              if (destino == 6){
                Mouse.begin();
                Mouse.move(h/2,h/2);
                delay(pause);
                Mouse.begin();
                Mouse.move(h/2,0);
                delay(pause);
                Mouse.begin();
                Mouse.move(h/2,0);
                delay(pause);
                Mouse.begin();
                Mouse.move(h/2,h/2);
               }
              if (destino == 7){//usado no caso de ponto 4 ja ter sido clicado
                Mouse.begin();
                Mouse.move(0,h);
                delay(pause);
                Mouse.begin();
                Mouse.move(0,h);
               }
              if (destino == 8){
                Mouse.begin();
                Mouse.move(h/2,h/2);
                delay(pause);
                Mouse.begin();
                Mouse.move(0,h/2);
                delay(pause);
                Mouse.begin();
                Mouse.move(0,h/2);
                delay(pause);
                Mouse.begin();
                Mouse.move(h/2,h/2);
               }
               if (destino == 9){//usado no caso de ponto 5 ja ter sido clicado
                Mouse.begin();
                Mouse.move(h,h);
                delay(pause);
                Mouse.begin();
                Mouse.move(h,h);
               }
 }
if (origen == 2){     
               if (destino == 1){
                Mouse.move(-h,0);
              }
              if (destino == 3){
                Mouse.move(h,0);
              }
              if (destino == 4){
                Mouse.move(-h,h);
              }
              if (destino == 5){
                Mouse.move(0,h);
              }
              if (destino == 6){
                Mouse.move(h,h);
              }
              if (destino == 7){
                Mouse.begin();
                Mouse.move(-h/2,h/2);
                delay(pause);
                Mouse.begin();
                Mouse.move(0,h/2);
                delay(pause);
                Mouse.begin();
                Mouse.move(0,h/2);
                delay(pause);
                Mouse.begin();
                Mouse.move(-h/2,h/2);
              }
              if (destino == 8){//usado no caso de ponto 5 ja ter sido clicado
                Mouse.begin();
                Mouse.move(0,h);
                delay(pause);
                Mouse.begin();
                Mouse.move(0,h);
              }
              if (destino == 9){
                Mouse.begin();
                Mouse.move(h/2,h/2);
                delay(pause);
                Mouse.begin();
                Mouse.move(0,h/2);
                delay(pause);
                Mouse.begin();
                Mouse.move(0,h/2);
                delay(pause);
                Mouse.begin();
                Mouse.move(h/2,h/2);
              }
}
if (origen == 3){
              if (destino == 1){//usado no caso de ponto 2 ja ter sido clicado
                Mouse.begin();
                Mouse.move(-h,0);
                delay(pause);
                Mouse.begin();
                Mouse.move(-h,0);
              }
              if (destino == 2){
                Mouse.move(-h,0);
              }
              if (destino == 4){
                Mouse.begin();
                Mouse.move(-h/2,h/2);
                delay(pause);
                Mouse.begin();
                Mouse.move(-h/2,0);
                delay(pause);
                Mouse.begin();
                Mouse.move(-h/2,0);
                delay(pause);
                Mouse.begin();
                Mouse.move(-h/2,h/2);
              }
              if (destino == 5){
                Mouse.move(-h,h);
              }
              if (destino == 6){
                Mouse.move(0,h);
              }
              if (destino == 7){//usado no caso de ponto 5 ja ter sido clicado
                Mouse.begin();
                Mouse.move(-h,h);
                delay(pause);
                Mouse.begin();
                Mouse.move(-h,h);
              }
              if (destino == 8){
                Mouse.begin();
                Mouse.move(-h/2,h/2);
                delay(pause);
                Mouse.begin();
                Mouse.move(0,h/2);
                delay(pause);
                Mouse.begin();
                Mouse.move(0,h/2);
                delay(pause);
                Mouse.begin();
                Mouse.move(-h/2,h/2);
              }
              if (destino == 9){//usado no caso de ponto 6 ja ter sido clicado
                Mouse.begin();
                Mouse.move(0,h);
                delay(pause);
                Mouse.begin();
                Mouse.move(0,h);
              }
}
if (origen == 4){
              if (destino == 1){
                Mouse.move(0,-h);
              }
              if (destino == 2){
                Mouse.move(h,-h);
              }
              if (destino == 3){
                Mouse.begin();
                Mouse.move(h/2,-h/2);
                delay(pause);
                Mouse.begin();
                Mouse.move(h/2,0);
                delay(pause);
                Mouse.begin();
                Mouse.move(h/2,0);
                delay(pause);
                Mouse.begin();
                Mouse.move(h/2,-h/2);
              }
              if (destino == 5){
                Mouse.move(h,0);
              }
              if (destino == 6){//usado no caso de ponto 5 ja ter sido clicado
                Mouse.begin();
                Mouse.move(h,0);
                delay(pause);
                Mouse.begin();
                Mouse.move(h,0);
              }
              if (destino == 7){
                Mouse.move(0,h);
              }
              if (destino == 8){
                Mouse.move(h,h);
              }
              if (destino == 9){
                Mouse.begin();
                Mouse.move(h/2,h/2);
                delay(pause);
                Mouse.begin();
                Mouse.move(h/2,0);
                delay(pause);
                Mouse.begin();
                Mouse.move(h/2,0);
                delay(pause);
                Mouse.begin();
                Mouse.move(h/2,h/2);
              }
}
if (origen == 5){
              if (destino == 1){
                Mouse.move(-h,-h);
              }
              if (destino == 2){
                Mouse.move(0,-h);
              }
              if (destino == 3){
                Mouse.move(h,-h);
              }
              if (destino == 4){
                Mouse.move(-h,0);
              }
              if (destino == 6){
                Mouse.move(h,0);
              }
              if (destino == 7){
                Mouse.move(-h,h);
              }
              if (destino == 8){
                Mouse.move(0,h);
              }
              if (destino == 9){
                Mouse.move(h,h);
              }
}
if (origen == 6){
              if (destino == 1){
                Mouse.begin();
                Mouse.move(-h/2,-h/2);
                delay(pause);
                Mouse.begin();
                Mouse.move(-h/2,0);
                delay(pause);
                Mouse.begin();
                Mouse.move(-h/2,0);
                delay(pause);
                Mouse.begin();
                Mouse.move(-h/2,-h/2);
              }
              if (destino == 2){
                Mouse.move(-h,-h);
              }
              if (destino == 3){
                Mouse.move(0,-h);
              }
              if (destino == 4){//usado no caso de ponto 5 ja ter sido clicado
                Mouse.begin();
                Mouse.move(-h,0);
                delay(pause);
                Mouse.begin();
                Mouse.move(-h,0);
              }
              if (destino == 5){
                Mouse.move(-h,0);
              }
              if (destino == 7){
                Mouse.begin();
                Mouse.move(-h/2,h/2);
                delay(pause);
                Mouse.begin();
                Mouse.move(-h/2,0); 
                delay(pause);
                Mouse.begin();
                Mouse.move(-h/2,0);
                delay(pause);
                Mouse.begin();
                Mouse.move(-h/2,h/2);
              }
              if (destino == 8){
                Mouse.move(-h,h);
              }
              if (destino == 9){
                Mouse.move(0,h);
              }
                
}
if (origen == 7){
              if (destino == 1){//usado no caso de ponto 4 ja ter sido clicado
                Mouse.begin();
                Mouse.move(0,-h);
                delay(pause);
                Mouse.begin();
                Mouse.move(0,-h);
              }
              if (destino == 2){
                Mouse.begin();
                Mouse.move(h/2,-h/2);
                delay(pause);
                Mouse.begin();
                Mouse.move(0,-h/2);
                delay(pause);
                Mouse.begin();
                Mouse.move(0,-h/2);
                delay(pause);
                Mouse.begin();
                Mouse.move(h/2,-h/2);
              }
              if (destino == 3){//usado no caso de ponto 5 ja ter sido clicado
                Mouse.begin();
                Mouse.move(h,-h);
                delay(pause);
                Mouse.begin();
                Mouse.move(h,-h);
              }
              if (destino == 4){
                Mouse.move(0,-h);
              }
              if (destino == 5){
                Mouse.move(h,-h);
              }
              if (destino == 6){
                Mouse.begin();
                Mouse.move(h/2,-h/2);
                delay(pause);
                Mouse.begin();
                Mouse.move(h/2,0);
                delay(pause);
                Mouse.begin();
                Mouse.move(h/2,0);
                delay(pause);
                Mouse.begin();
                Mouse.move(h/2,-h/2);
              }
              if (destino == 8){
                Mouse.move(h,0);
              }
              if (destino == 9){//usado no caso de ponto 8 ja ter sido clicado
                Mouse.begin();
                Mouse.move(h,0);
                delay(pause);
                Mouse.begin();
                Mouse.move(h,0);
              }
}
if (origen == 8){
              if (destino == 1){
                Mouse.begin();
                Mouse.move(-h/2,-h/2);
                delay(pause);
                Mouse.begin();
                Mouse.move(0,-h/2);
                delay(pause);
                Mouse.begin();
                Mouse.move(0,-h/2);
                delay(pause);
                Mouse.begin();
                Mouse.move(-h/2,-h/2);              
              }
              if (destino == 2){//usado no caso de ponto 8 ja ter sido clicado
                Mouse.begin();
                Mouse.move(0,-h);
                delay(pause);
                Mouse.begin();
                Mouse.move(0,-h);
              }
              if (destino == 3){
                Mouse.begin();
                Mouse.move(h/2,-h/2);
                delay(pause);
                Mouse.begin();
                Mouse.move(0,-h/2);
                delay(pause);
                Mouse.begin();
                Mouse.move(0,-h/2);
                delay(pause);
                Mouse.begin();
                Mouse.move(h/2,-h/2);    
              }
              if (destino == 4){
                Mouse.move(-h,-h);
              }
              if (destino == 5){
                Mouse.move(0,-h);
              }
              if (destino == 6){
                Mouse.move(h,-h);
              }
              if (destino == 7){
                Mouse.move(-h,0);
              }
              if (destino == 9){
                Mouse.move(h,0);
              }
}              
if (origen == 9){
             if (destino == 1){//usado no caso de ponto 5 ja ter sido clicado
                Mouse.begin();
                Mouse.move(-h,-h);
                delay(pause);
                Mouse.begin();
                Mouse.move(-h,-h);
              }
             if (destino == 2){
                Mouse.begin();
                Mouse.move(-h/2,-h/2);
                delay(pause);
                Mouse.begin();
                Mouse.move(0,-h/2);
                delay(pause);
                Mouse.begin();
                Mouse.move(0,-h/2);
                delay(pause);
                Mouse.begin();
                Mouse.move(-h/2,-h/2);    
              }
             if (destino == 3){//usado no caso de ponto 6 ja ter sido clicado
                Mouse.begin();
                Mouse.move(0,-h);
                delay(pause);
                Mouse.begin();
                Mouse.move(0,-h);
              }
             if (destino == 4){
                Mouse.begin();
                Mouse.move(-h/2,-h/2);
                delay(pause);
                Mouse.begin();
                Mouse.move(-h/2,0); 
                delay(pause);
                Mouse.begin();
                Mouse.move(-h/2,0); 
                delay(pause);
                Mouse.begin();
                Mouse.move(-h/2,-h/2);    
              }
             if (destino == 5){
                Mouse.move(-h,-h);
              }
             if (destino == 6){
                Mouse.move(0,-h);
              }
              if (destino == 7){//usado no caso de ponto 8 ja ter sido clicado
                Mouse.begin();
                Mouse.move(-h,0);
                delay(pause);
                Mouse.begin();
                Mouse.move(-h,0);
              }
             if (destino == 8){
                Mouse.move(-h,0);
              }
      }
Mouse.end();
delay(pause);
}

void movesequence(int patron[], int longitud){
  int i;  
  int h3 = h1;
  int h4 = 0;
  
  //posiciona o mouse para o primeiro ponto
  mover(patron[0]);
  delay(pause);
  //se h1 eh maior que h
  if (h1 > h) {
    for (i = 0; i < longitud - 1; i++){
      do
      {
        moverrel(patron[i],patron[i+1]);
        delay(pause);
        h1 = h1 - h;
      } while (h1 >= h);
      if (h1 < h) {
          h4 = h;
          h = h1;
          moverrel(patron[i],patron[i+1]);
          h = h4;
       }
       h1 = h3;
       delay(pause);
    }
    Mouse.release();
  }
  //se h1 nao eh maior que h
  else
  {
    for (i = 0; i < longitud - 1; i++){
      moverrel(patron[i],patron[i+1]);
      delay(pause);
    }
    Mouse.release();
  }
}
