/*
 * =====================================================================
 *   WALK INVADER  -  one alien marches across the screen
 * =====================================================================
 *
 *  A crab invader walks from the left edge of the screen to the right,
 *  waving its legs, using the same steps as the game:
 *
 *    1. make ready-shifted copies of both pictures       (make_fast)
 *    2. every tick: rub it out, move it, draw it again   (erase_fast,
 *                                                         draw_fast)
 *    3. swap between its two pictures to make it walk
 *
 *  For the first quarter of the way it plays the game's four-note
 *  marching "thump".  Over the rest of the walk it plays each of the
 *  game's other five sounds once, evenly spread out:
 *      the player's shot, an invader exploding, the mystery ship's
 *      whine, the mystery ship exploding, and the base exploding.
 *
 *  HOW TO BUILD  (or run build.bat)
 *
 *      zcc +zx81 -subtype=wrx -clib=wrx -O2 -Cz--collapsed walkInvader.c
 *          zx81lib/screen.c zx81lib/sprite.c zx81lib/timing.c
 *          zx81lib/sound.c -o walkInvader -create-app
 *
 *  Run walkInvader.P in EightyOne with 16K RAM and WRX hi-res on.
 *  To hear it: Options > Hardware, set "Sound Card" to Zon-X.
 * =====================================================================
 */

#pragma output CRT_ENABLE_STDIO = 0     /* no printf() / file handling  */
#pragma output CLIB_FOPEN_MAX = 0
#pragma output noslowfix = 1            /* no ZX81 text mode needed     */
#pragma output fputc_cons = 0           /* no text console (zx81lib draws its own letters) */

#include "zx81lib/screen.h"   /* the hi-res screen      */
#include "zx81lib/sprite.h"   /* drawing pictures       */
#include "zx81lib/timing.h"   /* the TV frame counter   */
#include "zx81lib/sound.h"    /* sound effects          */


/* The crab's two pictures: width, height, then 2 bytes per row */
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

const unsigned char crab_b[] = {
    12, 8,   /* width, height */
    0x20, 0x80,   /*   X     X    */
    0x91, 0x20,   /* X  X   X  X  */
    0xBF, 0xA0,   /* X XXXXXXX X  */
    0xEE, 0xE0,   /* XXX XXX XXX  */
    0xFF, 0xE0,   /* XXXXXXXXXXX  */
    0x7F, 0xC0,   /*  XXXXXXXXX   */
    0x20, 0x80,   /*   X     X    */
    0x40, 0x40,   /*  X       X   */
};

unsigned char fast_crab_a[FAST_SIZE];    /* their 4 pre-shifted copies */
unsigned char fast_crab_b[FAST_SIZE];

#define START_X     0                    /* left edge of the screen    */
#define END_X     244                    /* 256 - 12: right edge       */
#define QUIET_X   (START_X + (END_X - START_X) / 4)   /* quarter way   */
#define WALK_Y     92                    /* half way down the screen   */
#define STEP        2                    /* pixels per tick (even!)    */

/* The game's march: four low notes, played one after another */
const unsigned int march_notes[4] = { SOUND_HZ(73), SOUND_HZ(65),
                                      SOUND_HZ(58), SOUND_HZ(52) };


/* =====================================================================
 *  THE GAME'S OTHER SOUND EFFECTS  (the same notes and noises as in
 *  invaders.c).  Some last longer than a moment, so each one is played
 *  by calling update_effect() every tick, and "effect_age" counts the
 *  ticks since it started.
 * ===================================================================== */

#define NO_EFFECT       0
#define SHOT            1     /* the player's shot: "pew"              */
#define INVADER_BANG    2     /* an invader exploding                  */
#define UFO_WHINE       3     /* the mystery ship flying over          */
#define UFO_BANG        4     /* the mystery ship exploding            */
#define BASE_BANG       5     /* the player's base exploding           */
#define NUM_EFFECTS     5

#define BASE_BANG_TICKS 40    /* how long the base explosion lasts     */
#define UFO_WHINE_TICKS 16    /* how long we let the whine play        */

unsigned char effect;         /* which effect is playing               */
unsigned char effect_age;     /* ticks since it started                */

void start_effect(unsigned char which)
{
    effect = which;
    effect_age = 0;
}

/* The base explosion, exactly as in invaders.c: a crackling noise that
 * gets deeper and fades, with a falling tone underneath.  age = 1..40 */
void base_explosion_sound(unsigned char age)
{
    unsigned char vol;

    vol = (BASE_BANG_TICKS - age) >> 1;   /* fades as it goes on      */
    if (vol > 15) vol = 15;

    sound_noise(6 + (age >> 1) + ((age & 1) << 2), vol);
    sound_tone(1, SOUND_HZ(220) + (age << 5), vol >> 1);
}

/* Called every tick: keeps the current effect going */
void update_effect(void)
{
    effect_age++;

    if (effect == SHOT && effect_age == 1) {
        sound_tone(1, SOUND_HZ(1200), 10);              /* pew          */
    }
    if (effect == INVADER_BANG && effect_age == 1) {
        sound_noise(4, 13);                             /* crunch!      */
    }
    if (effect == UFO_WHINE && effect_age <= UFO_WHINE_TICKS) {
        /* swap between two notes every 2 ticks, like the flying UFO */
        if (effect_age & 2) sound_tone(1, SOUND_HZ(600), 10);
        else                sound_tone(1, SOUND_HZ(500), 10);
    }
    if (effect == UFO_BANG && effect_age == 1) {
        sound_noise(2, 15);                             /* big bang     */
    }
    if (effect == BASE_BANG && effect_age <= BASE_BANG_TICKS) {
        base_explosion_sound(effect_age);
    }
}


/* =====================================================================
 *  THE WALK
 * ===================================================================== */

/* The last 3/4 of the walk is split into 5 equal parts, one effect
 * starting in the middle of each part.                               */
#define EFFECT_GAP  ((END_X - QUIET_X) / NUM_EFFECTS)

void main(void)
{
    unsigned char x;          /* where the crab is                     */
    unsigned char frame;      /* which picture: 0 = a, 1 = b           */
    unsigned char ticks;      /* counts ticks, to time the legs        */
    unsigned char note;       /* which march note plays next           */
    unsigned char *picture;   /* the picture showing now              */
    unsigned char next_effect;    /* which effect plays next (1..5)    */
    unsigned char next_effect_x;  /* ... when the crab gets here       */

    screen_init();                       /* find the hi-res screen     */
    clear_screen();                      /* black sky                  */
    sound_init();                        /* sound card: all quiet      */
    make_fast(crab_a, fast_crab_a);      /* ready-shifted copies       */
    make_fast(crab_b, fast_crab_b);

    x = START_X;
    frame = 0;
    ticks = 0;
    note = 0;
    picture = fast_crab_a;
    draw_fast(x, WALK_Y, picture);

    effect = NO_EFFECT;
    next_effect = SHOT;
    next_effect_x = QUIET_X + EFFECT_GAP / 2;

    while (x < END_X) {
        wait_frames(2);                  /* 25 ticks a second          */
        sound_update();                  /* let the last sound fade    */
        update_effect();                 /* keep any effect going      */

        erase_fast(x, WALK_Y, picture);  /* rub out ...                */
        x = x + STEP;                    /* ... move ...               */

        /* every 4 ticks (8 pixels) it takes a step: swap pictures */
        ticks++;
        if (ticks == 4) {
            ticks = 0;
            frame = !frame;

            /* ... and in the first quarter, thump out the next note */
            if (x < QUIET_X) {
                sound_tone(0, march_notes[note], 15);
                note = (note + 1) & 3;          /* 0,1,2,3,0,1,...   */
            }
        }

        /* in the last three quarters, start each effect in turn */
        if (next_effect <= NUM_EFFECTS && x >= next_effect_x) {
            start_effect(next_effect);
            next_effect++;
            next_effect_x = next_effect_x + EFFECT_GAP;
        }

        if (frame) picture = fast_crab_b;
        else       picture = fast_crab_a;
        draw_fast(x, WALK_Y, picture);   /* ... draw it again          */
    }

    /* The base explosion is still going when the crab arrives, so
       carry on ticking until it has finished                         */
    while (effect_age < BASE_BANG_TICKS) {
        wait_frames(2);
        sound_update();
        update_effect();
    }

    sound_init();                        /* make sure all is silent    */
    while (1) { }                        /* stay at the right edge     */
}
