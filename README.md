# GW2Overlay

This is aimed to be a (very basic) GW2 overlay which runs natively on Linux as an alternative to GW2TaCo.
The GW2 data is sent via UDP from a python script inside the wine environment, since using mmap with wine is not straightforward.

## Usage
 * Launch the `launch.sh` script with the path to the `GW2Overlay` binary
 * Start GW2

It shouldn't matter in which order you start the programs, but as there is a bit of work done to ensure the overlay is started at the correct time, it might help to start GW2 first and start the overlay once you are on the character select screen (or later).

### Status
 * Current status: Very early PoC
 * Can load and display GW2TaCo XML POIs
 * Tested on Arch with i3wm
 * Position is currently hardcoded for my setup
 
### TODO
 * Some kind of GUI for settings etc.
  * Boss timer
  * API key (daily)
 * Vulkan renderer
 
### BUGS
 * POIs are moving a little bit. Probably due to delay
 * GW2 doesn't report the UP vector, therefore the perspective is not perfect
  * The current imeplementation seems to be good enough
