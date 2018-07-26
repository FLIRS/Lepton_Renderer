#include <stdio.h>
#include <sys/stat.h>
//getopt()
#include <unistd.h>

#include "debug.h"
#include "common.h"
#include "pixel.h"



int main(int argc, char *argv[])
{
	while (1)
	{
		int C = getopt (argc, argv, "");
		if (C == - 1) {break;}
		switch (C)
		{
			default:
			break;
		}
	}
	
	
	struct __attribute__((__packed__)) Pixel
	{
		 uint8_t a;
		 uint8_t b;
		 uint8_t g;
		 uint8_t r;
	};
	
	struct Pixel p [256];
	size_t n = 4;
	size_t depth = sizeof (uint8_t);
	size_t l = sizeof (p) / depth;
	int r = pix_load ((uint8_t *)p, &l, "src/common/heatmap.txt", depth);
	TRACE_F ("%i", r);
	TRACE_F ("%i", l);
	
	for (size_t i = 0; i < (l / n); i = i + 1)
	{
		printf ("%3i: %3i %3i %3i %3i\n", i, p [i].r, p [i].g, p [i].b, p [i].a);
	}

	return 0;
}

