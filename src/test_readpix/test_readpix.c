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
	
	struct Pixel_ABGR8888 p [256];
	
	int r = pix_load (p, COUNTOF (p), "src/common/heatmap.txt");
	for (int i = 0; i < r; i = i + 1)
	{
		printf ("%i: %i %i %i %i\n", i, p [i].r, p [i].g, p [i].b, p [i].a);
	}

	return 0;
}

