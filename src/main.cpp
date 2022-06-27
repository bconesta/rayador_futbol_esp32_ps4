#include <Arduino.h>
#include <PS4Controller.h>
#include <MotorDriver.h>
#include <BluetoothSerial.h>

//Definición de pines a utilizar por el puente H (L298N)
#define EN_1 14
#define IN1 13
#define IN2 12
#define EN_2 25
#define IN3 27
#define IN4 26

//Definición de pines a utilizar por sensores
#define TRIG_A 1
#define ECHO_A 1

#define TRIG_D 1
#define ECHO_D 1

#define TRIG_I 1
#define ECHO_I 1

#define IR_F 1
#define IR_D 1
#define IR_I 1

//MODOS
#define GATILLOS 0
#define ANALOGICOS 1
#define DESPEJE 2
#define LINEA 3

int aux = 0; 
int modo = 3;

#define DIST_MAX_DESP 100.00
#define DIST_MAX_LINEA 20.00
const unsigned long t_max_desp = (DIST_MAX_DESP/0.03433)*2.0;
const unsigned long t_max_linea = (DIST_MAX_LINEA/0.03433)*2.0;

bool sentido = false;

Motor motorL;
Motor motorR;

void setup() {
  Serial.begin(115200);
  
  motorL.attach(IN1,IN2,EN_1);
  motorR.attach(IN3,IN4,EN_2);
  
  pinMode(TRIG_A, OUTPUT);
  pinMode(TRIG_D, OUTPUT);
  pinMode(TRIG_I, OUTPUT);
  pinMode(ECHO_A, INPUT);
  pinMode(ECHO_D, INPUT);
  pinMode(ECHO_I, INPUT);

  pinMode(IR_F, INPUT);
  pinMode(IR_D, INPUT);
  pinMode(IR_I, INPUT);

  PS4.begin("ee:ff:ee:ff:01:04");
  Serial.println("Iniciando");
  while(modo == 3){
    if(PS4.isConnected()){
      if(PS4.Share()) modo=GATILLOS;
      else if(PS4.Options()) modo=ANALOGICOS;
      else if(PS4.Cross()) modo=DESPEJE;
      else if(PS4.Triangle()) modo=LINEA;
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
    int L_X = PS4.LStickX();
    switch(modo){
      case GATILLOS:
        //Avanzar con R2, retroceder con L2 y girar con analogico
        
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
//////////////////////////////////////////////////////////////////////////////////////////////////
      case ANALOGICOS:
        
      break;
//////////////////////////////////////////////////////////////////////////////////////////////////
      case DESPEJE:
        digitalWrite(TRIG_A, HIGH);
        delayMicroseconds(10);
        digitalWrite(TRIG_A, LOW);
        long distancia = pulseIn(ECHO_A, HIGH, t_max_desp);
        if(distancia){
          //Si sensa en el frente
          motorL.write(0,1,200);
          motorR.write(0,1,200);
        }
        else{
          digitalWrite(TRIG_D, HIGH);
          delayMicroseconds(10);
          digitalWrite(TRIG_D, LOW);
          distancia = pulseIn(ECHO_D, HIGH, t_max_desp);
          if(distancia){
            //Si sensa a la derecha
            sentido = true; 
          }
          else{
            digitalWrite(TRIG_I, HIGH);
            delayMicroseconds(10);
            digitalWrite(TRIG_I, LOW);
            distancia = pulseIn(ECHO_I, HIGH, t_max_desp);
            if(distancia){
              //Si sensa a la izquierda
              sentido = false;
            }
            else{
              //Si no sensó nada
            }
          }

          motorL.write(0,sentido,200);
          motorR.write(0,!sentido,200);
        
        }
      break;
//////////////////////////////////////////////////////////////////////////////////////////////////
      case LINEA:

      break;
//////////////////////////////////////////////////////////////////////////////////////////////////
    }
    
  }
  else{
    
  }
}