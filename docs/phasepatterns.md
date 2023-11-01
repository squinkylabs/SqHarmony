# Squinktronix Phasepatterns manual

This module is experimental at the moment. This means:

* The panel is very ugly.
* The feature set is likely to change.
* The patch format may change, such that patches saved with this module may not load correctly in future versions.

## What it does

Phasepatterns is very simple - it takes an input clocks and delays it. The delay is in metric units, not absolute time. So, when the input clock rate changes, so does the shift amount. So, here is a simple example:

* Regular clocks running into the clock input.
* Shift set to .5.

The output clock will be at the same rate (tempo) as the input clock, but it will be delayed by half a clock. If the clocks are perhaps quarter notes, the output will be shifted/delayed by an eighth note.

There is also a built in shift generator that will over the course of 8 clocks, delay the output by one clock. This is referred to as the "RIB generator", for "Reich In a Box", the original name of the modules.

## Things that may change

Hopefully users will suggest new features, and they will be implemented.

The RIB generator is not very flexible.

RIB generator could use a trigger input.

RIB button take effect immediately. It might be nice to wait for a clock input before starting?

The CV convention of one cycle == 1.0 may not give a wide enough range in many applications

## Possible uses

Used as a simple metric delay, it will add an offset clock that can be adjusted to be a "swing" or an echo.

If the delay is slowly changed during playback, "phasing" effects can be easily introduced (and controlled!) as in the music of Steve Reich. Some of that is explained [here](https://en.wikipedia.org/wiki/Piano_Phase). Note that the name of this module is in fact borrowed from the name of a Reich composition. A review of that recording can be found [here](https://pitchfork.com/reviews/albums/21584-four-organs-phase-patterns/).

Please remember that there are many potential uses of this module - recreating the compositions of Steve Reich is just one of them.

## Panel

The various controls and displays are all to compute and modulate the shift amount, and to display the shift amount. We use an arbitrary convention here that 1.0 is the duration of a lock. so shift = .25 would delay by a quarter of a clock. Shift 2.5 would delay by two clocks and a half. This 0.0 to 1.0 range would be 0 to 2*pi in radians, or 0 to 360 degrees in cartesian coordinates.

Next to the shift controls is a display that shows the current shift amount being applied. The shift amount is basically the sum of the shift knob, the shift control voltage, and the output of the "RIB generator".

### The inputs

Clock input. This is the clock that Phasepatterns will lock onto and delay. It follows the VCV standard - any input above 1.0 is treated as a high clock, and any input below .1 is treated as a low clock. In practice this means you can run most anything into it.

Shift amount input is combined with the other shift inputs to give the final shift amount. Here 0v is "no shift", 1v is "shift of one", which as explained above is one clock". The shift amount input responds to positive voltage.

### The outputs

Clock output is the only output of the module. If follows the VCV recommendations where output low is 0, output high is 10.

### The controls

Shift amount is a fixed shift that is added to the other shift sources. It currently has a range of 0..4.

The RIB button triggers a new varying shift cycle when it is pressed. This causes the LED to go on and the shift will start to increase. Once the shift had increased by one, the RIB generator stops.
