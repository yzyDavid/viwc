/*
 * testing inline asm in this file.
 */
#include <stdio.h>
#include <graphics.h>

int main()
{
	short int reg;
	printf("t.c\n");
	__asm
	{
		mov reg, ax
	}
}
