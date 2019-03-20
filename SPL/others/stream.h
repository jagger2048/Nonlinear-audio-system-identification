#pragma once
#include "frame.h"
#include <stdint.h>
//#include "wavfile.h"
enum audio_stream_type
{
	PCM,	// pcm		// http://soundfile.sapp.org/doc/WaveFormat/
	DSD,	// dsd
	DOP		// dsd over pcm
};
// pcm stream package
typedef struct {
	// audio info
	// translate functions (optional)
	audio_stream_type stream_type;
	//WAV wavfile;

	unsigned int channel;
	unsigned int sample_rate;
	uint64_t total_samples;
	uint64_t samples_per_channel;
	unsigned int bps;
	// raw stream
	FILE *fp;
	long int fpos;				// current pos
}spl_stream_st;

spl_stream_st * audio_stream_open(const char* file_name) {
	FILE *fp;
	//fp = fopen(file_name, "rb");
	fopen_s(&fp, file_name, "rb");
	if (!fp)
	{
		perror("File opening failed");
		return NULL;
	}
	spl_stream_st * pStream = (spl_stream_st *)malloc(sizeof(spl_stream_st));
	uint32_t data_size;
	pStream->fp = fp;
	uint16_t byte2;
	uint32_t byte4;
	// check stream type

	fseek(fp, 8, SEEK_SET);	
	fread(&byte4, 4, 1, fp);
	if (byte4 == 0x45564157) {	// 'WAV'	
		pStream->stream_type = PCM;
	}
	else
	{
		// we curently only support wav format file.
		fclose(fp);
		return NULL;
	}
	// load audio file message into stream
	// only support 16 bits per sample
	fseek(fp, 22, SEEK_SET);
	fread(&byte2, 2, 1, pStream->fp);
	pStream->channel = byte2;

	fread(&byte4, 4, 1, pStream->fp);
	pStream->sample_rate = byte4;
	fseek(pStream->fp, 6, SEEK_CUR);	// skip ByteRate(4 bytes) and BlockAlign(2 bytes)
	fread(&byte2, 2, 1, pStream->fp);
	pStream->bps = byte2;				// BitesPerSample
	//fseek(pStream->fp, 36, SEEK_SET);
	fseek(pStream->fp, 38, SEEK_SET);

	fread( &byte4, 4, 1, pStream->fp );

	if (byte4== 0x61746164)				// "data" 
	{
		fread(&byte4, 4, 1, pStream->fp);	// read data chunk size
		data_size = byte4;
	}
	pStream->samples_per_channel = data_size / (pStream->channel * pStream->bps / 8);
	pStream->total_samples = pStream->channel * pStream->samples_per_channel;
	pStream->fpos = ftell(pStream->fp);

	return pStream;// return pcm frame stream
} 
//spl_frame_p audio_stream_translate(spl_stream_st * fp,size_t frame_size);
int audio_stream_close(spl_stream_st * pStream) {
	if (!pStream)
	{
		return -1;
	}
	fclose(pStream->fp);
	return 0;
} // close audio stream
//
//int open_pcm_stream_and_read_f32(spl_frame_p pFrame,size_t frame_size);
//int open_pcm_stream_and_read_s16();
//read_s16_from_pcm_stream

int pcm_stream_frame_get(spl_stream_st* pStream, spl_frame_st *pFrame, size_t frame_size) {
	if (!pStream || feof(pStream->fp) || pStream->fpos <0)
	{
		return -1;
	}
	size_t bytes_to_read = sizeof(uint8_t) * frame_size *pStream->channel *  pStream->bps / 8;
	uint8_t *pPcmData = (uint8_t*)malloc(bytes_to_read);
	long int cur = ftell(pStream->fp);
	size_t read_count = 0;
	read_count = fread( pPcmData, bytes_to_read, 1, pStream->fp );
	cur = cur - ftell(pStream->fp);
	size_t byte_per_chunk = pStream->channel *  pStream->bps / 8;

	// translate and load pcm data into frame
	for (size_t n = 0; n < frame_size; n++)
	{
		for (size_t nChannels = 0; nChannels < pStream->channel; nChannels++)
		{
			// only support 16 bits per sample( 2 bytes ) 4 = nCh * bps/8
			//int16_t pcm_tmp = pPcmData[4*n + 2*nChannels] | pPcmData[4 * n + 2 * nChannels + 1] << 8;
			int16_t pcm_tmp = pPcmData[byte_per_chunk*n + 2 * nChannels] | pPcmData[byte_per_chunk * n + 2 * nChannels + 1] << 8;
			pFrame->sample_s16[nChannels][n] = pcm_tmp;
			pFrame->sample_f32[nChannels][n] = pcm_tmp / 32768.0f;
		}
	}
	return 0;
}