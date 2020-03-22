/*
School Project by Eric Nordgren IoT19

In this project I have wanted to see how efficient and 
fast the arduino could send sensorvalues to the database and 
be used on a webserver that I created with NodeJS.

*/

#include <WiFi101.h>
#include <MySQL_Connection.h>
#include <MySQL_Cursor.h>

WiFiClient client;
MySQL_Connection conn((Client *)&client);   // Creates the connection object


char ssid[] = "";
char pass []="";


IPAddress server_addr();    // MySQL server IP home - IPv4 - adress

char user[] = "";                      // MySQL user
char password[] = "";                  // MySQL password

//The pins

const int echoPin=2;
const int triggerPin=1;
#define gasPin A5
const int motorPin=3;
const int redLed=6;
const int greenLed=7;
const int blueLed=5;

//The variables

int dataBaseCounter = 0; //Variable to make sure that only one value gets updated to the database
int displayEndurance=0;
int defaultAirQuality = 0;
int airQuality = 0;
int seconds = 0;


void setup() {
  
  //exit(0); //Function to exit the program and reset the arduino if needed
  pinMode(blueLed,OUTPUT);
  pinMode(triggerPin,OUTPUT);
  pinMode(echoPin,INPUT);
  pinMode(gasPin,INPUT);
  pinMode(redLed,OUTPUT);
  pinMode(greenLed,OUTPUT);
  pinMode(motorPin,OUTPUT);
  digitalWrite(blueLed,HIGH); //Blue light to indicate that program i initializing
  delay(20000); //Delay to heat up the gassensor
  defaultAirQuality = analogRead(gasPin)*4; //Sets the default air quality of the current room
  Serial.begin(9600);
  Serial.println("Initialising connection");
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, pass);
  
  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(1000);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi Connected");
  Serial.print("Assigned IP: ");
  Serial.println(WiFi.localIP());
  Serial.println("Connecting to database");
  

  while (conn.connect(server_addr, 3306, user, password) != true)  // Looping until MySQL is connected
  {
    delay(200);
    Serial.print (".");
  }

  Serial.println("");
  Serial.println("Connected to SQL Server!");
  digitalWrite(greenLed,HIGH); //Green light to indicate that we now have a connection to the database
  digitalWrite(blueLed,LOW);
}



void loop() 
{
  
  airQuality = analogRead(gasPin)*4; //Reads the value of the gas sensor

  outBreakAlgorithm(); //This function activates if the air is polluted
  
  distanceGenerator(); //Checks if we want to set a new default air value, by getting close to the distance sensor

  insertSensorData(); //Inserts the data to the database
  
  delay(1000);
}

//The algorithm if the air is polluted

void outBreakAlgorithm()
{
  MySQL_Cursor *cur_mem = new MySQL_Cursor(&conn);
    
  if(airQuality>=(defaultAirQuality+150))
  {
    if(dataBaseCounter==0)
    {
       //Updates the database, instead of creating new rows every second
       
       String query = "UPDATE `safebox`.`data` SET `OutBreak`=now() WHERE Id='1'";
       const char *q = query.c_str();                       
       cur_mem->execute(q);
       delete cur_mem;
    }

    analogWrite(motorPin,100);
    seconds++;
    digitalWrite(redLed,HIGH);
    digitalWrite(greenLed,LOW);
    dataBaseCounter++;
    delay(1000);
  }
  else if(seconds>0 && airQuality<(defaultAirQuality+100))
  {
    analogWrite(motorPin,0);
    digitalWrite(greenLed,HIGH);
    digitalWrite(redLed,LOW);
    displayEndurance=(seconds*2);
    
    String query = "UPDATE `safebox`.`data` SET `DateTime`=now(),`Endurance` = " + String(displayEndurance)+ ", `CurrentAirValue`= " + String(airQuality)+ " WHERE Id='1'";
    String query2 ="INSERT INTO `safebox`.`endurance`(`AggregatedEndurance`) VALUES (" + String(displayEndurance)+ ")";
    const char *q = query.c_str();
    const char *q2 = query2.c_str();
    cur_mem->execute(q);
    cur_mem->execute(q2);
    delete cur_mem;

    dataBaseCounter=0;
    seconds=0;
  }
}

//Function to insert the values that needs to be updated in the database every loop

void insertSensorData()
{
  MySQL_Cursor *cur_mem = new MySQL_Cursor(&conn);   
  String query = "UPDATE `safebox`.`data` SET `DateTime`=now(),`DefaultAirValue`= " + String(defaultAirQuality)+ ", `CurrentAirValue`= " + String(airQuality)+ " WHERE Id='1'";
  const char *q = query.c_str();                          // Konverterar en sträng till en char-array
  cur_mem->execute(q);                              // Utför vår query, och stoppar resultatet i cur_mem, en INSERT ger ingen data tillbaka...
  delete cur_mem;
}

//Checks if something is close to the distance sensor. If so, a new default airvalue is set

void distanceGenerator() {
  digitalWrite(triggerPin, LOW);
  delayMicroseconds(2);
  digitalWrite(triggerPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(triggerPin, LOW);
  
 const unsigned long duration= pulseIn(echoPin, HIGH);
 int distance= duration/29/2;
 
 if(distance<7)
 {
    digitalWrite(blueLed,HIGH);
    defaultAirQuality = airQuality;
    delay(5000);
    digitalWrite(blueLed,LOW);
 } 
 delay(100);
 }
