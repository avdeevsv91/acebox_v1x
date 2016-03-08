/*
 * tone.h
 *
 * Работа со звуком
 * Working with the sound.
 */


#ifndef TONE_H_
#define TONE_H_

#include "pitches.h"

void tone(unsigned int frequency, unsigned long duration);
void noTone();

#include "tone.c"

#endif /* TONE_H_ */
