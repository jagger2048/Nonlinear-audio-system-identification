
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
	float* mag_db =  findSystemIR(yFiltered, duration, f1,f2, fs);
	


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

	// test passed
}

void dbbUintTest() {
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
	float* ir_mag = findSystemIR(yFiltered, duration, f1, f2, fs);
	
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
	freeDBB(dbb);

}
int main()
{
	dbbUintTest();

	return 0;
}
/*
1. DBB 算法的 c 语言实现
	   DBB 算法的 state1 滤波器在我们所使用的版本中效果并不太理想，可以尝试将
	cut-off freq 从 40 Hz 提升至 60 Hz，并增加 Q 值，更一般的情况是进行频响曲线拟合，
	通过输入曲线和输出曲线，倒推出处理系统的曲线，然后进行拟合，将原有调参的过程交给
	程序来处理。这样我们只需要给出目标曲线即可
		之前混响模型中的调参模式也可以这么来，处理完这部分之后最好重新审视一下
	那篇“使用遗传算法来进行混响模型参数提取”的论文。
			2019-3-13 10:06:44 by jagger
	- 2019年3月18日17:24:02 补充
		在完成使用正弦扫频信号测量频响后，对所设计的系统进行了频响测试，发现在较低的输入幅度
		下，输出的信号无法达到预期的增益值

2. 频响测量法的移植
	通过移植 matlab 版的频响测量法，加快调试的进程，并未后续的模型曲线拟合提供必要的工具
	-1. 需要引进 FFT 算法，目前待选的是 kissfft
	-2. 模拟 fftfilt 函数，实现卷积的功能
	-3. 建立一种较为通用的数据结构，用于在 C 和 matlab 中进行数据交互，有现成的最好
		暂时使用 txt,后续可以考虑 csv json
*/
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