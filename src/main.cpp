#include <Arduino.h>
#include <SoftwareSerial.h>
#include <Servo.h>

#define memory 50
#define delayT 10 // Delay between moves

const int servoNum = 6;

// Initial positions
#define servo01235InitPos 90;
#define servo4InitPos 0;

// Creating servos
Servo servo[servoNum];

// Defining BT
SoftwareSerial Bluetooth(D7, D8);

// Servo positions
int servoPos[servoNum];

// Servo previous positions
int servoPPos[servoNum];

// Servo saved positions
int servoSPos[servoNum][memory];

// Position in saved positions
int indexS = 0;

// Received BT data
String dataIn = "";

// Function prototypes
void moveServo(int whichServo, int PosServo);
bool checkPos(int pos);

void setup()
{
  // Setting initial positions
  for (size_t j = 0; j < memory; j++)
  {
    for (size_t i = 0; i < servoNum; i++)
    {
      servoSPos[i][j] = servo01235InitPos;
    }
    servoSPos[4][j] = servo4InitPos;
  }

  // Ataching servos
  servo[0].attach(D6);
  servo[1].attach(D5);
  servo[2].attach(D4);
  servo[3].attach(D3);
  servo[4].attach(D2);
  servo[5].attach(D1);

  // Setting initial positions
  for (size_t i = 0; i < servoNum; i++)
  {
    servo[i].write(servoSPos[i][0]);
    servoPPos[i] = servoSPos[i][0];
  }

  // Initializing BT
  Bluetooth.begin(38400);
  Bluetooth.setTimeout(1);
  delay(delayT);
}

void loop()
{
  if (Bluetooth.available() > 0) // Check BT
  {
    delay(1);                        // Wait for BT data
    dataIn = Bluetooth.readString(); // Receive BT data
    if (dataIn.startsWith("s"))      // Servo position?
    {
      // Which servo?
      String dataInServo = dataIn.substring(1, 2);
      int SelectServo = dataInServo.toInt();

      // Which position?
      String dataInServoPos = dataIn.substring(2, dataIn.length());
      int PosServo = dataInServoPos.toInt();

      moveServo(SelectServo - 1, PosServo);
    }
    else if (dataIn.startsWith("c")) // Command?
    {
      // Which command?
      String dataInFunc = dataIn.substring(1, 2);
      int SelectFunc = dataInFunc.toInt();

      switch (SelectFunc)
      {

      case 1: // Save

        if (indexS < memory)
        {
          for (size_t i = 0; i < servoNum; i++)
          {
            servoSPos[i][indexS] = servoPos[i];
          }
          indexS++;
        }

        break;

      case 2: // Play

        for (int j = 0; j < indexS; j++)
        {
          while (checkPos(j))
          {
            for (size_t i = 0; i < servoNum; i++)
            {
              if (servoPos[i] > servoSPos[i][j])
              {
                servo[i].write(--servoPos[i]);
              }
              else if (servoPos[i] < servoSPos[i][j])
              {
                servo[i].write(++servoPos[i]);
              }
            }
          }
        }

        break;

      case 4: // Reset

        for (size_t i = 0; i < servoNum; i++)
        {
          for (size_t j = 0; j < memory; j++)
          {
            if (i != 4)
            {
              servoSPos[i][j] = servo01235InitPos;
            }
            else
            {
              servoSPos[i][j] = servo4InitPos;
            }
          }
        }
        indexS = 0;

        break;
      }
    }
  }
}

void moveServo(int whichServo, int PosServo)
{
  servoPos[whichServo] = PosServo;

  if (whichServo <= 2)
  {
    servo[whichServo].write(servoPos[whichServo]);
  }
  else
  {
    if (servoPos[whichServo] > servoPPos[whichServo])
    {
      for (int i = servoPPos[whichServo]; i <= servoPos[whichServo]; i++)
      {
        servo[whichServo].write(i);
      }
    }
    else if (servoPos[whichServo] < servoPPos[whichServo])
    {
      for (int i = servoPPos[whichServo]; i >= servoPos[whichServo]; i--)
      {
        servo[whichServo].write(i);
      }
    }
  }

  servoPPos[whichServo] = servoPos[whichServo];
}

bool checkPos(int pos)
{
  for (size_t i = 0; i < servoNum; i++)
  {
    if (servoPos[i] - servoSPos[i][pos] != 0)
    {
      return 1;
    }
  }
  return 0;
}