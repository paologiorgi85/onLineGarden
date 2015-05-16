
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
/* To reload the scheduling it's also possible to use the reset button.                 */
/*                                                                                      */
/* The scheduling are retrieved from a Real time DataBase (Firebase) by Json objcet     */
/*                                                                                      */
/* This example code is in the public domain.                                           */
/* Athor: P. Giorgi                                                                     */
/****************************************************************************************/


#include <Process.h>
#include <ArduinoJson.h>
  
/* Variables */
String URL = "https://";
String firebaseURL = "luminous-heat-4517.firebaseio.com"; /* Substitute with your firebase App*/
int numOfScheduling = 0;
int hours, minutes, seconds;
String hourString;
String minString;
String secString;
String lastMinute = "MM";
/* Array that store the scheduling */
long* hour = 0;
long* minute = 0;
long* time = 0;
/* Define pin */
int pinLed          = 12;
int pinLedReset     = 13;
int valve           =  4;
int pinButtonStart  =  5;
int pinButtonReset   = 7;
int buttonState     = 0;
int buttonStart     = 0;

void setup() {
  /* Initialize Bridge */
  Bridge.begin();

  /* Initialize Serial */
  Serial.begin(9600);
  delay(10000);
  URL.concat(firebaseURL);
  URL.concat("/scheduling.json");
  
  /* Configuration I/0 */
  pinMode(pinLed, OUTPUT);
  pinMode(pinLedReset,OUTPUT);
  pinMode(pinButtonStart, INPUT);
  pinMode(pinButtonReset, INPUT);
  pinMode(valve, OUTPUT);
  pinMode(pinButtonReset, INPUT);
  digitalWrite(pinButtonReset, HIGH);
  
  /* Turn Off the valve */
  closeValve();
  
  /* Inizialize Scheduling */
  refreshScheduling();
}

void refreshScheduling() {
  
  lastMinute = "MM";
  
  Serial.println();
  Serial.println("--> Refresh of Scheduling <--");
  digitalWrite(pinLedReset, HIGH);
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
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(json);
  /* json example: {"first":{"hour":9,"minute":20,"name":"first","time":60},"second":{"hour":10,"minute":30,"name":"second","time":30}} */
  
  if (!root.success()) {
    Serial.println("parseObject() failed");
    return;
  }
  
  /* Reset the number of scheduling */
  numOfScheduling = 0;
  for (JsonObject::iterator item = root.begin(); item !=root.end(); ++item)
  {
    numOfScheduling++;
  }
  Serial.print("  2. Nunber of scheduling: ");
  Serial.println(numOfScheduling);
  if (hour != 0) {
    delete [] hour;
    delete [] minute;
    delete [] time;
  }
  hour           = new long [numOfScheduling];
  minute         = new long [numOfScheduling];
  time           = new long [numOfScheduling];
  int z = 0;
  for (JsonObject::iterator item = root.begin(); item !=root.end(); ++item)
  {
    Serial.print("     - Schedule name: ");
    Serial.print(item->key);
    Serial.print(" -> ");
    JsonObject& schedule = root[item->key].asObject();
    hour[z]           = schedule["hour"];
    minute[z]         = schedule["minute"];
    time[z]           = schedule["time"];
    Serial.print(hour[z]);
    Serial.print(":");
    Serial.print(minute[z]);
    Serial.print(" for ");
    Serial.print(time[z]);
    Serial.println(" sec.");
    z++;
  }
  Serial.println("  3. Wait 2sec.");
  delay(2000);
  Serial.println("  4. Finish Wait.");
  digitalWrite(pinLedReset, LOW);
  Serial.println("--> Refresh of Scheduling Complete <--");
  Serial.println();
}

void loop() {
  buttonState = digitalRead(pinButtonReset);  // read input buttonResetSchedule
  buttonStart = digitalRead(pinButtonStart);  // read input pinButtonStart
  
  /* Check if the reset button is pressed. */
  if (buttonState == HIGH) {
    refreshScheduling();
  } else if (buttonStart == 1) {
    /* Manage by fisical button */
    openValve();
    while (digitalRead(pinButtonStart) == 1) {
      delay(500);
    }
    closeValve();
    delay(1000);
  } else {
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
      if(minString.equals(lastMinute)) {
      /* Check if the control of scheduling has been already performed for this hour (HH:MM) */
        //Serial.print("It's the same minute (MM-> ");
        //Serial.print(lastMinute);
        //Serial.println("). Wait 10 sec.");
        delay(500);
      } else {
        Serial.print("--> Check at: ");
        Serial.print(hourString);
        Serial.print(":");
        Serial.println(minString);
        if(hourString.equals("00") && minString.equals("00") ) {
          /* If it's midnight, recalculate the list of scheduling */
          Serial.println("  It's midnight. Recalculare the Scheduling!");
          refreshScheduling();
        } else {
          /* Check if a scheduling had to start. */
          /* If more than one schedule start, the longer wins. So the duration (sec) is simply calulated with the longest one */
          boolean startIrrigation = false;
          long    duration = 0;
          for (int i = 0; i < sizeof(hour); i++){
            ///Serial.print("Schedule");
            //Serial.print(i);
            //Serial.print(": ");
            //Serial.print(String(hour[i]));
            //Serial.print(":");
            //Serial.println(String(minute[i]));
            String hourScheduling = String(hour[i]);
            String minuteScheduling = String(minute[i]);
            String valueZero = "0";
            /* Fix on format of hour and minute */
            if (hourScheduling.length() == 1) {
              valueZero.concat(hourScheduling);
              hourScheduling = valueZero;
            }
            valueZero = "0";
            if (minuteScheduling.length() == 1) {
              valueZero.concat(minuteScheduling);
              minuteScheduling = valueZero;
            }
            if(hourString.equals(hourScheduling) && minString.equals(minuteScheduling) ) {
              startIrrigation = true;
              if(time[i] > duration) {
                duration = time[i];
              }
            }
          }
          if(startIrrigation) {
            Serial.print("  Start Irrigation for ");
            Serial.print(duration);
            Serial.println(" sec. !");
            openValve();
            delay(duration*1000);
            closeValve();
          } else {
            Serial.println("  No irrigation scheduled for this time.");
            delay(500);
          }
        }
      }
      lastMinute = minString;
    } else {
      Serial.println("WARNING! Hour is not available.");
    }
  }
}

void openValve() {
  digitalWrite(pinLed, HIGH);
  digitalWrite(valve, HIGH);
  Serial.println("    Open valve");
}

void closeValve() {
  digitalWrite(pinLed, LOW);
  digitalWrite(valve, LOW);
  Serial.println("    Close valve");
}
