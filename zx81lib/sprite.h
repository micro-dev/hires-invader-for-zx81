/*
 * sprite.h  -  drawing small pictures on the hi-res screen  (part of zx81lib)
 *
 *  A sprite is:  width, height, then the rows of pixels, each row in
 *  whole bytes (a 12-pixel row takes 2 bytes, left pixel = top bit).
 *  Sprites can be up to 16 pixels wide.
 *
 *  draw_sprite()  draws in white (turns the sprite's bits OFF)
 *  erase_sprite() puts black back (turns them ON again)
 *
 *  PRE-SHIFTED ("fast") SPRITES
 *  For sprites that only ever stand on EVEN x positions, make_fast()
 *  builds four ready-shifted copies (shifted 0, 2, 4 and 6 pixels) in a
 *  FAST_SIZE-byte table.  draw_fast()/erase_fast() then just copy bytes,
 *  about twice as fast.  x MUST be even for the fast versions.
 */
#ifndef ZX81LIB_SPRITE_H
#define ZX81LIB_SPRITE_H

#include "screen.h"

void draw_sprite (unsigned char x, unsigned char y, const unsigned char *spr);
void erase_sprite(unsigned char x, unsigned char y, const unsigned char *spr);

/* A pre-shifted table is 97 bytes:
 *     [0]       number of rows (8 at most)
 *     [1..24]   the rows shifted by 0 pixels, 3 bytes per row
 *     [25..48]  shifted by 2,   [49..72] by 4,   [73..96] by 6        */
#define FAST_SIZE 97

void make_fast (const unsigned char *spr, unsigned char *out);
void draw_fast (unsigned char x, unsigned char y, const unsigned char *table);
void erase_fast(unsigned char x, unsigned char y, const unsigned char *table);

#endif
