//
// Flasher.cpp (A38-Flasher)
//
// (c) Steve Clarke, Vizier Design 2023
//
// This class provides an object to manage / flash single or
// multi-colour LEDs.
//
// CHANGELOG
// =========
//
// 20231019     0.1   Initial Version
//
//
// PRE-REQUISITES
// ==============
//
// Arduino ESP8266 or ESP32
//
//
// OVERVIEW
// ========
//
//  Process Task (required in the loop() function)
//
//    Flasher.ProcessTask() ;
//
//  Setup Flasher GPIO Ports and sense
//
//    Flasher.selectLeds(REDGPIO, false, GREENGPIO, false, BLUEGPIO, false) ;
//
//  Configure to Repeatedly flash 2 pulses
//
//    Flasher.set(2, FlasherTask::Blue) ;
//
//  Override Flashing with colour
//
//    Flasher.override(true, FlasherTask::Orange) ;
//    Flasher.override(false) ; // Stop overriding
//
//  Stop Flashing
//
//    Flasher.stop(FlasherTask::Off) ;
//
//  Blink with colour
//
//    Flasher.blink(500, FlasherTask::Teal) ;
//
//
// LICENSE
// =======
//
// Released under the GNU AFFERO GENERAL PUBLIC LICENSE
// See LICENSE file
//


// Enable Debug
#define FLASHER_DEBUG 1

#include <Arduino.h>

#include "Flasher.h"

#ifdef FLASHER_DEBUG
#define DEBUG(fmt, args...) Serial.printf(fmt, ##args)
#else
#define DEBUG(fmt, args...)
#endif

FlasherTask Flasher ;

////////////////////////////////////////////////
// FLASHER MANAGEMENT

FlasherTask::FlasherTask() {
  iGpioRed = -1 ;
  iGpioGreen = -1 ;
  iGpioBlue = -1 ;
  bBlinkRunning = false ;
  bFlasherRunning = false ;
  lightLeds(FlasherTask::All, false) ;
  setDevelMode(true, true) ;
}

void FlasherTask::lightLeds(Colour colour, bool state)
{
  bool red = (colour & FlasherTask::Red) !=0 ;
  bool green = (colour & FlasherTask::Green) !=0 ;
  bool blue = (colour & FlasherTask::Blue) !=0 ;
  bool builtin = (colour & FlasherTask::Builtin) !=0 ;

  DEBUG("red=%d/%d\n", red, bRedHigh) ;
  DEBUG("grn=%d/%d\n", green, bGreenHigh) ;
  DEBUG("blu=%d/%d\n", blue, bBlueHigh) ;
  
  if (iGpioBuiltin>0) 
    digitalWrite(iGpioBuiltin, builtin ? !bBuiltinHigh^state : bBuiltinHigh^state);

  if (iGpioRed>0)
    digitalWrite(iGpioRed, red ? !bRedHigh^state : bRedHigh^state);
  
  if (iGpioGreen>0)
    digitalWrite(iGpioGreen, green ? !bGreenHigh^state : bGreenHigh^state);
  
  if (iGpioBlue>0)
    digitalWrite(iGpioBlue, blue ? !bBlueHigh^state : bBlueHigh^state);
  
}


void FlasherTask::selectLeds( int gpioRed, bool invertedRed,
                              int gpioGreen, bool invertedGreen,
                              int gpioBlue, bool invertedBlue,
                              int gpioBuiltin, bool invertedBuiltin)
{
  DEBUG("Flash GPIO: %d %d %d %d\n", gpioRed, gpioGreen, gpioBlue, gpioBuiltin) ;

  iGpioRed = gpioRed ;
  iGpioGreen = gpioGreen ;
  iGpioBlue = gpioBlue ;
  iGpioBuiltin = gpioBuiltin ;
  bRedHigh = !invertedRed ;
  bGreenHigh = !invertedGreen ;
  bBlueHigh = !invertedBlue ;
  bBuiltinHigh = !invertedBuiltin ;

  if (iGpioRed>0) pinMode(iGpioRed, OUTPUT) ;
  if (iGpioGreen>0) pinMode(iGpioGreen, OUTPUT) ;
  if (iGpioBlue>0) pinMode(iGpioBlue, OUTPUT) ;
  if (iGpioBuiltin>0) pinMode(iGpioBuiltin, OUTPUT) ;

}

void FlasherTask::setDevelMode(bool develmode, bool verbose)
{
  this->develmode = develmode ;
  this->verbose = verbose ;
}

void FlasherTask::setupTask() {
  DEBUG("FlasherTask Setup\n");
}

void FlasherTask::processTask() {

  if (!bOverrideRunning && bBlinkRunning && millis() > iBlinkTimeout) {
    lightLeds(FlasherTask::All, false) ;
    lightLeds(cFlasherColour, true) ;
    bBlinkRunning = false ;
    DEBUG("Blink Stop, back to Flash RGBI=%X\n", (int)cFlasherColour) ;
  }

  if (!bOverrideRunning && !bBlinkRunning && bFlasherRunning && millis() > iTimeout) {

    // Called every 150ms
    iTimeout = millis() + 150 ;

    // 1: 11100000000000000000
    // 2: 11101000000000000000
    // 3: 11101010000000000000
    // 4: 11101010100000000000
    // 5: 11101010101000000000
    // 6: 11101010101010000000
    // 7: 11101010101010100000
    // 8: 11101010101010101000

    DEBUG("iFlasherState=%d\n", iFlasherState) ;
    switch (iFlasherState) {

      case 0:
        // First Flash Pulse (600ms)
        lightLeds(cFlasherColour, true) ;
        iFlasherState++ ;
        break ;
      case 1:
      case 2:
        iFlasherState++ ;
        break ;
      case 3:
        if (iFlasherCode==1) {
          iFlasherState=18 ;
        } else {
          iFlasherState=4 ;
        }
        break ;

      case 4:   // Second Pulse
      case 6:   // Third Pulse
      case 8:   // Fourth Pulse
      case 10:  // Fifth Pulse
      case 12:  // Sixth Pulse
      case 14:  // Seventh Pulse
      case 16:  // Eighth Pulse

        // Flash Off (150ms)
        lightLeds(FlasherTask::All, false) ;
        iFlasherState++ ;
        break ;

      case 5:
      case 7:
      case 9:
      case 11:
      case 13:
      case 15:
      case 17:

        // Next Pulse (150ms)
        lightLeds(cFlasherColour, true) ;
 
        if ((iFlasherState-1)/2 >= iFlasherCode) {
          // Stop after iFlasherCode pulses
          iFlasherState=18 ;
        } else {
          // Continue to flash (fall through after 8 flashes)
          iFlasherState++ ;
        }

        break ;

      case 18:

        // LEDs off
        lightLeds(FlasherTask::All, false) ;
        iFlasherState++ ;
        break ;

      case 19:
      case 20:
      case 21:

        // Wait
        iFlasherState++ ;
        break ;

      case 22:

        // Re-start cycle
        iFlasherState=0 ;
        break ;
    }
  }
}

void FlasherTask::override(bool state, Colour colour)
{
  cOverrideColour = colour ;

  if (state) {
    lightLeds(cOverrideColour, true) ;
    bOverrideRunning = true ;
    DEBUG("Override Start, RGBI=%X\n", (int)cOverrideColour) ;
  } else {
    if (bBlinkRunning) {
      DEBUG("Override Stop, back to blink RGBI=%X\n", (int)cBlinkColour) ;
      lightLeds(cBlinkColour, true) ;      
    } else {
      DEBUG("Override Stop, back to flash RGBI=%X\n", (int)cFlasherColour) ;
      lightLeds(cFlasherColour, true) ;
    }
    bOverrideRunning = false ;
  }

}

void FlasherTask::blink(int msec, Colour colour)
{
  iBlinkTimeout = millis() + msec ;
  cBlinkColour = colour ;
  bBlinkRunning = true ;
  lightLeds(cBlinkColour, true) ;
  DEBUG("blink start, RGBI=%X\n", (int)cBlinkColour) ;
}

// function: set()
// parameter: rate: Number of flash pulses
void FlasherTask::set(int rate, Colour colour) {

  int newCode = rate ;

  if ( iFlasherCode != newCode || 
       cFlasherColour != colour || 
       !bFlasherRunning ) {

    iTimeout = 0 ;
    iFlasherState = 0 ;
    iFlasherCode = newCode ;
    cFlasherColour = colour ;
    bFlasherRunning = true ;
    lightLeds(FlasherTask::All, false) ;
    DEBUG("Flasher = %d, RGBI=%X\n", rate, cFlasherColour);

  }

}

// function: stop()
void FlasherTask::stop(Colour colour) {

  cFlasherColour = colour ;
  lightLeds(colour, true) ;

  bFlasherRunning = false ;
  bBlinkRunning = false ;
  bOverrideRunning = false ;
  
  iFlasherState = 0 ;
  iFlasherCode = 0 ;
  iTimeout = 0 ;

  iBlinkTimeout = 0 ;

  DEBUG("Flasher = Stop, RGBI=%X\n", colour);
}


