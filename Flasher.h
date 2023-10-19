//
// Flasher.h (A38-Flasher)
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


#ifndef FLASHERTASK_DEFINED
#define FLASHERTASK_DEFINED


class FlasherTask {

public:
  typedef enum {
    Off = 0,
    Red = 1,
    Orange = 3,
    Green = 2,
    Teal = 6,
    Blue = 4,
    Purple = 5,
    White = 7,
    Builtin = 8,
    All = 15
  } Colour ;


private:

  int iGpioRed, iGpioGreen, iGpioBlue, iGpioBuiltin ;
  bool bRedHigh, bGreenHigh, bBlueHigh, bBuiltinHigh ;
  bool bUseBuiltin ;

  int iFlasherState;
  int iFlasherCode;
  Colour cFlasherColour ;
  bool bFlasherRunning;
  unsigned long int iTimeout;

  bool bBlinkRunning ;
  unsigned long int iBlinkTimeout;
  Colour cBlinkColour ;

  bool bOverrideRunning ;
  Colour cOverrideColour ;

  bool develmode;
  bool verbose;

private:
  void lightLeds(Colour colour, bool state) ;

public:
  void setupTask();
  void processTask();

public:
  FlasherTask();
  void selectLeds( int gpioRed=-1, bool invertedRed=false, 
                   int gpioGreen=-1, bool invertedGreen=false,
                   int gpioBlue=-1, bool invertedBlue=false,
                   int gpioBuiltin=-1, bool invertedBuiltin=false) ;

  void setDevelMode(bool develmode = false, bool verbose = false);

  void set(int rate, Colour colour = FlasherTask::All);
  void blink(int msec, Colour colour = FlasherTask::All) ;
  void override(bool enable, Colour colour = FlasherTask::All) ;
  void stop(Colour colour = FlasherTask::Off) ;

};

extern FlasherTask Flasher;

#endif
