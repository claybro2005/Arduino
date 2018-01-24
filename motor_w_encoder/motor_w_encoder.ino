/*   12/16/2017 read values and write to motor control 

Ver 1, Clayton Broman 
Program looks for a \n or \r at the end of all commands, lower case must be used throughout
format is function,axis,type (rel, abs),value 
Support only for a single axis to be called at a time
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


int pos;
int in1 = 3;
int in2 = 5;
int in3 = 6;
int in4 = 7;
int ind = 0;
int axis_a,axis_b;
char strVal[10];
boolean newData= false;
String type ="";
char func;
bool func_f, axis_f, minus_f, type_f, val_f, abs_f,rel_f, error,move_f,pitch_f,roll_f,yaw_f, recv_f;  //flags to see what part of a command is recieved
long int count_pitch = 0; //starts at 0 if the arduino is turned off (no initialize possible) 
long int count_yaw = 0;
long int count_roll = 0;


void setup() {

  /* Setup encoder pins as inputs */
  pinMode(ENC_A, INPUT);
  digitalWrite(ENC_A, HIGH);
  pinMode(ENC_B, INPUT);
  digitalWrite(ENC_B, HIGH);

  /* setup the motor pins as outputs*/
  pinMode(in1,OUTPUT);
  pinMode(in2,OUTPUT);
  pinMode(in3,OUTPUT);
  pinMode(in4,OUTPUT);

  /* Startup serial */
  Serial.begin(9600);
  Serial.println("<Arduino is ready>");

}

void loop() {

   //read serial 
   recv();
   //move to the right or left until you get to the right spot 
   //if(error){
   //   Serial.println("Not sure what that means");
      //clear buffer and listen
   //}
   //else{
   if(recv_f){
       execute();
   }
   //count
   //stop
   //reset the boolean variables for next command
   clr_var(strVal);
   ind =0;
   func_f = false; axis_f=false; type_f=false; val_f=false; rel_f=false; abs_f=false; error = false; move_f = false; rel_f = false; recv_f = false;
   pitch_f = false; yaw_f = false; roll_f = false;
}

void recv() {
   while(Serial.available()){
      recv_f = true;
      delay(100);
      char ch = Serial.read();
      //*** check for delimiter or end of line   ///////////////////////////
      if(ch == ',' || ch ==' ' ||ch == '\n' || ch == '\r'){
           //Serial.print("recieved command: ");
           ch = '\0';
           if( (func_f == true && axis_f == true) && (type_f == true && val_f ==false)){      // check for VALUE       
                pos = atoi(strVal);
                clr_var(strVal);
                val_f = true;
                ind = -1; //reset buffer indices
           }
           else if((func_f == true && axis_f == true) && (type_f == false && val_f == false)){    // check for TYPE
                if(strcmp(strVal, "abs")==0){
                    abs_f = true;            
                }
                else if(strcmp(strVal,"rel")==0){
                    rel_f = true;
                }
                else{
                    Serial.print("Unknown type of movement: ");
                    Serial.println(strVal);
                    error = true;
                }
                clr_var(strVal);
                type_f = true;
                ind = -1;
           }
           else if((func_f == true && axis_f == false) && (type_f ==false && val_f ==false)){          // check for AXIS
               Serial.print("got an axis:");
               Serial.print(strVal);
               Serial.println(":");
               if(strcmp(strVal,"pitch")==0){
                   axis_a = in1; // about the y-axis
                   axis_b = in2;
                   pitch_f = true;
                }
                else if(strcmp(strVal,"yaw")==0){
                    axis_a = in3; //about the z-axis
                    axis_b = in4;
                    yaw_f = true;
                    }
                else if(strcmp(strVal,"roll")==0){
                    axis_a = 1; //about the x-axis, this will be a stepper motor, figure out later...
                    roll_f = true;
                    //if pos or reset break and go do that
                }
                else{
                    Serial.println("error unknown axis");
                    error - true;
                }
                ind=-1;
                axis_f = true;
                clr_var(strVal);
           }
                                                    
           else if((func_f == false && axis_f == false) && (type_f ==false && val_f ==false)){                   // check for FUNCTION
              //Serial.print("got a function:");
              //Serial.print(strVal);
              //Serial.println(":");
              
              if(strcmp(strVal, "mv") == 0){
                   func_f = true; 
                   func = 'm';
                   }
              else if(strcmp(strVal,"idn?")==0){
                   Serial.print("Arduino, FA Positioner, Ver1");
                   //break out of this
                   break;
              }
              else if(strcmp(strVal,"help")==0){
                   Serial.print("Help!");
                   //break out of this
                   break;
              }
              else if(strcmp(strVal,"rst")==0){
                  // set rst to true
                  func == 'r';
                  Serial.println("Recv command recv'd");
              }
              else if(strcmp(strVal,"pos")==0){
                  //set pos to true
                  func = 'p';
                  Serial.println("You want the pos? You cant handle the POS!");
              }
              else{
                  //err(unknown function", strVal )
                  Serial.println("Sorry, come again?");
                  error = true;
              }
          func_f = true;
          ind =-1;
          clr_var(strVal);
          }
       } // end of delimiter
   strVal[ind++] = ch;
   } //end of while loop
 
}// end rcv_func

void execute(){
   switch(func){
      case 'm':
         if(yaw_f){
             count_yaw = go_to(count_yaw); 
             Serial.print("Success, new yaw pos: ");
             Serial.println(count_yaw);
         }
         else if(roll_f){
             count_roll = go_to(count_roll);
             Serial.print("Success, new roll pos: ");
             Serial.println(count_roll);
         }
         else if(pitch_f){
             count_pitch = go_to(count_pitch);
             Serial.print("Success, new pitch pos: ");
             Serial.println(count_pitch);
         }
         break;//mv,pitch,rel,50
         
      case 'p':
         if(yaw_f){
            Serial.print("Yaw axis at: ");
            Serial.println(count_yaw);
         }
         else if(pitch_f){
            Serial.print("Pitch axis at: ");
            Serial.println(count_pitch);
         }
         else if(roll_f){
            Serial.print("Roll axis at: ");
            Serial.println(count_roll);
         }
         break;
      case 'r':
         if(yaw_f){
            count_yaw =0;
            Serial.print("Yaw axis reset to: ");
            Serial.println(count_yaw);
         }
         else if(pitch_f){
            count_pitch = 0;
            Serial.print("Pitch axis reset to: ");
            Serial.println(count_pitch);
         }
         else if(roll_f){
            count_roll = 0;
            Serial.print("Roll axis reset to: ");
            Serial.println(count_roll);
         }
         break;
         
   }
}

long int go_to(long int axis ){
    //three cases pitch, roll, yaw
    bool is_big,is_small;  
    // convert rel to abs
    if(rel_f){
       pos = axis + pos;
    }
    while(axis != pos){
       Serial.print("Axis at: ");
       Serial.print(axis);
       Serial.print("   :Position at: ");
       Serial.println(pos);
       if(axis < pos){
           axis = forward(axis);
       }
       if(axis > pos){
           axis =reverse(axis);
       }
       delay(100);
    }

    Serial.println(pos);
    return axis;
}

long int forward(long int axis){
    int8_t tmp_data;
    digitalWrite(axis_a,LOW); // stop just in case it was going the other way it wont lurch
    digitalWrite(axis_b,LOW);
    delay(100);       
    digitalWrite(axis_a,HIGH); //forward
    digitalWrite(axis_b,LOW);
    Serial.println("turning Forward");
    while(axis < pos){
        tmp_data = read_encoder();
        if(tmp_data){
           axis += tmp_data;
           Serial.print("axis at: ");
           Serial.println(axis);
        } 
    }
    stop();
    return axis;
}
long int reverse(long int axis){
    int8_t tmp_data;
    digitalWrite(axis_a,LOW); // stop just in case it was going the other way it wont lurch
    digitalWrite(axis_b,LOW);
    delay(100);
    digitalWrite(axis_a,LOW);
    digitalWrite(axis_b,HIGH); //reverse
    Serial.println("Reverse");
    while(axis>pos){
        tmp_data = read_encoder();
        if(tmp_data){
           axis += tmp_data;
           Serial.print("axis at: ");
           Serial.println(axis);
        } 
    }
    stop();
    return axis;
}
void stop(){
    digitalWrite(axis_a,LOW); //stop
    digitalWrite(axis_b,LOW);
    Serial.println("Stopping");
}
 
/* returns change in encoder state (-1,0,1) */
int8_t read_encoder(){
  static int8_t enc_states[] = {0,-1,1,0,1,0,0,-1,-1,0,0,1,0,1,-1,0};
  static uint8_t old_AB = 0;
  /**/
  old_AB <<= 2;                   //remember previous state
  old_AB |= ( ENC_PORT & 0x03 );  //add current state
  return ( enc_states[( old_AB & 0x0f )]);
}

void clr_var(String val){
   for( int i = 0; i < sizeof(val);  ++i )
      strVal[i] = '\0';
}
