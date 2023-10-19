
//
//  Test Sequence:
//
//   0s - Flash 1 pulse Red
//   5s - Flash 3 pulses Blue
//  10s - Override with constant Green
//  15s - Back to Flashing 3 pulses Blue
//  20s - Set to constant Orange
//  25s - Blink Teal for 500ms
//

#include <Arduino.h>
#include <Flasher.h>

// Ensure your LED GPIOs are defined here
// If you don't have a green or blue LED
// set the GPIO to -1

#define REDGPIO 15    /* D8 - GPIO15 */
#define GREENGPIO 13  /* D7 - GPIO13 */
#define BLUEGPIO 12   /* D6 - GPIO12 */

long t ;
int step ;

void setup() {

  Serial.begin(115200) ;
  Serial.printf("\n\nWelcome\n\n") ;
 
  // Define the LEDs - false indicates that the LEDs are not inverted
  // i.e. a logic 1 on the output lights the LED, and logic 0 turns it off
 
  Flasher.selectLeds(REDGPIO, false, GREENGPIO, false, BLUEGPIO, false) ;

  t = millis() ;
  step=0 ;

}

void loop() {

  // Call Flasher Task Process Handler in loop()
  Flasher.processTask() ;

  // Update Flasher
  if (millis()>t) {
    
    t = millis() + 5000 ;

    switch (step) {

      case 0:
          Flasher.set(1, FlasherTask::Red) ; // 1 Flashes
          step++ ;
          break ;

      case 1:
          // After 5 seconds 
          Flasher.set(3, FlasherTask::Blue) ; // 3 Flashes
          step++ ;
          break ;

      case 2:
          // After 10 seconds
          Flasher.override(true, FlasherTask::Green) ; // Override with Green
          step++ ;
          break ;

      case 3:
          // After 15 seconds
          Flasher.override(false) ; // Disable override (back to 3 blue flashes)
          step++ ;
          break ;

      case 4:
          // After 20 seconds
          Flasher.stop(FlasherTask::Orange) ; // Stop Flasher on Orange
          step++ ;
          break ;

      case 5:
          // After 25 seconds
          Flasher.blink(500, FlasherTask::Teal) ; // Blink for 500ms
          step=0 ;
          break ;

    }

  }

}
