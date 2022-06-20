#include <Arduino.h>
#include <PS4Controller.h>
#include <MotorDriver.h>

//MODOS
#define GATILLOS 0
#define ANALOGICOS 1

int aux = 0; 
int modo = 3;

Motor motorL;
Motor motorR;

void setup() {
  Serial.begin(115200);
  
  motorL.attach(13,12,14);
  motorR.attach(27,26,25);
  
  PS4.begin("ee:ff:ee:ff:01:01");
  

  while(modo == 3){
    if(PS4.isConnected()){
      if(PS4.Share())modo=GATILLOS;
      if(PS4.Options())modo=ANALOGICOS;
    }
    
  }  
}

void loop() {
  if(PS4.isConnected()){
    if(aux < 200){
      aux++;
      PS4.setLed(0,255,0);
      PS4.sendToController();
    }

    if(PS4.PSButton())ESP.restart();

    switch(modo){
      case GATILLOS:
        //Avanzar con R2, retroceder con L2 y girar con analogico
        int L_X = PS4.LStickX();
        if(PS4.R2()){
          int PWM_R2 = PS4.R2Value();
          if(L_X < 15 && L_X > -15){
            motorL.write(1, 0, PWM_R2);
            motorR.write(1, 0, PWM_R2);
          }
          else if(L_X < -15){
            L_X = map(L_X, -127, 0, 0, PWM_R2);
            motorL.write(1, 0, L_X);
            motorR.write(1, 0, PWM_R2);
          }
          else{
            L_X = map(L_X, 127, 0, 0, PWM_R2);
            motorL.write(1, 0, PWM_R2);
            motorR.write(1, 0, L_X);
          }
        }
        else if(PS4.L2()){
          int PWM_L2 = PS4.L2Value();
          if(L_X < 15 && L_X > -15){
            motorL.write(0, 1, PWM_L2);
            motorR.write(0, 1, PWM_L2);
          }
          else if(L_X < -15){
            L_X = map(L_X, -127, 0, 0, PWM_L2);
            motorL.write(0, 1, L_X);
            motorR.write(0, 1, PWM_L2);
          }
          else{
            L_X = map(L_X, 127, 0, 0, PWM_L2);
            motorL.write(0, 1, PWM_L2);
            motorR.write(0, 1, L_X);
          }
        }
        else if(L_X < -15){
          L_X = map(L_X, -127, 0, 0, 255);
          motorL.write(0, 1, L_X);
          motorR.write(1, 0, L_X);
        }
        else if(L_X > 15){
          L_X = map(L_X, 127, 0, 0, 255);
          motorR.write(0, 1, L_X);
          motorL.write(1, 0, L_X);
        }
        else{
          motorL.write(0,0,0);
          motorR.write(0,0,0);
          
        }

        motorL.printPWM();
        Serial.print(" ");
        motorR.printPWM();
        Serial.println();

      break;

      case ANALOGICOS:
        
      break;
      
    }
    
  }
  else{
    
  }
}