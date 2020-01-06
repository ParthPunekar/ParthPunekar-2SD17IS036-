#include <Wire.h>           //To establish I2C communication
#include <NewPing.h>        //To read data from ultrasonic sensor
#define Slave 9             //I2C Slave Address

/*=======================================================================*
   Defining Pins of Ultrasonic sensors and defining the maximum distance
  =======================================================================*/
#define triggerPin0 8
#define echoPin0 8
#define triggerPin1 9
#define echoPin1 9
#define triggerPin2 10
#define echoPin2 10
#define maxDist 255
/*=======================================================================*
   Defining Solar Panal pins
  =======================================================================*/
#define solar0 A0
#define solar1 A1
#define solar2 A2
/*=======================================================================*
   Defining LDR pins
  =======================================================================*/
#define LDR0 A3
#define LDR1 A6
#define LDR2 A7
/*======================================================================*
   Object creation of the NewPing Class
  ======================================================================*/
NewPing sensor0(triggerPin0, echoPin0, maxDist);
NewPing sensor1(triggerPin1, echoPin1, maxDist);
NewPing sensor2(triggerPin2, echoPin2, maxDist);

const String password = "clckfajdd";      //Password Expected
String passwd = "";                       //Password Read
int Distance[3];                          //Distance Array (3 as there are three sensors)
int bcount = 1;                           //Counter to count which sensor to read from
bool flag = false;                        //Password Authenticator
byte bval = LOW;                          //Value to indicate the read value from the sensor
int val0;                                 //To read value of LDR0
int val1;                                 //To read value of LDR0
int val2;                                 //To read value of LDR0
int val0_0;                                 //To read value of LDR0
int val1_1;                                 //To read value of LDR0
int val2_2;                                 //To read value of LDR0
int temp;
/*=========================================================================*
   Setup Loop
  =========================================================================*/

void setup () {

  Wire.begin(Slave);                    //Begin I2C communication
  Serial.begin(9600);
  pinMode (solar0, INPUT);              //Declaring Solar panal as input
  pinMode (solar1, INPUT);              //Declaring Solar panal as input
  pinMode (solar2, INPUT);              //Declaring Solar panal as input
  pinMode (LDR0, INPUT);                //Declaring LDR as input
  pinMode (LDR1, INPUT);                //Declaring LDR as input
  pinMode (LDR2, INPUT);                //Declaring LDR as input
  Wire.onRequest(requestEvent);         //Method requestEvent invoked when the master requests any data from slave
  Wire.onReceive(receiveEvent);         //Method receiveEvent invoked when the master sends any data to the slave

  val0 = analogRead(solar0);
  val1 = analogRead(solar1);
  
  val2 = analogRead(solar2);
}


/*=========================================================================*
   Methord receiveEvent() -> Reads Password from the Master
  =========================================================================*/
  
void receiveEvent() {
  while (Wire.available()) {
    passwd += (char)Wire.read();
  }
}


/*=========================================================================*
   Method requestEvent()
  =========================================================================*/
  
void requestEvent() {

  /*
   * Password Authentication
   */
  if (passwd == password) {
    Wire.write(1);            //Sends Password is correct 
    flag = true;              //Sets Authentication flag to true
    passwd = "";              //Reset the passwd variable
  }
  else {                      //Sends Password is incorrect 
    passwd = "";              //Reset the passwd variable
  }

  if (flag) {

    /*
     * Analysing the distance and informing the master as per the condition
     */

    switch (bcount) {

      case 1:
      /*
       * Analysing Sensor1 Readings
       */
        if (Distance[0] < 10 && analogRead(solar0) > val0 + 100)
          bval = HIGH;
        else
          bval = LOW;
        break;

      case 2:
      
      /*
       * Analysing Sensor2 Readings
       */
        if (Distance[1] < 10 && analogRead(solar1) > val1 + 100)
          bval = HIGH;
        else
          bval = LOW;
        break;

      case 3:
      
      /*
       * Analysing Sensor3 Readings
       */
        if (Distance[2] < 10 && analogRead(solar2) > val2 + 100)
          bval = HIGH;
        else
          bval = LOW;
        break;

    }

    Wire.write(bval);           //Informing to master Serially
    bcount = bcount + 1;        //Increment the counter
    if (bcount > 3)             //Limit the count to 3
      bcount = 1;
  }
}


/*=========================================================================*
   Method readDistance() to read the distane from the sensors
  =========================================================================*/

void readDistance() {
  Distance[0] = sensor0.ping_cm();      //Reading Sensor1 in cm
  /*Serial.print(Distance[0]);
  Serial.print("\t");
  Serial.println(analogRead(solar0));*/
  if (Distance[0] > 254) {              //If distance exceeds Max distance 
    Distance[0] = 254;                  //Set the distance to maximum
  }
  delay (10);

  Distance[1] = sensor1.ping_cm();      //Reading Sensor2 in cm
  //Serial.print(Distance[1]);
  //Serial.print("\t");
  if (Distance[1] > 254) {              //If distance exceeds Max distance 
    Distance[1] = 254;                  //Set the distance to maximum
  }
  delay (10);

  Distance[2] = sensor2.ping_cm();      //Reading Sensor3 in cm
  //Serial.println(Distance[2]);
  if (Distance[2] > 254) {              //If distance exceeds Max distance 
    Distance[2] = 254;                  //Set the distance to maximum
  }
  delay(10);

}

/*=========================================================================*
   Method Loop()
  =========================================================================*/

void loop () {
  Serial.print(val0);
  Serial.print("\t");
  Serial.print(analogRead(solar0));
  Serial.print("\t");
  Serial.print(val1);
  Serial.print("\t");
  Serial.print(analogRead(solar1));
  Serial.print("\t");
  Serial.print(val2);
  Serial.print("\t");
  Serial.println(analogRead(solar2));
  
  //while(Serial.read() != 'a');
  if (flag) {                         //If Password is correct
    readDistance();                   //Read Distances from the sensors
  }
  delay(30);                          //Delay to coordinate the sensor and the microcontroller speed
}


/*=========================================================================*
   End of Slave Part Program
  =========================================================================*/
