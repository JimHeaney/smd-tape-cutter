//SMD Tape Cutter
// By: Jim Heaney, jim-heaney.com
//CC-BY-SA-NC 4.0

//Declarations
//7 Segment
const int a = 8;
const int b = 6;
const int c = 3;
const int d = 4;
const int e = 5;
const int f = 7;
const int g = 2;
const int dig1 = 10;
const int dig2 = 11;
const int dig3 = 12;
//Stepper Motors - 1 is advance, 2 is cut
const int step2 = 1;
const int dir2 = 0;
const int step1 = 13;
const int dir1 = 9;
//Inputs
const int modesw = A0;
const int pgm = A1;
const int pp = A3;
const int p = A2;
const int mm = A4;
const int m = A5;
//Variables
int mode = 0;
int count = 0;
float len = 0;
int page = 1;
bool pgmpress = 0;
long ppdelay = 0;
long pdelay = 0;
long mmdelay = 0;
long mdelay = 0;
long x100 = 0;
long x10 = 0;
long x1 = 0;
int delaystep = 0;
bool hold = 0;
bool holdpressed = 0;
bool temphold = 0;
const float steps = 8.70;
unsigned long maindelay;
 
void setup() {
  // put your setup code here, to run once:

  //Declare Pin Modes
  pinMode(a, OUTPUT);
  pinMode(b, OUTPUT);
  pinMode(c, OUTPUT);
  pinMode(d, OUTPUT);
  pinMode(e, OUTPUT);
  pinMode(f, OUTPUT);
  pinMode(g, OUTPUT);
  pinMode(dig1, OUTPUT);
  pinMode(dig2, OUTPUT);
  pinMode(dig3, OUTPUT);
  pinMode(step1, OUTPUT);
  pinMode(dir1, OUTPUT);
  pinMode(step2, OUTPUT);
  pinMode(dir2, OUTPUT);
  pinMode(modesw, INPUT);
  pinMode(pgm, INPUT);
  pinMode(pp, INPUT);
  pinMode(p, INPUT);
  pinMode(mm, INPUT);
  pinMode(m, INPUT);
}

void loop() {
  // put your main code here, to run repeatedly:

  //Check if settings mode is on
  mode = digitalRead(modesw);
  if (mode == 1){
    settings();
  }
  //Check if the pgm button is pressed. If so, enter hold mode.
  if (digitalRead(pgm == 1)){
    holdpressed = 1;
  }
  //In normal operating mode, advance by the set amount, call the cutter, subtract one from the count, display the count, and repeat.
  //Once out of parts to cut, wait until the menu is entered.
  if(count == 0){
    sevenseg(count);
    mode = digitalRead(modesw);
    if (mode = 1){
      settings();
    }
  }
  if(count > 0){
  sevenseg(count);
  cut();
  prt(len, 1);
  count = count - 1;
  }
  if (count <= 0){
    count = 0;
  }
  maindelay = millis() + 200;
  while (maindelay > millis()){
    sevenseg(count); 
  }  
}

void settings(){
  //Called when in the settings menu
  while (mode == 1){
    //Keep in this function until the mode switch is pressed down.
    mode = digitalRead(modesw); 
    if (page == 1){ 
      //Page 1: Set the starting location.
      //First, see if we need to go to the next settings page. 
      if (digitalRead(pgm) == 1){ 
        pgmpress = 1;
      }
      if ((digitalRead(pgm) == 0) && (pgmpress == 1)){ 
        pgmpress = 0;
        page = 2;
      }
      //Next, advance the tape by the amount requested.
      if (digitalRead(pp) == 1){
        mve(10, 0);
      }
      if (digitalRead(p) == 1){
        mve(1, 0);
      }
      if (digitalRead(mm) == 1){
        mve(-10, 0);
      }
      if (digitalRead(m) == 1){
        mve(-1, 0);
      }
    }          
    if (page == 2){ 
      //Page 2: Set the count of cuts to make
      //First, see if we need to go to the next settings page. 
      if (digitalRead(pgm) == 1){ 
        pgmpress = 1;
      }
      if ((digitalRead(pgm) == 0) && (pgmpress == 1)){ 
        pgmpress = 0;
        page = 3;
      }
      //Next, read the inputs and make the appropriate moves. Each button should only count 1 press every 200ms.
      if ((digitalRead(pp) == 1) && (ppdelay <= millis())){
        count = count + 10;
        ppdelay = millis() + 200;
      }
      if ((digitalRead(p) == 1) && (pdelay <= millis())){
        count = count + 1;
        pdelay = millis() + 200;
      }
      if ((digitalRead(mm) == 1) && (mmdelay <= millis())){
        count = count - 10;
        mmdelay = millis() + 200;
      }
      if ((digitalRead(m) == 1) && (mdelay <= millis())){
        count = count - 1;
        mdelay = millis() + 200;
      }
      //Display the current count on screen, but first check if it is negative.
      if (count < 0){
        count = 0;
      }
      sevenseg(count);            
    }
    if (page == 3){
      //Page 3: Set the number of parts per cut
      //If the pgm button is pressed, go back to page 1. 
       if (digitalRead(pgm) == 1){ 
        pgmpress = 1;
      }
      if ((digitalRead(pgm) == 0) && (pgmpress == 2)){ 
        pgmpress = 0;
        page = 1;
      }
      //First, read the inputs and make the appropriate moves.
      if (digitalRead(pp) == 1){
        len = len + 5;
        prt(5, 0);
      }
      if (digitalRead(p) == 1){
        len = len + 1;
        prt(1, 0);
      }
      if (digitalRead(mm) == 1){
        len = len - 5;
        prt(-10, 0);
      }
      if (digitalRead(m) == 1){
        len = len - 1;
        prt(-1, 0);
      }
      //Update the screen to show the current value of len, but first make sure it is positive.
      if (len < 0){
        len = 0;
      }
      sevenseg(len);
    }
  }
}

void sevenseg(int x){
  //Called to display the value x on the 7-segment display
  //Split the number into 3 individual digits
  x100 = (x / 100) % 10;
  x10 = (x / 10) % 10;
  x1 = (x / 1) % 10;

  //Next, write each of these to the scrren
  segs(x100);
  digitalWrite(dig1, HIGH);
  digitalWrite(dig1, LOW);
  segs(x10);
  digitalWrite(dig2, HIGH);
  digitalWrite(dig2, LOW);
  segs(x1);
  digitalWrite(dig3, HIGH);
  digitalWrite(dig3, LOW);
}

void segs(int y){
  //Lights the corresponding 7 segments of a single digit
  if (y == 0){
    digitalWrite(a, LOW);
    digitalWrite(b, LOW);
    digitalWrite(c, LOW);
    digitalWrite(d, LOW);
    digitalWrite(e, LOW);
    digitalWrite(f, LOW);
    digitalWrite(g, HIGH);
  }
  if (y == 1){
    digitalWrite(a, HIGH);
    digitalWrite(b, LOW);
    digitalWrite(c, LOW);
    digitalWrite(d, HIGH);
    digitalWrite(e, HIGH);
    digitalWrite(f, HIGH);
    digitalWrite(g, HIGH);
  }
  if (y == 2){
    digitalWrite(a, LOW);
    digitalWrite(b, LOW);
    digitalWrite(c, HIGH);
    digitalWrite(d, LOW);
    digitalWrite(e, LOW);
    digitalWrite(f, HIGH);
    digitalWrite(g, LOW);
  }
  if (y == 3){
    digitalWrite(a, LOW);
    digitalWrite(b, LOW);
    digitalWrite(c, LOW);
    digitalWrite(d, LOW);
    digitalWrite(e, HIGH);
    digitalWrite(f, HIGH);
    digitalWrite(g, LOW);
  }
  if (y == 4){
    digitalWrite(a, HIGH);
    digitalWrite(b, LOW);
    digitalWrite(c, LOW);
    digitalWrite(d, HIGH);
    digitalWrite(e, HIGH);
    digitalWrite(f, LOW);
    digitalWrite(g, LOW);
  }
  if (y == 5){
    digitalWrite(a, LOW);
    digitalWrite(b, HIGH);
    digitalWrite(c, LOW);
    digitalWrite(d, LOW);
    digitalWrite(e, HIGH);
    digitalWrite(f, LOW);
    digitalWrite(g, LOW);
  }
  if (y == 6){
    digitalWrite(a, LOW);
    digitalWrite(b, HIGH);
    digitalWrite(c, LOW);
    digitalWrite(d, LOW);
    digitalWrite(e, LOW);
    digitalWrite(f, LOW);
    digitalWrite(g, LOW);
  }
  if (y == 7){
    digitalWrite(a, LOW);
    digitalWrite(b, LOW);
    digitalWrite(c, LOW);
    digitalWrite(d, HIGH);
    digitalWrite(e, HIGH);
    digitalWrite(f, HIGH);
    digitalWrite(g, HIGH);
  }
  if (y == 8){
    digitalWrite(a, LOW);
    digitalWrite(b, LOW);
    digitalWrite(c, LOW);
    digitalWrite(d, LOW);
    digitalWrite(e, LOW);
    digitalWrite(f, LOW);
    digitalWrite(g, LOW);
  }
  if (y == 9){
    digitalWrite(a, LOW);
    digitalWrite(b, LOW);
    digitalWrite(c, LOW);
    digitalWrite(d, HIGH);
    digitalWrite(e, HIGH);
    digitalWrite(f, LOW);
    digitalWrite(g, LOW);
  }
  if (y >= 10){
    digitalWrite(a, HIGH);
    digitalWrite(b, HIGH);
    digitalWrite(c, HIGH);
    digitalWrite(d, HIGH);
    digitalWrite(e, HIGH);
    digitalWrite(f, HIGH);
    digitalWrite(g, LOW);
  }
}

void mve(float x, bool y){
  //Moves the tape forward by x  1/10 millimeters. Y sets the speed slow (0) or fast (1)
  //First, convert the distance into a number of steps. 
  long togo = steps * x;
  //Next, set the direction
  if (togo < 0){
    digitalWrite(dir1, LOW);
    togo = togo * -1;
  }
  else{
    digitalWrite(dir1, HIGH);
  }
  //Then, set the speed
  if (y == 0){
    delaystep = 1000;
  }
  else{
    delaystep = 500;
  }
  //Last, execute the required number of steps
  while (togo > 0){
    togo = togo -1;
    digitalWrite(step1, HIGH);
    delayMicroseconds(delaystep);
    digitalWrite(step1, LOW);
    delayMicroseconds(delaystep);
  }
  //Once done moving, delay for a small amount of time
  delay(200);
}

void prt(float x, bool y){
  //Moves the tape forward by x parts. Y sets the speed slow (0) or fast (1)
  //First, convert the parts into a number of steps. 
  long togo = 100 * x;
  //Next, set the direction
  if (togo < 0){
    digitalWrite(dir1, LOW);
    togo = togo * -1;
  }
  else{
    digitalWrite(dir1, HIGH);
  }
  //Then, set the speed
  if (y == 0){
    delaystep = 1000;
  }
  else{
    delaystep = 500;
  }
  //Last, execute the required number of steps
  while (togo > 0){
    togo = togo -1;
    digitalWrite(step1, HIGH);
    delayMicroseconds(delaystep);
    digitalWrite(step1, LOW);
    delayMicroseconds(delaystep);
  }
  //Once done moving, delay for a small amount of time
  delay(200);
}

void cut(){
  //Advance the cutting blade 360 degrees. 
  digitalWrite(dir2, HIGH);
  for (int i = 0; i <= 3200; i++){
    digitalWrite(step2, HIGH);
    delayMicroseconds(100);
    digitalWrite(step2, LOW);
    delayMicroseconds(100);
  }
}
