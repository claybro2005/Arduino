/*   12/16/2017 read values and write to motor control 

Ver 1, Clayton Broman 


common commands: 
IDN?, returns, Arduino, the version of this firmware and 
help, returns a list of commands

move(type, axis, value), 
    variables: 
    type = rel or abs. Description: relative motion with respect to current position, or absolute goes to a location.
    axes = pitch, roll, yaw. Description: yaw- rotation around the z-axis (fiber Array alignment)
           pitch- rotation about the y-axis (change the peak wavelength)
           roll - rotation about the x-axis (make FA planar to wafer surface)
    value = integer value, 1 step ~ .03 degrees rotation. 

reset(axis) : set the counter for that axis back to zero 
pos(axis)   : returns the current position of the given axis

*/

char val = 0;
int dir;
int in1 = 3;
int in2 = 5;
int inc = 0;
boolean newData= false;


void setup() {
 Serial.begin(9600);
 Serial.println("<Arduino is ready>");

 pinMode(in1,OUTPUT);
 pinMode(in2,OUTPUT);
 

 
}

void loop() {
 recvOneChar();
 showNewData();
}

void recvOneChar() {
 if (Serial.available() > 0){
      
     delay(3);
     val = Serial.read();
     if (isDigit(val)){
        dir = int(val);
        }
     if( val == '\n'){
        Serial.println(dir);   
        }
     newData = true;
   }
}

void showNewData() {
 if (newData == 1) {
   if(dir=='1'){
      digitalWrite(in1,LOW); // stop just in case it was going the other way it wont lurch
      digitalWrite(in2,LOW);
      delay(100);              
      digitalWrite(in1,HIGH); //forward
      digitalWrite(in2,LOW);
      Serial.println("Forward");
   }                       
   else if(dir == '2'){
      digitalWrite(in1,LOW); // stop just in case it was going the other way it wont lurch
      digitalWrite(in2,LOW);
      delay(100);
      digitalWrite(in1,LOW);
      digitalWrite(in2,HIGH); //reverse
      Serial.println("Reverse");
   }
   else{
      digitalWrite(in1,LOW); //stop
      digitalWrite(in2,LOW);
   }
 newData = false;
 }
}
