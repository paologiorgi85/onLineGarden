/*
  Running process using Process class. 

1. curl to identify the lenght of Json response
2. curl to retreive the Json response
3. print Json response
4. analyse the json response
5. Print sysdate
 This example code is in the public domain.

 */

#include <Process.h>
#include <ArduinoJson.h>

void setup() {
  // Initialize Bridge
  Bridge.begin();

  // Initialize Serial
  Serial.begin(9600);
  
  StaticJsonBuffer<200> jsonBuffer;
 
  // Wait until a Serial Monitor is connected.
  while (!Serial);

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

  for (JsonObject::iterator item = root.begin(); item !=root.end(); ++item)
  {
    Serial.print("Schedulazione ");
    Serial.print(item->key);
    Serial.println(": ");
    JsonObject& schedule = root[item->key].asObject();
    long hour = schedule["hour"];
    long minute = schedule["minute"];
    long time = schedule["time"];
    
    Serial.print("Start at: ");
    Serial.print(hour);
    Serial.print(":");
    Serial.print(minute);
    Serial.print(" - For: ");
    Serial.print(time);
    Serial.println(" sec");
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
    Serial.print("Actual hour: ");
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
