#include <stdint.h>
#include "debug.h"

void convert_u16c1_f32c1 (uint16_t src [], float des [], int n)
{
	while (n--)
	{
		des [n] = (float)src [n] / UINT16_MAX;
	}
}


void convert_u16c1_u8c3 (uint16_t src [], uint8_t des [], int n)
{
	while (n--)
	{
		des [n*3+0] = src [n] * UINT8_MAX / UINT16_MAX;
		des [n*3+1] = src [n] * UINT8_MAX / UINT16_MAX;
		des [n*3+2] = src [n] * UINT8_MAX / UINT16_MAX;
	}
}


void convert_f32c1_u8c3 (float src [], uint8_t des [], int n)
{
	while (n--)
	{
		des [n*3+0] = src [n] * UINT8_MAX;
		des [n*3+1] = src [n] * UINT8_MAX;
		des [n*3+2] = src [n] * UINT8_MAX;
	}
}
