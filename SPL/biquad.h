#pragma once
#include <stdio.h>

// the biquad filter
typedef struct {
	float B[3];
	float A[3];
	size_t samplerate;

	//float w0, w1, w2;
	float x1, x2, y1, y2;
	//bool isInitialized;
}
Biquad;

static inline void setBiquad(Biquad *obj,size_t samplerate, float coeffs_b[3], float coeffs_a[3]) {
	obj->samplerate = samplerate;
	for (size_t n = 0; n < 3; n++)
	{
		obj->B[n] = coeffs_b[n];
		obj->A[n] = coeffs_a[n];
	}
}
static inline void resetBiquad(Biquad *obj) {
	obj->x1 = 0;
	obj->x2 = 0;
	obj->y1 = 0;
	obj->y2 = 0;
}
Biquad* newBiquad(size_t samplerate) {
	Biquad* obj = (Biquad*)malloc(sizeof(Biquad));
	if (!obj)
	{
		return NULL;// throw erroe
	}
	else
	{
		obj->samplerate = samplerate;
		resetBiquad(obj);
		return obj;
	}
}
Biquad* createBiquad(size_t samplerate, float coeffs_b[3], float coeffs_a[3]) {
	Biquad* obj = newBiquad(samplerate);
	setBiquad(obj, samplerate, coeffs_b, coeffs_a);
	return obj;
}
static inline float runBiquad(Biquad* obj, float sample_in, float &sample_out) {
	// reference from:https://en.wikipedia.org/wiki/Digital_biquad_filter
	// Direct Form 1
	sample_out = obj->B[0] * sample_in + obj->B[1] * obj->x1 + obj->B[2] * obj->x2
		- obj->A[1] * obj->y1 - obj->A[2] * obj->y2;

	obj->x2 = obj->x1;		// x2: x(n-2)
	obj->x1 = sample_in;	// x1: x(n-1)

	obj->y2 = obj->y1;		// y2: y(n-2)
	obj->y1 = sample_out;	// y1: y(n-1)

	return sample_out;
	/**
	// Direct Form 2
	obj->w0 = sample_in - obj->A[1] * obj->w1 - obj->A[2] * obj->w2;
	obj->w1 = obj->w0;
	obj->w2 = obj->w1;
	float result = obj->B[0] * obj->w0 + obj->B[1] * obj->w1 + obj->B[2] * obj->w2;

	*/
}
int freeBiquad(Biquad* obj) {
	if (!obj)
	{
		return -1;
	}
	free(obj);
	return 0;
}