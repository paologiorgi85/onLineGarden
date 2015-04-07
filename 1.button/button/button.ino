/******************************************************/
/*                                                    */
/* This sketch mange the water valve with a button or */
/* by URL.                                            */
/*                                                    */
/* This example code is in the public domain.         */
/* Athor: P. Giorgi                                   */
/******************************************************/

/* Include libraries */
#include <Bridge.h>
#include <YunServer.h>
#include <YunClient.h>

/* Create Yun server */
YunServer server;

/* Define pin and variables */
int pinLed        = 13;
int valve         =  4;
int botton        =  7;
int statusLed     = LOW;

void setup() {
  /* Initialize serial connection */
  Serial.begin(115200);
  
  /* Start bridge */
  Bridge.begin();
  
  /* Start server */
  server.listenOnLocalhost();
  server.begin();
  
  /* Configuration I/0 */
  pinMode(pinLed, OUTPUT);
  pinMode(valve, OUTPUT);
  pinMode(botton, INPUT);
  
  /* Turn Off the valve */
  Serial.print("Setup operation");
  closeValve();
}
  
void loop() {
  /* Click button */
  int readButtonValue = digitalRead(botton);   
  if (readButtonValue == 1) {
    /* Managed by fisical button */
     if (statusLed == LOW) {
       statusLed=HIGH;
       openValve();  
     } else {
       statusLed=LOW;
       closeValve();
     }
     delay(1000);
  } else {
    /* Managed by Web */
    /* Get clients coming from server */
    YunClient client = server.accept();
    
    /* There is a new client? */
    if (client) {
      /* Process request */
      process(client);
      
      /* Close connection and free resources. */
      client.stop();
    }
  }
  
  // Poll every 50ms
  delay(50);
}

void openValve() {
  digitalWrite(pinLed, HIGH);
  digitalWrite(valve, HIGH);
  Serial.print("Open valve");
}

void closeValve() {
  digitalWrite(pinLed, LOW);
  digitalWrite(valve, LOW);
  Serial.print("Close valve");
}
      
/* Process incoming command */
void process(YunClient client) {
  // read the command
  String command = client.readStringUntil('/');
  
  /* is "digital" command */
  if (command == "digital") {
    digitalCommand(client);
  }
}
  
void digitalCommand (YunClient client) {
  int pin, value;
  // Read pin number
  pin = client.parseInt();
  
  /* If the next character is a '/' it means we have an URL
     with a value like: "/digital/13/1" */
  if (client.read() == '/') {
    value = client.parseInt();
    if(pin == 4 && value == 1) {
      openValve();
    } else if(pin == 4 && value == 0) {
      closeValve();
    } else {
      digitalWrite(pin, value);
    }
  }
  else {
    value = digitalRead(pin);
  }
}
