
//#include <iostream>
#include <stdio.h>
#include <stdint.h>

#include "wavfile.h"
#include "frame.h"
int main()
{
	printf("This is the signal processing library test case.\n");
	wav wavfile;
	spl_frame_p pFram = spl_frame_new(48000, 1024);
	return 0;
}


