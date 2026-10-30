/*
 * screen.h  -  the ZX81 WRX hi-res screen  (part of zx81lib)
 *
 *  The hi-res screen is 256 x 192 pixels held in 6144 bytes of RAM:
 *  32 bytes per line, 192 lines.  The byte for pixel (x,y) is at
 *      screen + y*32 + x/8
 *  On the ZX81 a 1 bit is BLACK.  This library uses a black background
 *  and draws in white by turning bits off (like the arcade's look).
 *
 *  Call screen_init() once at the start of main().
 */
#ifndef ZX81LIB_SCREEN_H
#define ZX81LIB_SCREEN_H

#define SCREEN_WIDTH     256
#define SCREEN_HEIGHT    192
#define SCREEN_LINE      32       /* bytes per screen line */

extern unsigned char *screen;              /* start of the screen memory  */
extern const unsigned char bit_mask[8];    /* bit for pixel 0..7 of a byte */

/* Find where z88dk put the screen.  Call this first. */
void screen_init(void);

/* Fill the whole screen black. */
void clear_screen(void);

/* Paint a block of the screen with one byte value: 0xFF = black,
 * 0x00 = white.  "col" and "width" are in BYTES (8 pixels each), "y"
 * and "height" in pixels.  clear_block() paints it black, fill_block()
 * white.  (They are macros, which saves memory.)                     */
void paint_block(unsigned char col, unsigned char y,
                 unsigned char width, unsigned char height,
                 unsigned char value);
#define clear_block(col, y, width, height)  paint_block(col, y, width, height, 0xFF)
#define fill_block(col, y, width, height)   paint_block(col, y, width, height, 0x00)

/* The address of the byte holding pixel (x,y). */
unsigned char *pixel_address(unsigned char x, unsigned char y);

/* 1 if the pixel at (x,y) is lit (white), 0 if black. */
#define pixel_lit(x, y)  ((*pixel_address((x), (y)) & bit_mask[(x) & 7]) == 0)

#endif
