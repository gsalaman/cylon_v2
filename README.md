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



