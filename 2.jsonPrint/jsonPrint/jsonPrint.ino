/******************************************************/
/* Retrieve the Json of scheduling from Firebase      */
/* using Process class.                               */
/*                                                    */
/* 1. curl to identify the lenght of Json response    */
/* 2. curl to retreive the Json response              */
/* 3. print Json response                             */
/* 4. analyse the json response                       */
/* 5. Print sysdate                                   */
/*                                                    */
/*  Note: this sketch start when the user enable      */
/*        serial monitor                              */
/*                                                    */
/* For this sketch is required only Arduino Yun,      */
/* connected to the WiFi. No additional fritzing      */
/* schema is required.                                */
/*                                                    */
/*  This example code is in the public domain.        */
/*   Author: P. Giorgi                                */
/* ****************************************************/

#include <Process.h>
#include <ArduinoJson.h>

/* Variables */
String URL = "https://";
String firebaseURL = "luminous-heat-4517.firebaseio.com"; /* Substitute with your firebase App*/

void setup() {
  // Initialize Bridge
  Bridge.begin();

  // Initialize Serial
  Serial.begin(9600);
  
  StaticJsonBuffer<200> jsonBuffer;
 
  // Wait until a Serial Monitor is connected.
  while (!Serial);
  
  URL.concat(firebaseURL);
  URL.concat("/scheduling.json");

/***************************************************/
/* 1. curl to identify the lenght of Json response */
/***************************************************/
  Process p;
  p.begin("curl");
  p.addParameter("-k");
  p.addParameter(URL);
  p.run();
  
  int i = 0;
  int jsonLength = 0;
  /* Find the lenght of Json object */
  while (p.available()>0) {
    char c = p.read();
    jsonLength++;
  }
  
  /* Create an array exact for the request */
  char json[jsonLength];

/*****************************************/
/* 2. curl to retreive the Json response */
/*****************************************/
  Process p1;
  p1.begin("curl");
  p1.addParameter("-k");
  p1.addParameter(URL);
  p1.run();
  
  while (p1.available()>0) {
    char c = p1.read();
    json[i] = c;
    i++;
  }
  
/**************************/
/* 3. print Json response */
/**************************/
  Serial.print("  1. Read Json: ");
  int a = 0;
  while (a < jsonLength) {
    Serial.print(json[a]);
    a++;
  }
  Serial.println();
  Serial.flush();
  
/********************************/
/* 4. analyse the json response */
/********************************/
  JsonObject& root = jsonBuffer.parseObject(json);
  
  /* json is like -->{"first":{"hour":9,"minute":20,"name":"first","time":60},"second":{"hour":10,"minute":30,"name":"second","time":30}} */

  if (!root.success()) {
    Serial.println("parseObject() failed");
    return;
  }

 /* Reset the number of scheduling */
  int numOfScheduling = 0;
  for (JsonObject::iterator item = root.begin(); item !=root.end(); ++item)
  {
    numOfScheduling++;
  }
  Serial.print("  2. Nunber of scheduling: ");
  Serial.println(numOfScheduling);
  
  for (JsonObject::iterator item = root.begin(); item !=root.end(); ++item)
  {
    Serial.print("     - Schedule name: ");
    Serial.print(item->key);
    Serial.print(" -> ");
    JsonObject& schedule = root[item->key].asObject();
    long hour = schedule["hour"];
    long minute = schedule["minute"];
    long time = schedule["time"];
    
    Serial.print(hour);
    Serial.print(":");
    Serial.print(minute);
    Serial.print(" for: ");
    Serial.print(time);
    Serial.println(" sec.");
  }
  
/********************************/
/*   5. Print sysdate           */
/********************************/
  Process date;
  int hours, minutes, seconds;
  int lastSecond = -1;
  date.begin("date");
  date.addParameter("+%T");
  date.run();
  if(date.available()>0) {
    String timeString = date.readString();
    int firstColon = timeString.indexOf(":");
    int secondColon= timeString.lastIndexOf(":");
    String hourString = timeString.substring(0, firstColon); 
    String minString = timeString.substring(firstColon+1, secondColon);
    String secString = timeString.substring(secondColon+1);
    Serial.print("  3. Hour: ");
    Serial.print(hourString);
    Serial.print(":");
    Serial.println(minString);
  } else {
    Serial.println("WARNING! Hour is not available.");
  }
}

void loop() {
  // Do nothing here.
}
