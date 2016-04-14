/*
 * to test and record the ascii of control keys and combined keys.
 */
#include <conio.h>

int main()
{
    int a;
    while (1)
    {
        printf("%2X\n", ((a = getch()) == 'q') ? (exit(0), 0) : a);
    }

    return 0;
}

