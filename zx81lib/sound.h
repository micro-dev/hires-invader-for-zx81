/*
 * sound.h  -  simple sound effects for an AY sound card  (part of zx81lib)
 *
 *  The ZX81 has no sound of its own, so this uses the ZON X-81 add-on,
 *  which has an AY-3-8912 sound chip (the same as the 128K Spectrum).
 *  EightyOne emulates it: Options > Hardware, Sound Card: Zon-X.
 *  Without the card the program still runs - it is just silent.
 *
 *  The chip has three voices ("channels").  This library uses them as:
 *      channel 0 and 1 : musical tones
 *      channel 2       : noise (bangs and explosions)
 *
 *  Each sound starts at a volume (0 = silent .. 15 = loudest) and then
 *  fades away by itself: call sound_update() once every game tick.
 */
#ifndef ZX81LIB_SOUND_H
#define ZX81LIB_SOUND_H

/* The pitch number ("period") for a note of hz Hertz.  Bigger = lower.
 * e.g. SOUND_HZ(440) is the A above middle C.                         */
#define SOUND_HZ(hz)   ((50781u / (hz)) * 2)
            /* (= 101562 / hz, the chip's clock / 16, worked out in a
                way that fits 16-bit numbers and is done by the compiler) */

void sound_init(void);      /* silence everything - call at the start  */
void sound_update(void);    /* fade the sounds - call once every tick  */

/* Start a tone on channel 0 or 1 */
void sound_tone(unsigned char channel, unsigned int period,
                unsigned char volume);

/* Start a noise on channel 2.  pitch 0 (hiss) .. 31 (rumble) */
void sound_noise(unsigned char pitch, unsigned char volume);

#endif
