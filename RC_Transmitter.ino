//Servo motor configuration
#include <Servo.h>

Servo servo;
//nrf24l01 configuration
#include <SPI.h>
#include "RF24.h"
#define D_RANGE 650
#define R_RANGE 180

//DC motor configuration
#define ENA 6
#define IN1 9
#define IN2 5
#define IN3 4
#define IN4 3
#define ENB 2

RF24 radio(7, 8);
byte addresses[][6] = {"Node1", "Node2"};
int data, R_data, P_data; // R_data => data from Rotary Encoder & P_data => data from slide potentiometer

void setup() {
  //Serial print
  Serial.begin(9600);
  Serial.print(F("Start"));
  //nrf24l01 setup
  radio.begin();
  radio.setRetries(15, 15);
  radio.setPALevel(RF24_PA_LOW);
  radio.openReadingPipe(1, addresses[0]);
  radio.startListening();

  //DC motor setup
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  digitalWrite(IN1,LOW); //forward
  digitalWrite(IN2,HIGH);
  digitalWrite(IN3,LOW); //foarward
  digitalWrite(IN4,HIGH);
  
  //Servo motor setup
  servo.attach(10);
  
  delay(10);
}


void loop() {

  if (radio.available()) {
      while (radio.available()) {
        radio.read(&data, sizeof(data)); //Read Data from the other nrf24l01
      }
      if (data > 200) {
        P_data = data / 10; //To differentiate V_data from the R_data, multiply the data from the slide_pot then devide it in equal number 
        P_data = P_data * 11 / 52;
        analogWrite(ENA, P_data); 
        analogWrite(ENB, P_data);

      }
      else {
        P_data = data; 
        servo.write(P_data);
      }

      Serial.print("Steering Wheel:");Serial.print(P_data);
      Serial.print("         ");
      Serial.print("Pedal: "); Serial.print(P_data);Serial.println(" ");
      delay(1);
  }
}
