
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
#include "kiss_fftr.h"
#include <fstream>
#include "response_measurement.h"

#include "my_fft.h"
#include "fft_convolver.h"
#include <math.h>
#include "dynamic_bass_boost2.h"
using namespace std;

void irUnitTest() {

	float fs = 48e3;
	float f1 = 1;
	float f2 = 20e3;
	float duration = 1;
	size_t len = duration * fs;
	float* ess = generateExpSineSweep(duration, f1, f2, fs);
	float* yFiltered = (float*)malloc(sizeof(float)*len);
	//Biquad* filter = createBiquad(fs, State2FilterCoeffs[0], State2FilterCoeffs[1]);

	float coefs[2][3] = { 
		{ 0.00391612666054739,	0.00783225332109477,	0.00391612666054739} ,
		{ 1, - 1.81534108270457,	0.831005589346757 }
	};
	Biquad* filter = createBiquad(fs,coefs[0],coefs[1]);
	for (size_t n = 0; n < len; n++)
	{
		runBiquad(filter, ess[n], yFiltered[n]);
	}
	float* ir;
	float* mag_db =  findSystemIR(yFiltered, duration, f1,f2, fs,ir);
	


	fstream fo;
	fo.open("biquad out5.txt", ios::out);
	for (size_t n = 0; n < len+1 ; n++)
	{
		fo << mag_db[n]<< "\n";
	}
	//wavfile_write_f32("inv ESS  impulse.wav", &impulse, 48000, 1, 48000);
	fo.close();
	free(ess);
	free(yFiltered);
	free(mag_db);
	free(ir);
	// test passed
}

void dbbUnitTest() {
	// This is the DBB algorithm test.
	// generate exponental sine sweep signal 1~20kHz,duration 1 s
	float fs = 48e3;
	float f1 = 1;
	float f2 = 20e3;
	float duration = 1;
	size_t len = duration * fs;
	float* ess = generateExpSineSweep(duration, f1, f2, fs);

	float* yFiltered = (float*)malloc(sizeof(float)*len);
	DBB* dbb = createDBB(fs);
	for (size_t n = 0; n < len; n++)
	{
		runDBB(dbb, ess[n], yFiltered[n]);
	}
	float* ir;
	float* ir_mag = findSystemIR(yFiltered, duration, f1, f2, fs,ir);
	
	// output the response of the DBB system
	fstream fo;
	fo.open("dbb mag full scale.txt", ios::out);
	for (size_t n = 0; n < len + 1; n++)
	{
		fo << ir_mag[n] << "\n";
	}
	fo.close();
	free(ess);
	free(yFiltered);
	free(ir_mag);
	free(ir);
	freeDBB(dbb);

}
void dbb2UnitTest_refactor() {
	// generate exponental sine sweep signal
	float fs = 48e3;
	float f1 = 1;
	float f2 = 20e3;
	float duration = 1;
	size_t len = duration * fs;
	float* ess = generateExpSineSweep(duration, f1, f2, fs);
	float* essProcessed = (float*)malloc(sizeof(float)*len);

	// your processing here
	float boostFreq = 60;
	float boostGain = 16;
	DBB2* dbb = createDbb2(boostFreq, boostGain, 1.5, fs);
	for (size_t n = 0; n < len; n++)
	{
		runDbb2(dbb, 0.02*ess[n], essProcessed[n]);
	}

	freeDbb2(dbb);
	//

		// find the impulse response of DBB 
	float* ir;
	float* ir_mag = findSystemIR(essProcessed, duration, f1, f2, fs,ir);
	fstream fo;
	fo.open("dbb2 mag full scale refactor 002.txt", ios::out);
	for (size_t n = 0; n < len + 1; n++)
	{
		fo << ir_mag[n] << "\n";
	}
	fo.close();
	free(ir_mag);
	free(ir);
	free(ess);
	free(essProcessed);

}
int main()
{
	dbb2UnitTest_refactor();

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