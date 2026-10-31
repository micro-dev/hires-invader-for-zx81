/*
 * timing.h  -  keeping time with the TV frame counter  (part of zx81lib)
 *
 *  The ZX81 counts TV frames (50 a second in the UK) in a system
 *  variable called FRAMES at address 16436.  It counts DOWN.
 *
 *  Watch out: in WRX hi-res mode FRAMES goes down by 2 every frame,
 *  because both the ZX81's ROM and z88dk's hi-res display routine
 *  count it.  wait_frames() allows for that, so its "n" is real
 *  TV frames.
 */
#ifndef ZX81LIB_TIMING_H
#define ZX81LIB_TIMING_H

#define FRAMES (*(unsigned char *)16436)

/* Wait until at least n TV frames have gone by since the last call.
 * Calling wait_frames(2) at the top of a game loop gives a steady
 * 25 loops a second however much (or little) the loop does - as long
 * as one loop's work fits in 2 frames.                                */
void wait_frames(unsigned char n);

#endif
