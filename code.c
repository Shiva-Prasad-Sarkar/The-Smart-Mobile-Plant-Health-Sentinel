#include <Servo.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27,16,2);

Servo plantServo;

// MOTOR PINS
#define IN1 8
#define IN2 9
#define IN3 10
#define IN4 11

// OTHER PINS
#define TRIG 6
#define ECHO 7
#define BUZZER 5
#define RELAY 4
#define SERVO_PIN 3

#define MOISTURE A0

// VARIABLES
int plantCount = 0;
bool leftSide = true;

unsigned long moveStartTime;
unsigned long pausedTime = 0;

const unsigned long PLANT_INTERVAL = 10000;

void setup()
{
  pinMode(IN1,OUTPUT);
  pinMode(IN2,OUTPUT);
  pinMode(IN3,OUTPUT);
  pinMode(IN4,OUTPUT);

  pinMode(TRIG,OUTPUT);
  pinMode(ECHO,INPUT);

  pinMode(BUZZER,OUTPUT);
  pinMode(RELAY,OUTPUT);

  digitalWrite(RELAY,HIGH);

  plantServo.attach(SERVO_PIN);

  lcd.init();
  lcd.backlight();

  plantServo.write(90);

  lcd.setCursor(0,0);
  lcd.print("Robot Starting");

  delay(2000);
}

void loop()
{
  servicePlants();
}

void servicePlants()
{
  plantCount = 0;

  while(plantCount < 5)
  {
    moveToNextPlant();

    stopMotors();

    if(leftSide)
      plantServo.write(30);
    else
      plantServo.write(150);

    delay(1000);

    waterRoutine();

    plantServo.write(90);

    plantCount++;
  }

  turnAround();

  leftSide = !leftSide;
}

void moveToNextPlant()
{
  moveForward();

  moveStartTime = millis();

  while(millis() - moveStartTime < PLANT_INTERVAL)
  {
    if(obstacleDetected())
    {
      stopMotors();

      digitalWrite(BUZZER,HIGH);

      unsigned long obstacleStart = millis();

      while(obstacleDetected())
      {
      }

      unsigned long obstacleEnd = millis();

      pausedTime += (obstacleEnd - obstacleStart);

      moveStartTime += (obstacleEnd - obstacleStart);

      digitalWrite(BUZZER,LOW);

      moveForward();
    }

    lcd.setCursor(0,0);
    lcd.print("Moving        ");
  }

  stopMotors();
}

void waterRoutine()
{
  int cycles = 0;

  while(cycles < 3)
  {
    int moisture = analogRead(MOISTURE);

    lcd.setCursor(0,0);
    lcd.print("Moist:");
    lcd.print(moisture);

    if(moisture < 700)
    {
      lcd.setCursor(0,1);
      lcd.print("Soil Wet      ");
      break;
    }

    lcd.setCursor(0,1);
    lcd.print("Watering...   ");

    digitalWrite(RELAY,LOW);

    delay(3000);

    digitalWrite(RELAY,HIGH);

    delay(2000);

    cycles++;
  }

  if(cycles >= 3)
  {
    lcd.setCursor(0,1);
    lcd.print("Max Water Done");
    delay(2000);
  }
}

bool obstacleDetected()
{
  long duration;
  int distance;

  digitalWrite(TRIG,LOW);
  delayMicroseconds(2);

  digitalWrite(TRIG,HIGH);
  delayMicroseconds(10);

  digitalWrite(TRIG,LOW);

  duration = pulseIn(ECHO,HIGH);

  distance = duration * 0.034 / 2;

  if(distance > 0 && distance < 15)
    return true;
  else
    return false;
}

void moveForward()
{
  digitalWrite(IN1,HIGH);
  digitalWrite(IN2,LOW);

  digitalWrite(IN3,HIGH);
  digitalWrite(IN4,LOW);
}

void stopMotors()
{
  digitalWrite(IN1,LOW);
  digitalWrite(IN2,LOW);

  digitalWrite(IN3,LOW);
  digitalWrite(IN4,LOW);
}

void turnAround()
{
  lcd.setCursor(0,0);
  lcd.print("Turning...    ");

  moveForward();

  delay(5000);

  digitalWrite(IN1,HIGH);
  digitalWrite(IN2,LOW);

  digitalWrite(IN3,LOW);
  digitalWrite(IN4,HIGH);

  delay(2500);

  moveForward();

  delay(5000);

  stopMotors();
}
