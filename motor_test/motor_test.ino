int in1 = 3;
int in2 = 5;

void setup() {
  // put your setup code here, to run once:
   pinMode(in1,OUTPUT);
   pinMode(in2,OUTPUT);
   digitalWrite(in1,LOW);
   digitalWrite(in2,HIGH);
}

void loop() {
  // put your main code here, to run repeatedly:
  delay(3);
}
