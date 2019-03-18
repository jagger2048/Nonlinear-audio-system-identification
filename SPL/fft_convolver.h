#pragma once
#include "my_fft.h"

// This is a fft based convolver.Its implementation can be showed as below:
// conv(seq_a,seq_b) = ifft( fft(seq_a) * fft(seq_b)); seq_a and seq_b should
// be zero padding to len(seq_a)+len(seq_b)-1

float* fftConvolver(float* seqA, size_t lenA, float* seqB, size_t lenB) {

	size_t len = (lenA + lenB ) / 2 * 2;
	float* output = (float*)malloc(sizeof(float)*len);
	float* bufferA = (float*)malloc(sizeof(float)*len);
	float* bufferB = (float*)malloc(sizeof(float)*len);

	// zeros padding
	memset(bufferA, 0, sizeof(float)*len);
	memset(bufferB, 0, sizeof(float)*len);
	memcpy(bufferA, seqA, lenA * sizeof(float));
	memcpy(bufferB, seqB, lenB * sizeof(float));

	rfftConfig* fftPlan = createFft(len,false);	

	
	kfComplex* fftTmpA = (kfComplex*)malloc(sizeof(kfComplex)*len);
	kfComplex* fftTmpB = (kfComplex*)malloc(sizeof(kfComplex)*len);
	kfComplex* fftTmpC = (kfComplex*)malloc(sizeof(kfComplex)*len);

	memset(fftTmpA, 0, sizeof(kfComplex)*len);
	memset(fftTmpB, 0, sizeof(kfComplex)*len);
	memset(fftTmpC, 0, sizeof(kfComplex)*len);

	runFft(fftPlan, bufferA, fftTmpA);
	runFft(fftPlan, bufferB, fftTmpB);

	// complex multiply
	for (size_t n = 0; n < len; n++)
	{
		fftTmpC[n].r = fftTmpA[n].r * fftTmpB[n].r - fftTmpA[n].i * fftTmpB[n].i;
		fftTmpC[n].i = fftTmpA[n].r * fftTmpB[n].i + fftTmpA[n].i * fftTmpB[n].r;
	}
	setFft(fftPlan, len,true);			
	runIfft(fftPlan, fftTmpC,output );	// ifft

	for (size_t n = 0; n < len; n++)	
	{
		output[n] = output[n] / (float)(len);
	}

	free(bufferA);
	free(bufferB);
	free(fftTmpA);
	free(fftTmpB);
	free(fftTmpC);
	freeFft(fftPlan);

	return output;
}