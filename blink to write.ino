#include <Wire.h>
#include <LCD_I2C.h>
#include <TimerOne.h>

LCD_I2C lcd(0x27, 20, 4);

void setup() {
  // put your setup code here, to run once:

  //start lcd, serial, and timer
  lcd.begin();
  lcd.backlight();
  int B;
  Serial.begin(9600);
  Timer1.initialize(100000);
  Timer1.attachInterrupt(callback);

}
    
//string of characters with alphabet and hyphen for space    
char letter[27] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ-";
char what[12];
int idx=0;
// value measured by emg in bits
float emg;
// will registered whether a rise or a fall has been found
volatile int state = 0;
// indices for interrupt, letter string, and lcd block
volatile int i = 0;
volatile int a;
volatile int b = 1;
//time markers
double t0;
double t1;
double last_t0;

void loop() {
  // put your main code here, to run repeatedly
    // read the emg
    emg = analogRead(A0);
    emg = emg*0.004; //gain/resolution was changed from 0.00488 to reduce instances of saturation of signal
    Serial.println(emg);

// will check whether we have previously registered a rise or a fall
// depending on it, it will further check for one or two blinks and see if we should add or delete a letter
// this is done checking for the time in between two blinks using the millis() function
    switch (state) {
      case 0:
        if (emg >= 2.5) {
          last_t0 = t0;
          t0 = millis();

          state = 1;
          
        }

        break;

      case 1:
      
        if (emg < 2.5){
          t1 = millis();
          state=0;


        if (t1-t0 < 300 && t1-t0 > 0 && t0 - last_t0 > 700) {
        lcd.setCursor(b,1);
        lcd.print(letter[a]);
        b = b+1;
        i=0;
        if (b > 20) {
          b = 0;
          lcd.clear();
        }

    } else if (t0-last_t0 <= 700) {
      b=b-1;
      lcd.setCursor(b,1);
      lcd.print("!");
    }
     
        }

        break;

        default:
          state = 0;

          break;
    }

    // show the letter the user may select once every 10 iterations of TimerInterrupt (every 1s)
    if (i % 10 == 0) {


    lcd.setCursor(0,0);
    lcd.print(letter[a]);
    } 


         


}

//callback to run with TimerInterrupt
//it changes the letter once every second, which is 10 times the interrupt of 0.1s
void callback() {
 i = i+1;
 if (i > 270) {
  i = 0;
 }
 a = (i/10);
}
