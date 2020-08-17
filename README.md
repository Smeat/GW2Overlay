# GW2Overlay

This is aimed to be a (very basic) GW2 overlay which runs natively on Linux as an alternative to GW2TaCo.
The GW2 data is sent via UDP from a python script inside the wine environment, since using mmap with wine is not straightforward.

## Usage
 1. Start GW2
 2. _After_ GW2 is launched, start the "mumble.py" script inside the same wine prefix
 3. Launch the Overlay
 
  I've tried launching the python program with a batch script with 20 sec delay, but that didn't work. I always need to open a wine console after GW2 is up an running. Not sure why

### Status
 * Current status: Very early PoC
 * Can load and display GW2TaCo XML POIs
 * Tested on Arch with i3wm
 * Position is currently hardcoded for my setup
 
### TODO
 * Some kind of GUI for settings etc.
  * Boss timer
  * API key (daily)
 * Make the POIs always face the camera
 
### BUGS
 * GW2 doesn't report the UP vector, therefore the perspective is not perfect and the POIs are moving
  * Currently only a very simple approach is implemented
