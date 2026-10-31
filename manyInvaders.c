/*
 * =====================================================================
 *   SINGLE INVADER  -  one alien drawn exactly the way invaders.c does
 * =====================================================================
 *
 *  The smallest program using zx81lib: it puts one crab invader in the
 *  middle of a black hi-res screen, using the same steps as the game:
 *
 *    1. find the hi-res screen that z88dk set up         (screen_init)
 *    2. fill it black - on the ZX81 a 1 bit is black     (clear_screen)
 *    3. make 4 ready-shifted copies of the picture       (make_fast)
 *    4. copy the right one onto the screen               (draw_fast)
 *
 *  HOW TO BUILD  (or run build.bat)
 *
 *      zcc +zx81 -subtype=wrx -clib=wrx -O2 -Cz--collapsed singleInvader.c
 *          zx81lib/screen.c zx81lib/sprite.c -o singleInvader -create-app
 *
 *  Run singleInvader.P in EightyOne with 16K RAM and WRX hi-res on.
 * =====================================================================
 */

#pragma output CRT_ENABLE_STDIO = 0     /* no printf() / file handling  */
#pragma output CLIB_FOPEN_MAX = 0
#pragma output noslowfix = 1            /* no ZX81 text mode needed     */
#pragma output fputc_cons = 0           /* no text console (zx81lib draws its own letters) */

#include "zx81lib/screen.h"   /* the hi-res screen  */
#include "zx81lib/sprite.h"   /* drawing pictures   */


/* The crab: width, height, then 2 bytes per row (left pixel = top bit) */
const unsigned char crab_a[] = {
    12, 8,   /* width, height */
    0x20, 0x80,   /*   X     X    */
    0x11, 0x00,   /*    X   X     */
    0x3F, 0x80,   /*   XXXXXXX    */
    0x6E, 0xC0,   /*  XX XXX XX   */
    0xFF, 0xE0,   /* XXXXXXXXXXX  */
    0xBF, 0xA0,   /* X XXXXXXX X  */
    0xA0, 0xA0,   /* X X     X X  */
    0x1B, 0x00,   /*    XX XX     */
};

unsigned char fast_crab[FAST_SIZE];      /* its 4 pre-shifted copies */

void main(void)
{
    screen_init();                       /* find the hi-res screen     */
    clear_screen();                      /* black sky                  */
    make_fast(crab_a, fast_crab);        /* 4 ready-shifted copies     */

    for (int x=0; x<12; x++) {
       draw_fast(10+x*20, 92, fast_crab);      
    }

    while (1) { }                        /* stay here, showing it      */
}
