/**
* Grace Barkhuff
* CS 243
* https://sites.google.com/a/mtholyoke.edu/cs-243-spring-15-barkh22g/project-2-medsbuddy/final-code-meds-buddy
* MedsBuddy


* Acts as an alarm. Counts ten seconds, then LEDs turn red.
* Waits for two capacitive touch sensors to be held.
* This sets off a song, flapping wings, and LED spectacle.
* Then everything stops and LEDs are green to indicate this is over.

* Music Code adapted from toneMelody example in Arduino library.
* Neopixel Code adapted from strandTest in the NeoPixel library.
* Capacitive touch code adapted from the code by Eva Snyder which was adapted from
* http://www.instructables.com/id/Turn-a-pencil-drawing-into-a-capacitive-sensor-for/

**/

/**
*Neopixel Setup
**/

#include <Adafruit_NeoPixel.h>
#include <avr/power.h>

//pin for neopixels
#define PIN 6
// How many NeoPixels are attached to the Arduino?
#define NUMPIXELS 4

// When we setup the NeoPixel library, we tell it how many pixels, and which pin to use to send signals.
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

/**
* Servo Motor Setup
**/
#include <Servo.h> 
 
//define variable for servo A and servo B (left and right wings)
Servo myservoA; 
Servo myservoB;
 
 
/**
* Piezzo Buzzer/Music Setup
**/ 
//let it know about pitches.h tab
#include "pitches.h"

//variable for buzzer pin
int buzzer = 2;
 
// notes in the alert melody:
int alertMelody[] = {NOTE_D7, NOTE_CS7, NOTE_C7, NOTE_B6, NOTE_AS6, NOTE_A6, NOTE_GS6,
                    NOTE_G6, NOTE_FS6, NOTE_F6, NOTE_E6};

//duration of notes in alert melody
// note durations: 4 = quarter note, 8 = eighth note, etc.:
int alertNoteDurations[] = {10, 10, 10, 10, 10, 10, 10, 10, 10, 10};



// notes in the Muffin Man melody:
int muffinMan[] = {
  NOTE_D4, NOTE_G4,NOTE_G4, NOTE_A4, NOTE_B4, NOTE_G4, NOTE_G4, NOTE_FS4,
  NOTE_E4, NOTE_A4, NOTE_A4, NOTE_G4, NOTE_FS4, NOTE_D4, NOTE_D4, 
  NOTE_D4, NOTE_G4, NOTE_G4, NOTE_A4, NOTE_B4, NOTE_G4, NOTE_G4, NOTE_G4,
  NOTE_A4, NOTE_A4, NOTE_D4, NOTE_D4, NOTE_G4};

// note durations in Muffin Man
int muffinDurations[] = {
  4, 4, 3, 8, 4, 4, 3, 8, 4, 4, 3, 8, 4, 4, 2, 4, 4, 3, 8, 4, 4, 4, 4, 4, 4, 4, 4, 1};

//notes in Csipp Csepp
int csippCsepp[] = {NOTE_G4, NOTE_G4, NOTE_D4, NOTE_D4, NOTE_E4, NOTE_G4, NOTE_D4, NOTE_D4, 
                    NOTE_B4, NOTE_B4, NOTE_A4, NOTE_G4, NOTE_D4, NOTE_G4, NOTE_G4, NOTE_G4, NOTE_G4,NOTE_D4, 
                    NOTE_G4, NOTE_G4, NOTE_D4, NOTE_D4, NOTE_E4, NOTE_G4, NOTE_D4, NOTE_D4, 
                    NOTE_B4, NOTE_B4, NOTE_A4, NOTE_G4, NOTE_D4,NOTE_E4, NOTE_G4, NOTE_FS4, NOTE_A4, NOTE_G4};

//note durations in Csipp Csepp
int csippCseppDurations[] = {4, 4, 4, 4, 4, 4, 4, 4, 4, 8, 8, 4, 4, 8, 8, 8, 8, 2,
                            4, 4, 4, 4, 4, 4, 4, 4, 4, 8, 8, 4, 4, 8, 8, 8, 8, 2};
                            
//notes in Pop Goes the Weasel
int weasel[] = { NOTE_G4, NOTE_G4, NOTE_A4, NOTE_A4, NOTE_B4, NOTE_D5,NOTE_B4, NOTE_G4,NOTE_D4,
                        NOTE_G4, NOTE_G4, NOTE_A4, NOTE_C5, NOTE_B4, NOTE_G4, NOTE_D3,
                        NOTE_G4, NOTE_G4, NOTE_A4, NOTE_A4, NOTE_B4, NOTE_D5, NOTE_B4, NOTE_G4, 
                        NOTE_E5, NOTE_A4, NOTE_C5, NOTE_B4, NOTE_G4};

//note durations in Pop Goes the Weasel                        
int weaselDurations[] ={ 4, 8, 4, 8, 8, 8, 8, 4, 8, 4, 8, 4, 8, 3, 4, 8, 
                        4, 8, 4, 8, 8, 8, 8, 3, 3, 4, 8, 3, 2};


/**
* Capacitative Touch Sensors Setup
**/ 

// Pins to connect to two conductive sensors ("hands")
int capSensePinA = 4;
int capSensePinB = 5;

//golbal int to keep track of how long hands held for
int count = 0;

// This is how high the sensor needs to read in order
//  to trigger a touch.
int touchedCutoff = .5;

//--------------------------------------------------------------

void setup() {
  
  //begin Serial Monitor
  Serial.begin(9600);
    
  //begin random number generator with random seed
  randomSeed(analogRead(0));
    
  //init Neopixel library  
  pixels.begin(); 
    
  
  
   //attach servoA to pin 9, B to pin 10
   myservoA.attach(9); //A is the servo which runs between 180 and 90 degrees
   myservoB.attach(10); //B 90 to 0 degrees
   
   //place both servos at point 90 degrees 
   myservoA.write(90);
   myservoB.write(90);
   
   
   //turn on one purple light to begin
   // pixels.Color takes RGB values, from 0,0,0 up to 255,255,255
   pixels.setPixelColor(0, pixels.Color(100,0,100)); // Moderately bright purple color.

   pixels.show(); // This sends the updated pixel color to the hardware.
    
   //begin 10 second counter to simulate an alarm
   //let 10 seconds be 10am
   delay(1000 * 10);
  
  //then at "10am" call method to turn on red alert lights 
  //and play alert sound
  alertChild(); 
  
  //now ready for child to hold the buddy's hands to say the meds have been taken
}

void loop() {
  
  
  //if BOTH capacitive touch sensors are being held 
  if (readCapacitivePin(capSensePinA) > touchedCutoff && readCapacitivePin(capSensePinB) > touchedCutoff) {
   
    //increase counter
    count = count + 1;
    
    //print count
    Serial.println(count);
    
    //turn on next Neopixel to indicate what the counter reads
    pixels.setPixelColor(count-1, pixels.Color(0,0,150)); // Moderately bright blue color.

    pixels.show(); // This sends the updated pixel color to the hardware.
    
    //wait 1 second
    delay(1000);
   
  }
  //if both hands are not being held (either just one or neither)
  else {

    //reset counter to 0
    count = 0;
    
    //print count
    Serial.println(count);
  }
  
  //Print the value of the capacitive sensors
  Serial.print("Capacitive Sensor reads: ");
  Serial.println(readCapacitivePin(capSensePinA));
  Serial.println(readCapacitivePin(capSensePinB));

  
  //check to see if both hands have been held held for 5 seconds (or more somehow)
  if (count > 4){
   //begin flapping, singing, light spectacle by choosing one of three songs
   chooseSong();
 
  //after, be in the final state where neopixels are all green
  finalState();
  }
}

//--------------------------------------------------------------

/**
* This method calls others to play the alert music, and to turn all Neopixels red.
**/
void alertChild(){
  
  //play alert music
  alertMusic();
  
  //turn on red lights
  lightPixels(255, 0, 0);
}

/**
* This method plays the alert music, once forward and once backward.
**/
void alertMusic(){
  
  // iterate over the notes of the melody forward:
  for (int thisNote = 0; thisNote < 10; thisNote++) {

    // to calculate the note duration, take one second 
    // divided by the note type.
    //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
    int alertNoteDuration = 1000/alertNoteDurations[thisNote];
    tone(buzzer, alertMelody[thisNote],alertNoteDuration);

    // to distinguish the notes, set a minimum time between them.
    // the note's duration + 30% seems to work well:
    int pauseBetweenNotes = alertNoteDuration * 1.30;
    delay(pauseBetweenNotes);
    // stop the tone playing:
    noTone(buzzer);
  }
  
  // iterate over the notes of the melody backward:
  for (int thisNote = 8; thisNote >0; thisNote--) {

    // to calculate the note duration, take one second 
    // divided by the note type.
    //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
    int alertNoteDuration = 1000/alertNoteDurations[thisNote];
    tone(buzzer, alertMelody[thisNote],alertNoteDuration);

    // to distinguish the notes, set a minimum time between them.
    // the note's duration + 30% seems to work well:
    int pauseBetweenNotes = alertNoteDuration * 1.30;
    delay(pauseBetweenNotes);
    // stop the tone playing:
    noTone(buzzer);
  }
}

/**
* Method readCapacitivePin
* Input: Arduino pin number
* Output: A number, from 0 to 17 expressing
* how much capacitance is on the pin
* When you touch the pin, or whatever you have
* attached to it, the number will get higher
* In order for this to work now,
* The pin should have a resistor pulling
* it up to +5v.
* Method adapted from the code by Eva Snyder which was adapted from
* http://www.instructables.com/id/Turn-a-pencil-drawing-into-a-capacitive-sensor-for/
**/
uint8_t readCapacitivePin(int pinToMeasure){
  
  // This is how you declare a variable which
  //  will hold the PORT, PIN, and DDR registers
  //  on an AVR
  volatile uint8_t* port;
  volatile uint8_t* ddr;
  volatile uint8_t* pin;
  // Here we translate the input pin number from
  //  Arduino pin number to the AVR PORT, PIN, DDR,
  //  and which bit of those registers we care about.
  byte bitmask;
  if ((pinToMeasure >= 0) && (pinToMeasure <= 7)){
    port = &PORTD;
    ddr = &DDRD;
    bitmask = 1 << pinToMeasure;
    pin = &PIND;
  }
  if ((pinToMeasure > 7) && (pinToMeasure <= 13)){
    port = &PORTB;
    ddr = &DDRB;
    bitmask = 1 << (pinToMeasure - 8);
    pin = &PINB;
  }
  if ((pinToMeasure > 13) && (pinToMeasure <= 19)){
    port = &PORTC;
    ddr = &DDRC;
    bitmask = 1 << (pinToMeasure - 13);
    pin = &PINC;
  }
  // Discharge the pin first by setting it low and output
  *port &= ~(bitmask);
  *ddr  |= bitmask;
  delay(1);
  // Make the pin an input WITHOUT the internal pull-up on
  *ddr &= ~(bitmask);
  // Now see how long the pin to get pulled up
  int cycles = 16000;
  for(int i = 0; i < cycles; i++){
    if (*pin & bitmask){
      cycles = i;
      break;
    }
  }
  // Discharge the pin again by setting it low and output
  //  It's important to leave the pins low if you want to 
  //  be able to touch more than 1 sensor at a time - if
  //  the sensor is left pulled high, when you touch
  //  two sensors, your body will transfer the charge between
  //  sensors.
  *port &= ~(bitmask);
  *ddr  |= bitmask;
  
  return cycles;
}

/**
* chooseSong() randomly chooses 1 of 3 songs to play on the Piezzo Buzzer.
**/
void chooseSong(){
  
  //pick a random int 0 - 3 (exclusive of 3)
  int randNumber = random(3);
  
  //pass song notes and note durations as param depending on random number
  
  //if randNumber = 0
  if (randNumber <1){
    //choose MuffinMan
    flapAndSing(muffinMan, muffinDurations, (sizeof(muffinDurations)/sizeof(int)));
  }
  //if randNumber = 1
  else if (randNumber <2){
    //choose csippCsepp
    flapAndSing(csippCsepp, csippCseppDurations, (sizeof(csippCseppDurations)/sizeof(int)));
  }
  //if randNumber = 2
  else{
    //choose weasel
    flapAndSing(weasel, weaselDurations, (sizeof(weaselDurations)/sizeof(int)));
  }
    
}

/**
* flapAndSing() is the primary method for controlling activity during the "happy"
* reaction of the buddy to the child's holding its hands.
* The method takes in the melody, the note durations of said melody, and the length of
* the melody. It plays the song, interspersing wing flaps
* and Neopixel color updates.
**/
void flapAndSing(int *melody, int *noteDurations, int length){
  
  // iterate over the notes of the melody that are held in an array:
  for (int thisNote = 0; thisNote < length; thisNote++) {

    // to calculate the note duration, take one second 
    // divided by the note type.
    //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
    int noteDuration = 1000/noteDurations[thisNote];
    //play note on buzzer pin for the specified duration
    tone(buzzer, melody[thisNote], noteDuration);
    
    //turn on neopixels according to the iteration number in the loop
    chooseNeoColor(thisNote);

    // to distinguish the notes, set a minimum time between them.
    // the note's duration + 30% seems to work well:
    int pauseBetweenNotes = noteDuration * 1.30;
      delay(pauseBetweenNotes);


    //call one flap every 3 notes
    if (thisNote % 3 == 0){
      flapWings(thisNote);   
    }
    
    // stop the tone playing:
    noTone(buzzer);
 
    }
 }
  
/**
* flapWings() flaps up if on even iteration of for loop or
* down if on odd.
**/
void flapWings(int i){
  
    //if i even
    if (i % 2 == 0){
    myservoA.write(180);
    myservoB.write(0);
    }
    
    //if odd
    else{
    myservoA.write(90); 
    myservoB.write(90);
    }

}
  
  
/**
* chooseNeoColor() cycles through four different colors to turn on 
* neopixels depending on the note of melody it's on.
**/
void chooseNeoColor(int i){
  
  //red
  if (i % 4 == 0)
  lightPixels(150, 0, 0);
  
  
  //green
   else if (i % 4 == 1)
  lightPixels(0, 150, 0);
  
  //blue
   else if (i % 4 == 2)
  lightPixels(0, 0, 150);
  
  //purple
   else if (i % 4 == 3)
  lightPixels(150, 25, 150);
  
}

/**
* lightPixels() method
*turns on all pixels to same color as indicated by (i,j,k) (r,g,b) input.
**/
void lightPixels(int i, int j, int k){
  //for every light
  for(int m=0;m<NUMPIXELS;m++){
     //color red
     // pixels.Color takes RGB values, from 0,0,0 up to 255,255,255
     pixels.setPixelColor(m, pixels.Color(i, j, k)); // Moderately bright green color.

     pixels.show(); // This sends the updated pixel color to the hardware.
  }
}
  
/**
* finalState() goes through rainbowCycle() for a final celebration,
* then sets all Neopixels to green.
**/
void finalState(){
  
  //detach servos
  myservoA.detach();
  myservoB.detach();
    
  //cycle through rainbow of colors on Neopixels with 5 unit delay
  rainbowCycle(5); 
  
  //for all Neupixels
  for(int i=0;i<NUMPIXELS;i++){
    
     // pixels.Color takes RGB values, from 0,0,0 up to 255,255,255
     pixels.setPixelColor(i, pixels.Color(0,150,0)); // Moderately bright green color.

     pixels.show(); // This sends the updated pixel color to the hardware.
  }
}

/**
* rainbowCycle() method taken from strandTest in the Neopixel library.
**/
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256* 5; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< pixels.numPixels(); i++) {
      pixels.setPixelColor(i, Wheel(((i * 256 / pixels.numPixels()) + j) & 255));
    }
    pixels.show();
    delay(wait);
  }
}

/**
* Wheel method taken from strandTest Neopixel library.
* Needed in order to use rainbowCycle().
* Input a value 0 to 255 to get a color value.
* The colours are a transition r - g - b - back to r.
**/
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
   return pixels.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else if(WheelPos < 170) {
    WheelPos -= 85;
   return pixels.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  } else {
   WheelPos -= 170;
   return pixels.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  }
}
