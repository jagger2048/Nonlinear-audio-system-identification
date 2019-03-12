
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
#include "rms_limiter.h"
#include "peak_limiter.h"

#include "dynamic_bass_boost.h"
using namespace std;
int main()
{
	WAV* wavfile;
	wavfile = wavfile_read("dukou_noReverb.wav"); // input the ess to test.
	size_t samplerate = wavfile->sampleRate;
	size_t totalSample = wavfile->totalPCMFrameCount;
	float* output = (float*)malloc(sizeof(float) * totalSample);	// mono data
	// DBB initialize
	DBB* db = createDBB(samplerate);
	Biquad *prefilter = createBiquad(samplerate, FreFilterCoeffs[0], FreFilterCoeffs[1]);
	Biquad *state1Filter = createBiquad(samplerate, State1FilterCoeffs[0], State1FilterCoeffs[1]);
	Biquad *state2Filter = createBiquad(samplerate, State2FilterCoeffs[0], State2FilterCoeffs[1]);
	RmsLimiter* state1Limiter = createRmsLimiter(-15, 1, 0.05, 0.2, 0, samplerate);
	Compressor* state2SoftClip = createCompressor(-6, 6, 5, 0.05, 0.2, 0, samplerate);// 4.4
	float tmp1 = 0;
	float tmp2 = 0;
	float tmp3 = 0;
	float state1 = 0;
	for (size_t n = 0; n < totalSample; n++)
	{
		runDBB(db,wavfile->pDataFloat[0][n], output[n]);
		//runBiquad(prefilter, wavfile->pDataFloat[0][n] , tmp1);
		//runBiquad(state1Filter, tmp1, tmp2);
		//output[n] = tmp1 + tmp2;

		//output[n] = tmp1;
	}
	wavfile_write_f32("DBB  output.wav", &output, totalSample, 1, samplerate);

	wavfile_destory(wavfile);
	freeDBB(db);
	freeBiquad(prefilter);
	freeBiquad(state1Filter);
	freeBiquad(state2Filter);
	freeRmsLimiter(state1Limiter);
	freeCompressor(state2SoftClip);
	return 0;
}
/*
	FILE *fp;
	ofstream fo;
	fo.open("limiter.txt", ios::out);

	printf("This is the signal processing library test case.\n");
	WAV *wavfile = wavfile_create();
	wavfile = wavfile_read("limit .wav");

	RmsLimiter* limiter = createRmsLimiter(
											-21,	// T
											5,		// kneeWidth
											0.05,	// at
											0.2,	// rt
											0,		// makeup gain
											wavfile->sampleRate	);
	size_t numSamples = wavfile->totalPCMFrameCount;
	float* output = (float*)malloc(sizeof(float) * numSamples);
	float linearOutput = 0;
	float linearInput = 0;
	for (float n = -50; n < 10; n+=0.0001)	// Test case: -50 dB->10 dB step 0.1 dB
	{
		linearInput = powf(10, n / 20.0f);	// db -> v
		runRmsLimiter(limiter, linearInput, linearOutput);
		fo << linearInput <<"    "<< linearOutput <<endl;
	}

	//wavfile_write_f32("limit3 .wav", &output, numSamples, 1, wavfile->sampleRate);
	wavfile_destory(wavfile);
	fo.close();// close
	free(output);
	freeRmsLimiter(limiter);
	return 0;

*/

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