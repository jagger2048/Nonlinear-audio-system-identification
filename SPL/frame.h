#pragma once
//#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>	// memmove	// should be optimized.


typedef struct {
	int16_t *sample_s16[2];		// 0 L ; 1 R
	float *sample_f32[2];

	size_t sample_rate;			// sample rate
	size_t frame_size;			// number of samples per chunk package
}spl_frame_st;

spl_frame_st * spl_frame_new(size_t sample_rate, size_t frame_size) {

	spl_frame_st * pFrame = (spl_frame_st *)malloc(sizeof(spl_frame_st ));
	for (size_t channel = 0; channel < 2; channel++)
	{
		pFrame->sample_f32[channel] = (float*)malloc(sizeof(float) * frame_size);
		pFrame->sample_s16[channel] = (int16_t*)malloc(sizeof(int16_t) * frame_size);
	}
	pFrame->sample_rate = sample_rate;
	pFrame->frame_size = frame_size;
	return pFrame;
}

void static inline spl_frame_load_s16(spl_frame_st * pFrame, size_t channel,void* dest, const void* src, size_t count) {
	// load |count| s16 fromat pcm samples data from |src| to |dest|
	memmove( pFrame->sample_s16[channel],src,count * sizeof(int16_t) );	// should be optimized.
}

void static inline spl_frame_load_f32(spl_frame_st * pFrame, size_t channel, void* dest, const void* src, size_t count) {
	// load |count| f32 fromat samples data from |src| to |dest| for |channel|
	memmove(pFrame->sample_f32[channel], src, count * sizeof(float) );	// should be optimized.
}

int spl_frame_free(spl_frame_st * pFrame) {
	if (pFrame !=NULL)
	{
		for (size_t channel = 0; channel < 2; channel++)
		{
			free(pFrame->sample_f32[channel]);
			free(pFrame->sample_s16[channel]);
		}
		free(pFrame);
		return 0;
	}
	return -1; 
}

