#include <SoftwareSerial.h>

#define button 3
#define breathes_pin 0
#define beats_pin 1
#define time_delay 60
#define RX 5
#define TX 6

float raw_breathes = 0;
int timer=0, breath_now=0, breath_before=0, breathes=0, common_breathes=0, counter=0;
double common_beats=0, start_time=0, end_time=0;
bool beat_detected=false, printed=false;

SoftwareSerial ComunicationSerial(RX, TX);



void breath_detection(){
  for(int sample=0; sample<50; sample++)
    raw_breathes += analogRead(breathes_pin);
  raw_breathes /= 50;
  if(raw_breathes > 290){
    if(timer > 3)
      breath_now = 1;
    timer++;}
  else{
    timer = 0;
    breath_now = 0;}}


int sample_difference(){
  float first_sample = read_sample();
  delay(20);  
  return read_sample() - first_sample;}


float read_sample(){
  float sample = 0;
  for(int sample_counter=0; sample_counter<22; sample_counter++)
    sample += analogRead(beats_pin);
  return sample/22;}


void beat_detection() {
  float difference = sample_difference();
  if(difference > 0){
    while(difference > 0){
      difference = sample_difference();} 
    counter++; 
    if(counter == 1){
      start_time = millis();}}
  else{
    while(difference <= 0){
      difference= sample_difference();} 
    counter++;
    if(counter == 1){
      start_time = millis();}}
  if(counter == 50){
    end_time = millis(); 
    common_beats = ((counter*30000))/((end_time-start_time)) *0.19-12.56;
    if(common_beats < 50.0)
      common_beats = 0.0;
    beat_detected = true;
    counter = 0;}}


void measure(){
  if(!beat_detected)
    beat_detection();
  else{
    breath_detection();
    if(breath_now != breath_before){
      if(breath_before)
        breathes++;}
    breath_before = breath_now;
    if(millis()%7500 <= 100 and millis() > 100 and breathes != 0 and common_beats != 0){
      Serial.print(breathes*8);
      Serial.print("-");
      Serial.println(common_beats);
      common_breathes = breathes*8;
      beat_detected = false;
      printed = false;
      breathes = 0;}
    raw_breathes = 0;}}


void comunication(){
  if(!printed){
    if(common_beats >= 140)
      if(common_breathes >= 25)
        ComunicationSerial.write("11\n");
      else
        ComunicationSerial.write("10\n");
    else
      if(common_breathes >= 25)
        ComunicationSerial.write("01\n");
      else
        ComunicationSerial.write("00\n");
    printed = true;}}

  


void setup() {
  Serial.begin(9600);
  pinMode(button, INPUT);
  ComunicationSerial.begin(9600);
}


void loop (){
  if(digitalRead(button))
    comunication();
  else
    measure();
}
