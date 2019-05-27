#include <stdio.h>
#include <sys/stat.h>

//getopt()
#include <unistd.h>

#include "debug.h"
#include "common.h"
#include "map.h"



void test__map_lin_int (int X, int A0, int A1, int B0, int B1)
{
	int Y = map_lin_int (X, A0, A1, B0, B1);
	printf ("%6i %6i %6i %6i , %6i => %-6i\n", A0, A1, B0, B1, X, Y);
}


int main(int argc, char *argv[])
{
	while (1)
	{
		int c = getopt (argc, argv, "");
		if (c == -1) {break;}
		switch (c)
		{
			default:
			break;
		}
	}
	
	printf ("%6s %6s %6s %6s : %6s => %-6s\n", "A0", "A1", "B0", "B1", "X", "Y");
	test__map_lin_int (0, 0, 0, 0, 0);
	test__map_lin_int (1, 0, 0, 0, 0);
	test__map_lin_int (1, 1, 1, 0, 1);
	test__map_lin_int (100, 50, 200, 0, 10);
	test__map_lin_int (5, 0, 10, 0, 100);
	test__map_lin_int (19199, 0, 19200, 0, 65534);
	
	
	return 0;
}

