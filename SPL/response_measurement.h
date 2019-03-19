#pragma once

// This file contains the algorithm to measure the frequency response of a 
// non-linear system, which based on exponential sine sweep , ESS method.
#include <math.h>
#include <stdlib.h>
#ifndef M_PI
#define M_PI 3.141592654
#endif // !M_PI

#include "fft_convolver.h"

float* generateExpSineSweep(float duration, float f1, float f2, size_t samplerate) {
	// Generate the exponential sine sweep signal
	float w1 = 2 * M_PI*f1;
	float w2 = 2 * M_PI*f2;
	size_t length = duration * samplerate;
	float* timeIndex = (float*)malloc(sizeof(float) * length);

	float K = duration * w1 / logf(w2 / w1);
	float L = duration / logf(w2 / w1);
	float* expSineSweep = (float*)malloc(sizeof(float) * length);

	for (size_t n = 0; n < length; n++)
	{
		timeIndex[n] = n * 1.0f / samplerate;//t = 0:1 / fs : (duration*fs - 1) / fs;
	}

	for (size_t n = 0; n < length; n++)
	{
		expSineSweep[n] = sinf(K * (expf(timeIndex[n] / L) - 1));
	}

	free(timeIndex);
	return expSineSweep;
}
float* generateInvExpSineSweep(float duration, float f1, float f2, size_t samplerate) {
	// Generate the invert exponential swept sin signal
	float w1 = 2 * M_PI*f1;
	float w2 = 2 * M_PI*f2;
	size_t length = duration * samplerate;
	float* timeIndex = (float*)malloc(sizeof(float) * length);

	float K = duration * w1 / logf(w2 / w1);
	float L = duration / logf(w2 / w1);
	float* expSineSweep = (float*)malloc(sizeof(float) * length);
	float* invExpSineSweep = (float*)malloc(sizeof(float) * length);

	for (size_t n = 0; n < length; n++)
	{
		timeIndex[n] = n * 1.0f / samplerate;//t = 0:1 / fs : (duration*fs - 1) / fs;
	}

	for (size_t n = 0; n < length; n++)
	{
		expSineSweep[n] = sinf(K * (expf(timeIndex[n] / L) - 1));
	}

	for (size_t n = 0; n < length; n++)
	{
		//invsweep = sweep(length(sweep) :-1 : 1).*exp(-t. / L);
		invExpSineSweep[n] = expSineSweep[length - n - 1] * expf(-timeIndex[n] / L);
	}
	free(timeIndex);
	free(expSineSweep);
	return invExpSineSweep;
	//invsweep = invsweep / max(abs(invsweep));
}

float* findSystemIR(float* recordedExpSineSweep, float duration, float f1, float f2, size_t samplerate, float* impulseResponse) {
	// Fine the impulse response of the non-linear system.
	// generate inverse sinc sweep signal
	float* invEss = generateInvExpSineSweep(duration, f1, f2, samplerate);
	impulseResponse= fftConvolver(recordedExpSineSweep, duration*samplerate, invEss, duration*samplerate);

	size_t len = (duration*samplerate*2) / 2 * 2;// lenA lenB

	// Normalize
	double max = 0;
	for (size_t n = 0; n < len; n++)
	{
		if (max< impulseResponse[n])
		{
			max = impulseResponse[n];
		}
	}
	for (size_t n = 0; n < len; n++)
	{
		impulseResponse[n] /= max;
	}
	//printf("max is:%f", max);
	// calculate the system's response
	rfftConfig* fftPlan = createFft(len, false);
	kfComplex *yIr = (kfComplex*)malloc(sizeof(kfComplex)*len);
	memset(yIr, 0, sizeof(kfComplex)*len);
	runFft(fftPlan, impulseResponse, yIr);

	float* mag = (float*)malloc(sizeof(float)*len );
	for (size_t n = 0; n < len; n++)
	{
		mag[n] = 20 * log10f(sqrt(yIr[n].r*yIr[n].r + yIr[n].i*yIr[n].i));	// without normalized
	}

	free(invEss);
	free(impulseResponse);
	freeFft(fftPlan);
	free(yIr);
	return mag;

}

