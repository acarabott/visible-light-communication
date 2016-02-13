/**
 * Simple Read
 *
 * Read data from the serial port and change the color of a rectangle
 * when a switch connected to a Wiring or Arduino board is pressed and released.
 * This example works with the Wiring / Arduino program that follows below.
 */


import processing.serial.*;

Serial myPort;  // Create object from Serial class
int val;      // Data received from the serial port

int historySize = 500;
int[] history = new int[historySize];
int idx = 0;

void setup()
{
  size(800, 400);
  // I know that the first port in the serial list on my mac
  // is always my  FTDI adaptor, so I open Serial.list()[0].
  // On Windows machines, this generally opens COM1.
  // Open whatever port is the one you're using.

  String portName = Serial.list()[6];

  myPort = new Serial(this, portName, 115200);

  println("all ports");
  for(int i=0; i < Serial.list().length; i++) {
    print(i);
    print(": ");
    println(Serial.list()[i]);
  }
  println("using port");
  println(portName);
}

void draw()
{
  if ( myPort.available() > 0) {  // If data is available,
    val = myPort.read();         // read it and store it in val
  }

  background(0);             // Set background to white
  int ptr = (historySize-1) - (idx % historySize);
  history[ptr] = val;
  idx++;

  fill(255);
  stroke(255);
  for(int i = 0; i < historySize; i++) {
    float x = map(i, 0, historySize, 0, width);
    // float x = map(ptr, 0, historySize, 0, width);
    // float x = width - map(i, 0, historySize, 0, width);
    float y = history[i] == 1 ? height * 0.5 : height * 0.4;
    point(x, y);
  }


}