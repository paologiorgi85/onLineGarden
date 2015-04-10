## onLineGarden ##


This Repo help you to manage your irrigation system.

We start with simple examples and my expectations are to grow up and realize a complete system:

* automatic
* easy to use
* customizable

### What is this repository for? ###

The Arduino sketch are:

1. button: start the irrigation with a button plugged in the Arduino board.
2. jsonPrint: retrieve and print the Json stored on Firebase.
3. scheduling: retrieve and start the irrigation using the scheduling stored on Firebase.
4. improvements: add some improves to the sketch, like reset button, double water valve, etc...

Sketchs number 2, 3 and 4 nead a Real Time DataBase (Firebase).
To create and manage this DataBase I created a simple Web Application with AngulaJS (folder "web_app").

I provide: 
 * Test istance of Firebase: https://luminous-heat-4517.firebaseio.com/
 * Web App: http://gardenduino.altervista.org/app/index.html

No login are required for this two resources. Use it only as example.

### How do I get set up? ###

Start from the first example and proceed with the next.
Copy the folder into your Arduino library and start to modify and contribute to the Repo.
Into each folder there are a sketch and all the schema, realized with Fritzing, to reproduce the circuit.

The understanding is easy and there are many ways to improve these sketchs.

I design the circuit with Fritzing.
Construct the circuit with the example images present into the folter "prototypes_Fritzing".

### Contribution guidelines ###

* Writing tests
* Code review
* Other guidelines

### Who do I talk to? ###

I'm Paolo Giorgi and in free time I try to simplify my life.

Arduino helps me but I hope that someone else does it with his own contribute to this Repo.
