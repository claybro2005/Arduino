import org.gamecontrolplus.gui.*;
import org.gamecontrolplus.*;
import net.java.games.input.*;
import processing.serial.*;

//Serial myPort;
ControlIO control;
ControlButton laser;
ControlButton run_prev;
Configuration config;
ControlDevice gpad;

int turretPosX, turretPosY;
int bg_int = 0;
float leftTriggerMultiplier, leftTriggerTolerance, leftTriggerTotalValue;
float rightTriggerMultiplier, rightTriggerTolerance, rightTriggerTotalValue;

float leftX = 0;
float leftY = 0;
float rightY = 0;
byte lOutput = 0;
byte rOutput = 0;
byte lDir = 0;
byte rDir = 0;

// Used for timing since Processing has no real timers.
int previousMillis = 0;
int interval = 300; // 300 milliseconds

// Save previous states. If there is no change from the input
// just use the previous values.
float lPrevious;
float rPrevious;

float angle;
float magnitude;
float output;
float tolerance = 0.17;  // set tolerance value for joysticks
float maxSpeed = 127;

int mode = 0;
boolean selectSet = false; // for reading the state of the select button
boolean startSet = false; // for reading the state of the start button
boolean ySet = false;
boolean xSet = false;
boolean DEBUG = false;

public void setup() {
  size(400, 400);
  //println(Serial.list());
  // Initialise the ControlIO
  control = ControlIO.getInstance(this);
  // Find a device that matches the configuration file
  //gpad = control.getDevice("XBOX 360 Wired Controller [Gamepad]");
  gpad = control.getMatchedDevice("gamepad");
  if (gpad == null) {
    println("No suitable device configured");
    System.exit(-1); // End the program NOW!
  }
  else{
    println("gpad config file found");
    println(gpad);  
  }
  laser = gpad.getButton("laser");

}

public void draw(){
  background(255, 200, 255);
  int leftLine = width*5/16, rightLine = width*11/16;


  //run_prev = gpad.getButton("run_prev");
  turretPosX =  int(map(gpad.getSlider("XPOS").getValue(), -1, 1, 0,180));
  turretPosY =  int(map(gpad.getSlider("YPOS").getValue(), -1, 1, 0,180));
  
  if (laser.pressed()){
    if(bg_int==0){
      fill(0,0,0);
      bg_int =1;
      println("laser on");
    }
    else{
      fill(255,0,0);
      bg_int=1;
      println("laser Off");
    }
  }
  
  
  //-------------------  Draw stuff here -------------//
  rectMode(CENTER);

  stroke(0);
  fill(154, 154, 154); // light grey
  
  // draw sliders
  rect(leftLine, width/2, 256, 20);
  rect(rightLine+60, height/2, 20, 256); 
  
  // draw center lines
  line(height/2, leftLine , height/2, leftLine + 40);
  line(rightLine, height/2, rightLine + 100, height/2);
  
  // draw maxSpeed stops
  line(leftLine-20, height/2-maxSpeed-1, leftLine+20, height/2-maxSpeed-1);
  line(leftLine -20, height/2+maxSpeed+1, leftLine+20, height/2+maxSpeed+1);
  line(rightLine-20, height/2-maxSpeed-1, rightLine+20, height/2-maxSpeed-1);
  line(rightLine -20, height/2+maxSpeed+1, rightLine+20, height/2+maxSpeed+1);
  
  // draw rectangles for sticks
  if(mode == 0 && laser.pressed()) {
    fill(255,0,0); // red
    rect(leftLine, height/2 - turretPosX, 20, 40);
  }
  else {
    fill(0);
    rect(leftLine, height/2 - turretPosX, 20, 40);
  }

  if( mode == 0 && laser.pressed()) {
    fill(255,0,0); // red
    rect(rightLine, height/2 - turretPosY, 40, 20);
  }
  else {
    fill(0);
    rect(rightLine, height/2 - turretPosY, 40, 20);
  }
                 
  fill(0);
  
  textAlign(RIGHT, CENTER);
  text(int(leftY), leftLine - 50, height/2);
  textAlign(LEFT, CENTER);
  text(int(rightY), rightLine + 50, height/2);
  
  textAlign(CENTER, BASELINE);
  text("Output: " + binary(lOutput,8),leftLine, height/2 - 150);
  text("Output: " + binary(rOutput,8),rightLine, height/2 - 150);
 
  textAlign(CENTER, TOP);
  text("L", leftLine, height/2 + 140);
  text("R", rightLine, height/2 + 140); 
  
  if(DEBUG == false)
  {
  textAlign(CENTER, BASELINE);
  if(mode == 0)
    text("Skid Steer", width/2, 20);
  else if (mode == 1)
    text("Single Stick Mode", width/2, 20);

  }
  
  textAlign(LEFT);
  text("mode: " + mode, 20, 20);
  
  // display extra info
  if(DEBUG == true) {
    textAlign(LEFT, BASELINE);
    text("Angle: " + degrees(angle), 20, height/2 + 170);
    text("Magnitude: " + magnitude, 200, height/2 + 170);
    text("Output: " + output, 20, height/2 + 190);
    text("MaxSpeed: " + nf(maxSpeed,0,2), 200, height/2 + 190);
  }
   

  //println(turretPosX);
  //println(turretPosY);
  //println(laser);
  //println(run_prev);
}