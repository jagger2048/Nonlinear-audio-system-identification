
//#include <iostream>
#include <stdio.h>
#include <stdint.h>

//#include "frame.h"
//#include "stream.h"
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "wavfile.h"
#include "dr_wav.h"

#include <fstream>
#include <iostream>
#include "limiter.h"
using namespace std;
int main()
{
	FILE *fp;
	ofstream fo;
	fo.open("limiter.txt", ios::out);

	printf("This is the signal processing library test case.\n");
	WAV *wavfile = wavfile_create();
	wavfile = wavfile_read("limit .wav");

	RmsLimiter* limiter = RmsLimiter_create(
											-15,	// T
											5,		// ratio
											1,		// kneeWidth
											0.05,	// at
											0.2,	// rt
											0,		// makeup gain
											wavfile->sampleRate	);
	size_t numSamples = wavfile->totalPCMFrameCount;
	float* output = (float*)malloc(sizeof(float) * numSamples);
	float linearOutput = 0;
	float linearInput = 0;
	for (float n = -50; n < 10; n+=0.01)	// Test case: -50 dB->10 dB step 0.1 dB
	{
		//tmp = RmsLimiter_process(limiter, wavfile->pDataFloat[0][n], output[n]);
		//RmsLimiter_process(limiter, powf(10,n/20.0f), tmp);
		linearInput = powf(10, n / 20.0f);
		RmsLimiter_process(limiter, linearInput, linearOutput);
		fo << linearInput <<"    "<< linearOutput <<"    "<<20*log10(fabsf(linearOutput)) <<endl;
		cout<< linearInput << "    " << linearOutput << "    " << 20 * log10(fabsf(linearOutput)) << endl;
	}
	//wavfile_write_f32("limit3 .wav", &output, numSamples, 1, wavfile->sampleRate);
	wavfile_destory(wavfile);
	fo.close();// close
	free(output);
	return 0;
}


/*
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
*/