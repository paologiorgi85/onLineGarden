/****************************************************************************************/
/*                SCHEDULE A JOB                                                        */
/*                                                                                      */
/* This sketch load all the scheduling that a resource had to perform.                  */
/* Each minute we check if the job had to start.                                        */ 
/* We use a Json object like this:                                                      */
/*  -> {"first":{"hour":9,"minute":20,"name":"first","time":60}, ...} <-                */
/*  hour: hour that the job had to start                                                */
/*  minute: the miunte that the job had to start                                        */
/*  time: duration (in second) that the job had to go on                                */
/*                                                                                      */
/* Each day it reload the planned scheduling in order to retrieve all possible updates. */
/* For this reason, please do not insert any schedule at hour: 00:00.                   */
/*                                                                                      */
/* This example code is in the public domain.                                           */
/* Athor: P. Giorgi                                                                     */
/****************************************************************************************/

/* Fix: la prima chimata al metodo refreshScheduling() funziona
/*      la seconda e le successive restituiscono errore:
/*          "parseObject() failed"  
/*               CONTROLLARE!!!                       */

#include <Process.h>
#include <ArduinoJson.h>

StaticJsonBuffer<200> jsonBuffer;
  
// Variables
int numOfScheduling = 0;
int hours, minutes, seconds;
String hourString;
String minString;
String secString;
long* hour = 0;
long* minute = 0;
long* time = 0;

void setup() {
  // Initialize Bridge
  Bridge.begin();

  // Initialize Serial
  Serial.begin(9600);
  
  delay(10000);
 
  // Inizialize Scheduling
  refreshScheduling();

}

void loop() {
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
    Serial.print("Actual hour: ");
    Serial.print(hourString);
    Serial.print(":");
    Serial.println(minString);
    if(hourString.equals("00") && minString.equals("00") ) {
      Serial.println("It's midnight. Recalculare the Scheduling!");
      refreshScheduling();
    } else {
      Serial.println("It's not midnight. Wait a scheduling.");
    }
  } else {
    Serial.println("WARNING! Hour is not available.");
  }
}

void refreshScheduling() {
/***************************************************/
/* 1. curl to identify the lenght of Json response */
/***************************************************/
  Process p;
  p.begin("curl");
  p.addParameter("-k");
  p.addParameter("https://luminous-heat-4517.firebaseio.com/scheduling.json");
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
  p1.addParameter("https://luminous-heat-4517.firebaseio.com/scheduling.json");
  p1.run();
  
  while (p1.available()>0) {
    char c = p1.read();
    json[i] = c;
    i++;
  }

/**************************/
/* 3. print Json response */
/**************************/
  Serial.println("--> Start read <--");
  int a = 0;
  while (a < jsonLength) {
    Serial.print(json[a]);
    a++;
  }
  Serial.println();
  Serial.println("--> End read <--");
  Serial.flush();
  
/********************************/
/* 4. analyse the json response */
/********************************/
  JsonObject& root = jsonBuffer.parseObject(json);
  
  /* json is like -->{"prima":{"hour":9,"minute":20,"name":"prima","time":60},"seconda":{"hour":10,"minute":30,"name":"seconda","time":30}} */
  
  if (!root.success()) {
    Serial.println("parseObject() failed");
    return;
  }
  
  //Reset the number of scheduling
  numOfScheduling = 0;
  for (JsonObject::iterator item = root.begin(); item !=root.end(); ++item)
  {
    numOfScheduling++;
  }
  Serial.print("Nunber of scheduling: ");
  Serial.println(numOfScheduling);
  if (hour != 0) {
    delete [] hour;
    delete [] minute;
    delete [] time;
  }
  hour = new long [numOfScheduling];
  minute = new long [numOfScheduling];
  time = new long [numOfScheduling];
  int z = 0;
  for (JsonObject::iterator item = root.begin(); item !=root.end(); ++item)
  {
    Serial.print("Schedule name: ");
    Serial.print(item->key);
    Serial.println("-> ");
    JsonObject& schedule = root[item->key].asObject();
    hour[z] = schedule["hour"];
    minute[z] = schedule["minute"];
    time[z] = schedule["time"];
    z++;
  }
  Serial.println(" Wait 50sec.");
  delay(50000);
  Serial.println(" Finish Wait.");
}


