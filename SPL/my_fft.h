#pragma once
#include "kiss_fftr.h"
#include <stdint.h>
// the wrapage of kiss fft
// v 0.1	2019-3-13 16:41:34

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

	typedef  kiss_fft_cpx kfComplex;
	typedef kiss_fftr_cfg rfftConfig;

	typedef struct
	{
		kiss_fft_cfg forwardFFT;
		kiss_fft_cfg inverseFFT;
	}
	cfftConfig;


	rfftConfig createFft(size_t frameSize) {
		rfftConfig obj = kiss_fftr_alloc(frameSize, 0, NULL, NULL);		// 1: inverse 
		return obj;
	}

	static inline void runFft(rfftConfig obj, float* inFrame, kfComplex* outFrame) {
		kiss_fftr(obj, inFrame, outFrame);
	}
	static inline void runIfft(rfftConfig obj, kfComplex* inFrame, float* outFrame) {
		kiss_fftri(obj, inFrame, outFrame);
	}

	void freeFft(rfftConfig obj) {
		if (obj)
		{
			free(obj);
		}
	}


	cfftConfig* createFftc(size_t frameSize) {
		cfftConfig* obj = (cfftConfig*)malloc(sizeof(cfftConfig));
		obj->forwardFFT = kiss_fft_alloc(frameSize, 0, NULL, NULL);
		obj->inverseFFT = kiss_fft_alloc(frameSize, 1, NULL, NULL);		// 1: inverse 
		return obj;
	}
	void runFftc(cfftConfig obj, kfComplex* inFrame, kfComplex* outFrame) {
		kiss_fft(obj.forwardFFT, inFrame, outFrame);
	}
	void runIftc(cfftConfig obj, kfComplex* inFrame, kfComplex* outFrame) {
		kiss_fft(obj.inverseFFT, inFrame, outFrame);
	}
	void freeFftc(cfftConfig* obj) {
		if (obj)
		{
			free(obj->forwardFFT);
			free(obj->inverseFFT);
			free(obj);
		}
	}

#ifdef __cplusplus
}
#endif // __cplusplus