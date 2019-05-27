#include <stdio.h>
#include <sys/stat.h>

//getopt()
#include <unistd.h>

#include "debug.h"
#include "common.h"
#include "map.h"



void test__map_lin_u16 (uint16_t X, uint16_t A0, uint16_t A1, uint16_t B0, uint16_t B1)
{
	uint16_t Y = map_lin_u16 (X, A0, A1, B0, B1);
	printf ("%3i %3i %3i %3i %3i -> %3i\n", X, A0, A1, B0, B1, Y);
}


int main(int argc, char *argv[])
{
	while (1)
	{
		int C = getopt (argc, argv, "");
		if (C == -1) {break;}
		switch (C)
		{
			default:
			break;
		}
	}
	
	printf ("  X  A0  A1  B0  B1 ->   Y\n");
	test__map_lin_u16 (0, 0, 0, 0, 0);
	test__map_lin_u16 (1, 0, 0, 0, 0);
	test__map_lin_u16 (1, 1, 1, 0, 1);
	test__map_lin_u16 (100, 50, 200, 0, 10);
	test__map_lin_u16 (5, 0, 10, 0, 100);
	
	test__map_lin_u16 (19199, 0, 19200, 0, 65534);
	
	
	return 0;
}

