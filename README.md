# OpenInputLagPatch

A **very unfinished** replacement for [vpatch](https://ux.getuploader.com/swmplv75e/), which fixes frame limiter and input lag issues in Touhou games.

Currently, only EoSD v1.02h and MoF v1.00a are supported. Porting support for other games should be very easy, though.

# Usage
1. Compile as x86/Release
2. Rename the output to dinput8.dll
3. Copy the DLL to your game directory
4. *(Touhou 9.5 and below)* Install [d3d8to9](https://github.com/crosire/d3d8to9) **(The ENB converter will not work!)**
4. Run the game as usual

# Configuration
OpenInputLagPatch is configured via a file called `openinputlagpatch.ini` stored in the same directory as the game executable. The game will run perfectly fine without the config file, but there's always the option of manually creating one. Here's an example config file:
```
[Option]

; Target framerate of the game
; Going above 60 doesn't make the game any smoother, it just makes it faster
; Default: 60
GameFPS = 60

; Amount of time in milliseconds before screen refresh to try running the game logic
; Lower is better, but it comes with a higher risk of the game not being able to run in time and causing more input lag
; Default: 4
BltPrepareTime = 4

; Use Direct3D9Ex instead of Direct3D9 to allow reducing input lag by 0 to 2 frames
; Has no effect on Touhou 9.5 and below if d3d8to9 isn't being used
; Default: 1
D3D9Ex = 1

; Forces fullscreen to use 60hz, even if your display can go higher than that
; Default: 0
Force60Hz = 0

; Opens a console for debugging purposes
; Default: 0
DebugConsole = 0

; Opens a message box on boot to allow attaching a debugger
; Default: 0
DebugWait = 0

; Overrides game detection to a specific game
; See games.h for possible values
; CHANGING THIS WILL PROBABLY BREAK EVERYTHING. Only use this if you are 100% sure that the game's offsets are exactly the same!
; Default: -1
GameOverride = -1
```

# Technical details
The game loop is modified to disable the in-game frame limiter.

*(Touhou 6 only)* The game loop is modified to run the drawing logic *after* the game update logic instead of the other way around, which should shave off a frame of input lag.

`Direct3D9Create` is hooked to use `Direct3D9CreateEx` instead, which allows the use of `IDirect3DDevice9Ex::SetMaximumFrameLatency`, which should shave off an additional 0 to 2 frames of input lag.

Finally, a heavily simplified version of vpatch's frame limiter without `AutoBltPrepareTime` is used.

# TODO
- Support more games
- Take a look at vpatch's `AutoBltPrepareTime` algorithm
- Probably more stuff I forgot