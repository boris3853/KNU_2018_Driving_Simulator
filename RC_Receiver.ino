
//nrf24l01 configuration
#include <SPI.h>
#include "RF24.h"
RF24 radio(7,8);
byte addresses[][6] = {"Node"};
int R_data = 90;      // Initial Rotation Value (Steering Wheel)
int P_data = 0;       // Initial Velocity Value (Pedal)

//RotaryEncoder configuration

#include <RotaryEncoder.h>
#define ROTARYSTEPS 2
#define ROTARYMIN 0
#define ROTARYMAX 180
RotaryEncoder encoder(A2, A3);
int R_lastPos = -1;   // Previous Rotation Value

//Potentiometer  configuration
int P_lastPos = -1;   // Previous Velocity Value


void setup() {
  //Serial print
  Serial.begin(9600);
  Serial.println(F("Start"));
  //nrf24l01 setup
  radio.begin();
  radio.setRetries(15, 15);
  radio.setPALevel(RF24_PA_LOW);
  radio.openWritingPipe(addresses[0]);
  radio.startListening();

  //RotaryEncoder setup
    PCICR |= (1 << PCIE1);
    PCMSK1 |= (1 << PCINT10) | (1 << PCINT11); //interrupt in Analogpin 2&3
    encoder.setPosition(90 / ROTARYSTEPS); // start with the value of 90
    
  //Potentiometer setup
  pinMode(A0, INPUT);


  
  delay(10);
}

ISR(PCINT1_vect) { //Using Interrupt in Rotary Encoder
  encoder.tick(); 
}

void loop() {
  
  //Rotary Encoder positioning
    int R_newPos = encoder.getPosition() * ROTARYSTEPS;
   if (R_newPos < ROTARYMIN) {                        // position < 180
    encoder.setPosition(ROTARYMIN / ROTARYSTEPS);
    R_newPos = ROTARYMIN;
    } else if (R_newPos > ROTARYMAX) {                // position > 0
    encoder.setPosition(ROTARYMAX / ROTARYSTEPS);
    R_newPos = ROTARYMAX;
    }

    //Potentiometer Positioning
    int P_newPos = analogRead(A0);
    
        

  //nrf24l01 sending message
    if (R_lastPos != R_newPos) {
    Serial.print("Steering Wheel:");
    Serial.print(R_newPos);
    Serial.println();
    R_data = R_newPos;
    radio.stopListening();
    radio.write(&R_data,sizeof(R_data));  // Transmitting Steering Wheel Value
    radio.startListening();
    R_lastPos = R_newPos;
    }
    
    if (P_lastPos != P_newPos) {
    Serial.print("Padal:");
    Serial.print(P_newPos);
    Serial.println();
    P_data = P_newPos*10;
    radio.stopListening();
    radio.write(&P_data,sizeof(P_data)); // Transmitting Padal Value
    radio.startListening();
    P_lastPos = P_newPos;
    }


  delay(100);
}
