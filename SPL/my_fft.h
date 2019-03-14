#pragma once
#include "kiss_fftr.h"
#include <stdint.h>
// the wrapage of kiss fft
// v 0.1	2019-3-13 16:41:34

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

	typedef  kiss_fft_cpx kfComplex;
	//typedef kiss_fftr_cfg rfftConfig;

	typedef struct {
		kiss_fftr_cfg fftPlan;
		bool isInverse;
		size_t length;
	}
	rfftConfig;

	typedef struct
	{
		kiss_fft_cfg forwardFFT;
		kiss_fft_cfg inverseFFT;
	}
	cfftConfig;

	void setFft(rfftConfig* obj,size_t length, bool isInverseFft) {
		if (obj->fftPlan)
		{
			free(obj->fftPlan);
		}
		obj->length = length;
		obj->isInverse = isInverseFft;
		obj->fftPlan = kiss_fftr_alloc(obj->length, obj->isInverse, NULL, NULL);

	}
	rfftConfig* createFft(size_t frameSize,bool isInverseFft) {
		rfftConfig* obj = (rfftConfig*)malloc(sizeof(rfftConfig));
		obj->fftPlan = NULL;
		obj->isInverse = isInverseFft;
		obj->length = frameSize;

		setFft(obj,obj->length,isInverseFft);
		
		return obj;
	}

	static inline void runFft(rfftConfig* obj, float* inFrame, kfComplex* outFrame) {
		if (!obj->isInverse)
		{
			kiss_fftr(obj->fftPlan, inFrame, outFrame);
		}
	}
	static inline void runIfft(rfftConfig* obj, kfComplex* inFrame, float* outFrame) {
		if (obj->isInverse)
		{
			kiss_fftri(obj->fftPlan, inFrame, outFrame);
		}
	}

	void freeFft(rfftConfig* obj) {
		if (obj)
		{
			free(obj->fftPlan);
			free(obj);
		}
	}

	//////
	cfftConfig* createFftc(size_t frameSize) {
		cfftConfig* obj = (cfftConfig*)malloc(sizeof(cfftConfig));
		obj->forwardFFT = kiss_fft_alloc(frameSize, 0, NULL, NULL);
		obj->inverseFFT = kiss_fft_alloc(frameSize, 1, NULL, NULL);		// 1: inverse 
		return obj;
	}
	void runFftc(cfftConfig* obj, kfComplex* inFrame, kfComplex* outFrame) {
		kiss_fft(obj->forwardFFT, inFrame, outFrame);
	}
	void runIfftc(cfftConfig* obj, kfComplex* inFrame, kfComplex* outFrame) {
		kiss_fft(obj->inverseFFT, inFrame, outFrame);
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