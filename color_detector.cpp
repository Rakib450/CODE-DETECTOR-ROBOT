#include <NewPing.h>

// Configuration des broches pour le capteur de couleur
#define s0 22      
#define s1 23
#define s2 24
#define s3 25
#define out 26

// Capteurs ultrason
#define trigPinLeft 15
#define echoPinLeft 2    // INT4
#define trigPinFront 17
#define echoPinFront 21  // INT0
#define trigPinRight 18
#define echoPinRight 19  // INT5

// Pins moteurs
#define ENA 3
#define IN1 8
#define IN2 9
#define ENB 6
#define IN3 4
#define IN4 7

volatile float duration1, distance1;
volatile float duration2, distance2;
volatile float duration3, distance3;

int Red = 0, Blue = 0, Green = 0;
bool boostFait = false;

unsigned long lastLoopTime = 0;
const unsigned long loopInterval = 150; // ms

// Déclarations des fonctions
void handleEchoRight();
void handleEchoFront();
void handleEchoLeft();
void GetColors();
void avancer();
void arrier();
void tournerDroite();
void tournerGauche();
void arreter();
void choose();

void setup() {
  Serial.begin(115200);

  pinMode(ENA, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(ENB, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  pinMode(trigPinLeft, OUTPUT);
  pinMode(echoPinLeft, INPUT);
  pinMode(trigPinFront, OUTPUT);
  pinMode(echoPinFront, INPUT);
  pinMode(trigPinRight, OUTPUT);
  pinMode(echoPinRight, INPUT);

  pinMode(s0, OUTPUT);
  pinMode(s1, OUTPUT);
  pinMode(s2, OUTPUT);
  pinMode(s3, OUTPUT);
  pinMode(out, INPUT);
  digitalWrite(s0, HIGH);
  digitalWrite(s1, HIGH);

  attachInterrupt(digitalPinToInterrupt(echoPinLeft), handleEchoLeft, CHANGE);
  attachInterrupt(digitalPinToInterrupt(echoPinFront), handleEchoFront, CHANGE);
  attachInterrupt(digitalPinToInterrupt(echoPinRight), handleEchoRight, CHANGE);
}

void loop() {
  unsigned long currentTime = millis();
  if (currentTime - lastLoopTime >= loopInterval) {
    lastLoopTime = currentTime;

    digitalWrite(trigPinLeft, LOW); delayMicroseconds(2);
    digitalWrite(trigPinLeft, HIGH); delayMicroseconds(10);
    digitalWrite(trigPinLeft, LOW);

    digitalWrite(trigPinFront, LOW); delayMicroseconds(2);
    digitalWrite(trigPinFront, HIGH); delayMicroseconds(10);
    digitalWrite(trigPinFront, LOW);

    digitalWrite(trigPinRight, LOW); delayMicroseconds(2);
    digitalWrite(trigPinRight, HIGH); delayMicroseconds(10);
    digitalWrite(trigPinRight, LOW);

    
    GetColors();

    if (Red <= 15 && Green <= 15 && Blue <= 15) {
      choose();
    }
    else if (Red < Blue && Red <= Green && Red < 23) {
      tournerGauche();
      avancer();
      delay(50);
      Serial.println("RED");
    }
    else if (Blue < Green && Blue < Red && Blue < 20) {
      Serial.println("BLUE");
      tournerDroite();
      avancer();
      delay(50);                                                                                                                        
    }
    /*else if (Green < Red && Green - Blue <= 8) {
      avancer();
      Serial.println("GREEN");
    }*/
    else {
   choose();
    }
   
    
 

  
  }
}

// === Définition des fonctions ===

void handleEchoLeft() {
  static unsigned long startTime;
  if (digitalRead(echoPinLeft) == HIGH)
    startTime = micros();
  else
    distance1 = (micros() - startTime) * 0.0343 / 2.0;
}

void handleEchoFront() {
  static unsigned long startTime;
  if (digitalRead(echoPinFront) == HIGH)
    startTime = micros();
  else
    distance2 = (micros() - startTime) * 0.0343 / 2.0;
}

void handleEchoRight() {
  static unsigned long startTime;
  if (digitalRead(echoPinRight) == HIGH)
    startTime = micros();
  else
    distance3 = (micros() - startTime) * 0.0343 / 2.0;
}

void GetColors() {
  digitalWrite(s2, LOW);
  digitalWrite(s3, LOW);
  Red = pulseIn(out, digitalRead(out) == HIGH ? LOW : HIGH);
  delay(5);

  digitalWrite(s3, HIGH);
  Blue = pulseIn(out, digitalRead(out) == HIGH ? LOW : HIGH);
  delay(5);

  digitalWrite(s2, HIGH);
  Green = pulseIn(out, digitalRead(out) == HIGH ? LOW : HIGH);
  delay(5);
}

void avancer() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);

  if (!boostFait) {
    analogWrite(ENA, 100);
    analogWrite(ENB, 100);
    delay(150);
    boostFait = true;
  }

  analogWrite(ENA, 50);
  analogWrite(ENB, 60);
}

void arrier() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);

  if (!boostFait) {
    analogWrite(ENA, 100);
    analogWrite(ENB, 100);
    delay(150);
    boostFait = true;
  }

  analogWrite(ENA, 60);
  analogWrite(ENB, 70);
}

void tournerDroite() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  analogWrite(ENA, 80);
  analogWrite(ENB, 80);
  delay(550);
  arreter();
}

void tournerGauche() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  analogWrite(ENA, 80);
  analogWrite(ENB, 80);
  delay(550);
  arreter();
}

void arreter() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
  analogWrite(ENA, 0);
  analogWrite(ENB, 0);
  boostFait = false;
}

void choose() {
  if (distance2 > 20) {
    avancer();
  }else if (distance1 >= distance2 && distance1 >= distance3) {
    tournerGauche();
  } else if (distance3 >= distance2 && distance3 >= distance1) {
    tournerDroite();
  } else {
    tournerDroite();
    tournerDroite();  // Faire demi-tour
  }
}

