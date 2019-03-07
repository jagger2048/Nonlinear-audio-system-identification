#pragma once

// This file contains the algorithm to measure the frequency response of a 
// non-linear system, which based on exponential swept sine, ESS method.
#include <math.h>
#include <stdlib.h>
#ifndef M_PI
#define M_PI 3.141592654
#endif // !M_PI

float* generateExpSweptSin(float duration, float f1, float f2, size_t samplerate) {
	// Generate the exponential swept sin signal
	float w1 = 2 * M_PI*f1;
	float w2 = 2 * M_PI*f2;
	size_t length = duration * samplerate;
	float* timeIndex = (float*)malloc(sizeof(float) * length);

	float K = duration * w1 / logf(w2 / w1);
	float L = duration / logf(w2 / w1);
	float* expSweptSin = (float*)malloc(sizeof(float) * length);
	float* invExpSweptSin = (float*)malloc(sizeof(float) * length);

	for (size_t n = 0; n < length; n++)
	{
		timeIndex[n] = n * 1.0f / samplerate;//t = 0:1 / fs : (duration*fs - 1) / fs;
	}

	for (size_t n = 0; n < length; n++)
	{
		expSweptSin[n] = sinf(K * (expf(timeIndex[n] / L) - 1));
	}

	for (size_t n = 0; n < length; n++)
	{
		//invsweep = sweep(length(sweep) :-1 : 1).*exp(-t. / L);
		invExpSweptSin[n] = expSweptSin[length - n - 1] * expf(-timeIndex[n] / L);
	}
	free(timeIndex);
	free(invExpSweptSin);
	return expSweptSin;
	//invsweep = invsweep / max(abs(invsweep));
}
float* generateInvExpSweptSin(float duration, float f1, float f2, size_t samplerate) {
	// Generate the invert exponential swept sin signal

}
float* findSystemIR(float* RecordedExpSweptSin, float duration, float f1, float f2, size_t samplerate) {
	// Fine the impulse response of the non-linear system.
}