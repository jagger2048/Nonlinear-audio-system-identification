#pragma once
#include "frame.h"
#include <stdint.h>

enum audio_stream_type
{
	PCM,	// pcm
	DSD,	// dsd
	DOP		// dsd over pcm
};
// pcm stream package
typedef struct {
	// audio info
	// raw stream
	// translate functions (optional)
	audio_stream_type stream_type;
	size_t nChannel;
	size_t byte_per_sample;
	int16_t *pData_s16;
}spl_stream_st, *spl_stream_p;


spl_stream_p audio_stream_open(FILE fp,const char* file_name); // return pcm frame stream
spl_frame_p audio_stream_translate(spl_stream_p fp,size_t frame_size);
int audio_stream_close(spl_stream_p pStream); // close audio stream

int open_pcm_stream_and_read_f32(spl_frame_p pFrame,size_t frame_size);
int open_pcm_stream_and_read_s16();
read_s16_from_pcm_stream

pcm_stream_frame_read