# OpenInputLagPatch

A **heavily work-in-progress** replacement for [vpatch](https://ux.getuploader.com/swmplv75e/), which fixes frame limiter and input lag issues in Touhou games.

# Usage
1. Compile as x86/Release
2. Copy openinputlagpatch.dll and oilp_loader.exe to your game directory
3. *(Touhou 9.5 and below)* Install [d3d8to9](https://github.com/crosire/d3d8to9) **(The ENB converter will not work!)**
4. Run oilp_loader.exe

# Compatibility
Not every game is supported, but support for more games is actively being worked on.
| Game                            | Supported | Replay speed control |
|---------------------------------|-----------|----------------------|
| Embodiment of Scarlet Devil     |✅|✅|
| Perfect Cherry Blossom          |✅|✅|
| Imperishable Night              |✅|✅|
| Phantasmagoria of Flower View   |✅|❌|
| Shoot the Bullet                |❌|❔|
| Mountain of Faith               |✅|✅|
| Uwabami Breakers                |❌|❔|
| Subterranean Animism            |✅|✅|
| Undefined Fantastic Object      |✅|✅|
| Double Spoiler                  |❌|❔|
| Great Fairy Wars                |❌|❔|
| Ten Desires                     |✅|✅|
| Double Dealing Character        |✅|✅|
| Impossible Spell Card           |❌|❔|
| Legacy of Lunatic Kingdom       |✅|✅|
| Hidden Star in Four Seasons     |✅|✅|
| Violet Detector                 |❌|❔|
| Wily Beast and Weakest Creature |❌|❔|
| Unconnected Marketeers          |❌|❔|
| Danmakufu v0.12m                |❌|❔|
| Danmakufu ph3                   |❌|❔|

# Configuration
OpenInputLagPatch is configured via a file called `openinputlagpatch.ini` stored in the same directory as the game executable. The game will run perfectly fine without the config file, but there's always the option of manually creating one. Here's an example config file:
```
[Option]

; Target framerate of the game
; Going above 60 doesn't make the game any smoother, it just makes it faster
; Default: 60
GameFPS = 60

; Allow controlling the speed of replay playback
; This will override the in-game replay speed control if it exists
; Default: 1
ReplaySpeedControl = 1

; Target framerate for replay skipping (hold LCTRL during replay playback)
; Default: 240
ReplaySkipFPS = 240

; Target framerate for replay slowdown (hold LSHIFT during replay playback)
; Default: 30
ReplaySlowFPS = 30

; Amount of time in milliseconds before screen refresh to try running the game logic
; Lower is better, but it comes with a higher risk of the game not being able to run in time and causing more input lag
; Default: 4
BltPrepareTime = 4

; Use Direct3D9Ex instead of Direct3D9 to allow reducing input lag by 0 to 2 frames
; Has no effect on Touhou 9.5 and below if d3d8to9 isn't being used
; Default: 1
D3D9Ex = 1

; Chooses the refresh rate used by the game. Only works if D3D9Ex is enabled
; 0: Maximum refresh rate, 1: 60hz, 2: Largest multiple of 60 (60hz, 120hz, 180hz, 240hz, etc.)
; Default: 2
FullscreenRefreshRate = 2

; Opens a console for debugging purposes
; Default: 0
DebugConsole = 0

; Opens a message box on boot to allow attaching a debugger
; Default: 0
DebugWait = 0

; Fixes the input glitching issues that can occur in EoSD and PCB if you deselect the window while the game is loading for example
; Default: 0
FixInputGlitching = 0

; Overrides game detection to a specific game
; See games.h for possible values
; CHANGING THIS WILL PROBABLY BREAK EVERYTHING. Only use this if you are 100% sure that the game's offsets are exactly the same!
; Default: -1
GameOverride = -1
```

# Technical details
*(Touhou 6-7 only)* The game loop is modified to run the drawing logic *after* the game update logic instead of the other way around, which should shave off a frame of input lag.

`Direct3D9Create` is hooked to use `Direct3D9CreateEx` instead, which allows the use of `IDirect3DDevice9Ex::SetMaximumFrameLatency`, which should shave off an additional 0 to 2 frames of input lag.

Finally, the in-game frame limiter is disabled and a heavily simplified version of vpatch's frame limiter is used.

# TODO
- Support more games
- Take a look at vpatch's `AutoBltPrepareTime` algorithm
- Probably more stuff I forgot
