#pragma once
#include <stdlib.h>
#include "rms_limiter.h"
#include "biquad.h"
#include "compressor.h"

 float FreFilterCoeffs[2][3] = {
		0.997777849841792,	-1.99299886148802,		0.995223369031024,
		1,					-1.99298803444253,		0.99301204591831
};

 float State1FilterCoeffs[2][3] = {
		0.000868926863592,	2.527391006E-06,		-0.000866399472585,
		1,					-1.99789629951018,		0.997898465845326
};
	  	
 float State2FilterCoeffs[2][3] = {
		0.001586967109371,	8.417329742E-06,		-0.001578549779629,
		1,					-1.99615925409839,		0.996166468952456
};

// It's a C version Dynamic Bass Boost algorithm proted from Sigma Project.
typedef struct {
	Biquad* preFilter;
	Biquad* state1Filter;
	Biquad* state2Filter;
	RmsLimiter* state1Limiter;
	Compressor* state2SoftClip;

	size_t samplerate;
}
DBB;

DBB* createDBB(size_t samplerate) {
	DBB* obj = (DBB*)malloc(sizeof(DBB));
	if (!obj) {
		//free(obj);
		return NULL;
	}
	obj->preFilter = createBiquad(samplerate,FreFilterCoeffs[0], FreFilterCoeffs[1]);
	obj->state1Filter = createBiquad(samplerate,State1FilterCoeffs[0], State1FilterCoeffs[1]);
	obj->state2Filter = createBiquad(samplerate,State2FilterCoeffs[0], State2FilterCoeffs[1]);

	obj->state1Limiter = createRmsLimiter(-15, 5, 0.05, 0.2, 0, samplerate);//-24
	obj->state2SoftClip = createCompressor(-15, 6,5, 0.05, 0.2, 0, samplerate);//-24
	obj->samplerate = samplerate;
	return obj;
}
float runDBB(DBB* obj, float sample_in, float &sample_out) {
	float afterPreFilter = 0;
	float afterState1 = 0;
	float afterState2 = 0;
	float tmp;
	// Prefilter
	runBiquad(obj->preFilter, sample_in*2.0f, afterPreFilter);

	// State1
	runBiquad(obj->state1Filter, afterPreFilter, tmp);
	afterState1 = afterPreFilter + runRmsLimiter(obj->state1Limiter,tmp, tmp);

	// State2
	runBiquad(obj->state2Filter, afterState1, tmp);
	afterState2 = afterState1 + runCompressor(obj->state2SoftClip, tmp, tmp);
	
	// output
	sample_out = afterState2 * 0.5f;
	return afterState2 * 0.5f;
}
int freeDBB(DBB*obj) {
	if (!obj) {
		return -1;
	}
	freeBiquad(obj->preFilter);
	freeBiquad(obj->state1Filter);
	freeBiquad(obj->state2Filter);
	freeRmsLimiter(obj->state1Limiter);
	freeCompressor(obj->state2SoftClip);
	free(obj);
	return 0;
}

