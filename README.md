# Introduction

Flasher is an Arduino class object which can manage a builtin LED and
a multi-colour LED - setting colours and managing flashing.

# Author

Flasher is created by Steve Clarke
(c) Steve Clarke, Vizier Design 2023

# Source Code

Flasher sourcecode is available at: https://www.github.com/trumpton/A38-Flasher

# License

Released under the GNU AFFERO GENERAL PUBLIC LICENSE
See LICENSE file

# Overview

## Process Task (required in the loop() function)

  Flasher.ProcessTask() ;

## Setup Flasher GPIO Ports and sense

  Flasher.selectLeds(REDGPIO, false, GREENGPIO, false, BLUEGPIO, false) ;

## Configure to Repeatedly flash 2 pulses

  Flasher.set(2, FlasherTask::Blue) ;

## Override Flashing with colour

  Flasher.override(true, FlasherTask::Orange) ;
  Flasher.override(false) ; // Stop overriding

## Stop Flashing

  Flasher.stop(FlasherTask::Off) ;

## Blink with colour

  Flasher.blink(500, FlasherTask::Teal) ;


