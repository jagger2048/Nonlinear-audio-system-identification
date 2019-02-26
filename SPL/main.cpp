
//#include <iostream>
#include <stdio.h>
#include <stdint.h>

#include "frame.h"
#include "stream.h"
int main()
{
	printf("This is the signal processing library test case.\n");
	spl_frame_st *pFrame = spl_frame_new(48000, 1024);

	spl_stream_st *pStream = NULL;
	pStream = audio_stream_open("music_48k_32bit.wav");

	pcm_stream_frame_get(pStream,pFrame, 1024);
	for (size_t n = 0; n < 1024; n++)
	{
		printf("%d -- %f || %d -- %f\n",
			pFrame->sample_s16[0][n], 
			pFrame->sample_f32[0][n], 
			pFrame->sample_s16[1][n],
			pFrame->sample_f32[0][n]);
	}
	printf("test 2");
	audio_stream_close(pStream);
	return 0;
}


