const int  ledler[]={7,6,5,4,3,2,};
const int  ledcount=6;
void setup()
{
  for(int i=0;i<ledcount;i++){
    pinMode(ledler[i],OUTPUT);
    
  }
  
}

void loop()
{
  for(int i=0;i<ledcount;i++){
    digitalWrite(ledler[i],HIGH);
    delay(500);}
  for(int i=0;i<ledcount;i++){
    digitalWrite(ledler[i],LOW);
    delay(500);}
}
