# Player poses

Each character directory contains the five PNG files loaded by the game:

- `idle.png`
- `walk1.png`
- `walk2.png`
- `jump.png`
- `fall.png`

Files may use any canvas size, but all five poses for one character must have
the same width and height. Use a transparent RGBA canvas, keep the character's
feet on the same baseline in every pose, and draw the character facing right.
The renderer flips the image when the player faces left.

The current files are runtime copies from the CC0 Kenney Platformer Characters
pack retained under `assets/vendor/kenney-platformer-characters`.
