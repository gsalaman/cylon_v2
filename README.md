# Cylon on the "Tiny Stick", part deux

## Intro

In today's session with Jeff, there were a couple things I wanted to show, but my demo cylon code didn't have good examples:
* Doing code without embedded waits
* Using enums
* solid commenting

We've been playing around with getting a "cylon" (or knight-rider) working on the tiny stick...this project is gonna be my take at implementing that whilst demonstrating good software engineering principles.

## Requirements
Have an eye that moves right and left.
Speed configurable via the Poteniometer.

Eye length variable between 1 and (the number of leds - 1)
Start with this as configurable via button-presses.
Eventually go to a more complex system where holding the button goes into a mode where you then set speed (via pot) and then length (also via pot).

Eye color starts compile-time set.
Background color start compile-time set. 
(MAY eventually change this to react to a button press...same complex system as above, but two more vars for eye and background color)

Eye length goes from eye color (head) to background color via a color interpolation.  Reuse this from a prior project...and package it up so that others can also use it.  Start with it in a .h/.cpp file in the same folder if that works...

When the "eye head" hits an edge, it's gonna wait there until the rest of the "tail" gets aborbed into the edge.  Here's a text example, where I'm using "numbers" as the various eye colors.  The head eye color is "1", the next "2", the next "3", the next "4", and background color is "5".  This is a Eye of length 5.  I'm listing each iteration on a line, with 8 numbers representing the LED spots.


```
5 4 3 2 1 5 5 5
5 5 4 3 2 1 5 5
5 5 5 4 3 2 1 5
5 5 5 5 4 3 2 1
5 5 5 5 5 4 3 1  <- Tail starts "collapsing".  Favor lower end.
5 5 5 5 5 5 4 1
5 5 5 5 5 5 5 1   <- Tail completely eaten
5 5 5 5 5 5 1 2   <- Tail starts too grow again
5 5 5 5 5 1 2 3
5 5 5 5 1 2 3 4
5 5 5 1 2 3 4 5
```
## Eye motion design
### Option 1:  "states of motion"
In this design option, we have three states for eye motion:
1) The eye is completly on the screen and moving either left or right.
2) The eye is at an edge, and the tail is being absorbed
3) We've reversed diretion, and the tail is being created.

It may make sense to have a "left" and "right" state for all three of these, or a single "left"/"right" variable that these three use.  

### Option 2:  virtual window
In this case, we make the "virtual window" of the eye bigger than the actual 8 leds...big enough to contain the full tail on either side.  We only display the LEDs that are in our "real" window, but if the eye is off one side or another, we override that end pixel with the main eye color.

In this option, the virtual window size is the size of the real window, plus two times our max eye size.  In our current requirements, it's 8+7+7 or 22.

This looks like the following:
```
                     0  1  2  3  4  5  6  7                            <- Real window index
0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20 21 22.   <- Virtual window index
```
Which means that the real window index is the virtual index minus 7.  



