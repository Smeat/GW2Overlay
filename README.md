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
 * OpenGL and Vulkan renderers both working
   * CPU usage with Vulkan is about 50% less than with OpenGL
 
### TODO
 * Some kind of GUI for settings etc. (Very basic one is implemented)
   * Boss timer
   * API key (daily)
 * Vulkan specific
   * Depth buffer
   * Draw everything in a single cmd (might also fix the bug after creating the queue)
   * Proper color blending
 
### BUGS
 * POIs are moving a little bit. Probably due to delay
 * GW2 doesn't report the UP vector, therefore the perspective is not perfect
   * The current imeplementation seems to be good enough
 * Vulkan specific
   * For the first seconds after creating the new command buffers (when switching the displayed POIs) the overlay lags behind. After that it works perfectly
   * POI objects behind transparency look weird
  
 ![screenshot](https://user-images.githubusercontent.com/1054449/90751525-89bd8880-e2d6-11ea-9985-2ffa561d9049.png)
