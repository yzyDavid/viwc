#include <stdio.h>

int main()
{
    FILE *fp;
    int i, j;
    fp = fopen("size.txt", "w");
    for (i = 1; i <= 200; i++)
    {
        fprintf(fp, "%d", i % 10);
    }
    fprintf(fp, "\n");
    for (i = 1; i <= 200; i++)
    {
        fprintf(fp, "%d", (i / 10) % 10);
    }
    fprintf(fp, "\n");
    for (i = 3; i <= 200; i++)
    {
        fprintf(fp, "%d\n", i);
    }
    fclose(fp);
}
