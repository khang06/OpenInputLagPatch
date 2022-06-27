# OpenInputLagPatch

A **very unfinished** replacement for [vpatch](https://ux.getuploader.com/swmplv75e/), which fixes frame limiter and input lag issues in Touhou games.

Currently, only EoSD v1.02h and MoF v1.00a are supported. Porting support for other games should be very easy, though.

# Usage
1. Compile as x86/Release
2. Rename the output to dinput8.dll
3. Copy the DLL to your game directory
4. *(Touhou 9.5 and below)* Install [d3d8to9](https://github.com/crosire/d3d8to9) **(The ENB converter will not work!)**
4. Run the game as usual

# Technical details
The game loop is modified to disable the in-game frame limiter.

*(Touhou 6 only)*  The game loop is modified to run the drawing logic *after* the game update logic instead of the other way around, which should shave off a frame of input lag.

`Direct3D9Create` is hooked to use `Direct3D9CreateEx` instead, which allows the use of `IDirect3DDevice9Ex::SetMaximumFrameLatency`, which should shave off an additional 0 to 2 frames of input lag.

Finally, a very simplified version of vpatch's frame limiter is used with a hardcoded `BltPrepareTime` of 2 ms and without `AutoBltPrepareTime`.

# TODO
- Support more games
- Figure out why `D3DProxyWindow` gets stuck on the screen when switching from fullscreen to windowed with fullscreen optimizations enabled
- Make it more configurable
- Take a look at vpatch's `AutoBltPrepareTime` algorithm
- Probably more stuff I forgot