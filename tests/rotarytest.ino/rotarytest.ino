
int pulses, A_SIG=0, B_SIG=1;

void setup(){
  attachInterrupt(15, A_RISE, RISING);
  attachInterrupt(32, B_RISE, RISING);
  Serial.begin(115200);
}//setup


void loop(){
    
}

void A_RISE(){
 detachInterrupt(15);
 A_SIG=1;
 
 if(B_SIG==0)
 pulses++;//moving forward
 if(B_SIG==1)
 pulses--;//moving reverse
 Serial.println(pulses);
 attachInterrupt(15, A_FALL, FALLING);
}

void A_FALL(){
  detachInterrupt(15);
 A_SIG=0;
 
 if(B_SIG==1)
 pulses++;//moving forward
 if(B_SIG==0)
 pulses--;//moving reverse
 Serial.println(pulses);
 attachInterrupt(15, A_RISE, RISING);  
}

void B_RISE(){
 detachInterrupt(32);
 B_SIG=1;
 
 if(A_SIG==1)
 pulses++;//moving forward
 if(A_SIG==0)
 pulses--;//moving reverse
 Serial.println(pulses);
 attachInterrupt(32, B_FALL, FALLING);
}

void B_FALL(){
 detachInterrupt(32);
 B_SIG=0;
 
 if(A_SIG==0)
 pulses++;//moving forward
 if(A_SIG==1)
 pulses--;//moving reverse
 Serial.println(pulses);
 attachInterrupt(32, B_RISE, RISING);
}
