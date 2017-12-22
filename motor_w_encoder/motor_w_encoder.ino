/*   12/16/2017 read values and write to motor control 

Ver 1, Clayton Broman 
Program looks for a \n or \r at the end of all commands, lower case must be used throughout, function,axis,
format is function,axis,type (rel, abs),value 
common commands: 
IDN?, returns, Arduino, the version of this firmware and 
help, returns a list of commands

move: mv, axis=var, type, value=var 
    variables: 
    type = rel or abs. Description: relative motion with respect to current position, or absolute goes to a location.
    axes = pitch, roll, yaw. Description: yaw- rotation around the z-axis (fiber Array alignment)
           pitch- rotation about the y-axis (change the peak wavelength)
           roll - rotation about the x-axis (make FA planar to wafer surface)
    value = integer value, 1 step ~ .03 degrees rotation. 
    
    Example: mv,rel,pitch,-10\n\r
    Returns: pitch -27   (given that the previous location was -17)
     
reset: rst,axis : set the counter for that axis back to zero
       Example: reset,pitch\n\r         returns: pitch 0
     
position: pos(axis): returns the current position of the given axis
    Example: pos,pitch\n\r
    Returns: pitch -100


*/


#define ENC_A 14
#define ENC_B 15
#define ENC_C A0
#define ENC_D A1
#define ENC_PORT PINC


long int pos;
int in1 = 3;
int in2 = 5;
int in3 = 6;
int in4 = 7;
int ind = 0;
int axis_a,axis_b;
char strVal[10];
boolean newData= false;
String func = "", type ="", axis="" ;
bool func_f, axis_f, minus_f, type_f, val_f, abs_f,rel_f, error;  //flags to see what part of a command is recieved
int count_pitch = 0; //starts at 0 if the arduino is turned off (no initialize possible) 
int count_yaw = 0;
int count_roll = 0;


void setup() {

  /* Setup encoder pins as inputs */
  pinMode(ENC_A, INPUT);
  digitalWrite(ENC_A, HIGH);
  pinMode(ENC_B, INPUT);
  digitalWrite(ENC_B, HIGH);

  /* setup the motor pins as outputs*/
  pinMode(in1,OUTPUT);
  pinMode(in2,OUTPUT);
  //pinMode(in3,OUTPUT);
  //pinMode(in4,OUTPUT);

  /* Startup serial */
  Serial.begin(9600);
  Serial.println("<Arduino is ready>");

}



void loop() {

   //read serial 
   recv();
   //reset the variable flags
   func_f = false; axis_f=false;
   //move to the right or left until you get to the right spot 

   if(error){
      //clear buffer and listen
   }
   go(minus_f,pos);
   //count
   //stop   
   
}

void recv() {
 while(Serial.available()){  
     delay(3);
     char ch = Serial.read();
     strVal[ind++] = ch;
     //*** check for delimiter or end of line   ///////////////////////////
     if(ch == ',' || ch =='\n' || ch=='\r'){
           if( (func_f == true && axis_f == true) && (type_f == true && val_f ==false)){      // check for VALUE  
                
                pos = atoi(strVal);
                clr_var(strVal);
                val_f = true;
                ind = 0;
           }
           else if((func_f == true && axis_f == true) && (type_f == false && val_f == false)){    // check for TYPE
                if(strVal == "abs"){
                    abs_f = true;            
                }
                else if(strVal == "rel"){
                    rel_f = true;
                }
                else{
                    Serial.println("Unknown type of movement");
                    error = true;
                }
                clr_var(strVal);
                type_f = true;
                ind = 0;
           }
           else if((func_f == true && axis_f == false) && (type_f ==false && val_f ==false)){          // check for AXIS
               if(strVal == 'pitch'){
                   axis_a = in1; // about the y-axis
                   axis_b = in2;
                   //if pos or reset break and go do that
                }
                else if(strVal == 'yaw'){
                    axis_a = in3; //about the z-axis
                    axis_b = in4;
                    //if pos or reset break and go do that
                    }
                else if(strVal == 'roll'){
                    axis = "X"; //about the x-axis
                
                    //if pos or reset break and go do that
                }
                else{
                    Serial.println("error unknown axis");
                    error - true;
                }
                ind=0;
                axis_f = true;
                clr_var(strVal);
           }
                                                    
           else if((func_f == false && axis_f == false) && (type_f ==false && val_f ==false)){                                                                // check for FUNCTION
              if(strVal == 'mv' ){         
                   ///move
                   func_f = true; 
                   func = "move";
                   }
              else if(strVal == 'idn?'){
                   Serial.print("Arduino, FA Positioner, Ver1");
                   //break out of this
                   func_f = true;
                   func = "idn";
              }
              else if(strVal == 'help'){
                   Serial.print("Help");
                   //break out of this
                   func_f = true;
                   func = "help";
              }
              else if(strVal == 'rst'){
                  // set rst to true
                  Serial.println("Recv command recv'd");
              }
              else if(strVal == "pos"){
                  //set pos to true
                  Serial.println("You want the pos? You cant handle the POS!");
              }
              else{
                  //err(unknown function", strVal )
                  Serial.println("Sorry, come again?");
                  error = true;
              }
              func_f = true;
              clr_var(strVal);
          }
          if((func == "move" && func_f ==true) && (type== false && axis== false)){
             
              Serial.println('ok now what?');
          }
     
       }
    }
}

void forward(int axis){
    digitalWrite(axis,LOW); // stop just in case it was going the other way it wont lurch
    digitalWrite(axis,LOW);
    delay(100);              
    digitalWrite(axis,HIGH); //forward
    digitalWrite(axis,LOW);
    Serial.println("turning Forward");
}
void reverse(){
    digitalWrite(in1,LOW); // stop just in case it was going the other way it wont lurch
    digitalWrite(in2,LOW);
    delay(100);
    digitalWrite(in1,LOW);
    digitalWrite(in2,HIGH); //reverse
    Serial.println("Reverse");
}
void stop(){
    digitalWrite(in1,LOW); //stop
    digitalWrite(in2,LOW);
}
void go(bool dir, int pos) {

}

void clr_var(String val){
   for( int i = 0; i < sizeof(val);  ++i )
      val[i] = '\0';
}
 
/* returns change in encoder state (-1,0,1) */
int8_t read_encoder(){
   static long int counter = 0;      //this variable will be changed by encoder input
   int8_t tmpdata;
   /**/
   tmpdata = read_encoder();
   if( tmpdata ) {
      counter += tmpdata;
   }

   static int8_t enc_states[] = {0,-1,1,0,1,0,0,-1,-1,0,0,1,0,1,-1,0};
   static uint8_t old_AB = 0;
   /**/
   old_AB <<= 2;                          //remember previous state
   old_AB |= ( ENC_PORT & 0x03 );         //add current state
   return ( enc_states[( old_AB & 0x0f )]);
}
