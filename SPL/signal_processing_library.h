#pragma once
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "frame.h"
// filter
int spl_biquad();
int spl_iir();
int spl_cascade_iir();
int spl_fir();

// delay
int spl_delay_make();
int spl_delay_get();
int spl_delay_run();

//chunk->samples[L][num]->sample_f32