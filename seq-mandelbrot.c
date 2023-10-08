#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define WIDTH 1000
#define HEIGHT 1000
#define MAX_ITER 1000

int mandelbrot(double real, double imag)
{
    int n;
    double r = 0.0;
    double i = 0.0;

    for (n = 0; n < MAX_ITER; n++)
    {
        double r2 = r * r;
        double i2 = i * i;
        if (r2 + i2 > 4.0)
        {
            return n;
        }
        i = 2.0 * r * i + imag;
        r = r2 - i2 + real;
    }

    return MAX_ITER;
}

int main()
{

    FILE *fp = fopen("mandelbrot1.ppm", "wb");
    if (fp == NULL)
    {
        fprintf(stderr, "Error: Unable to open the file for writing.\n");
        return 1;
    }
    clock_t start_time, end_time;
    double cpu_time_used;
    start_time = clock();
    fprintf(fp, "P6\n%d %d\n255\n", WIDTH, HEIGHT);

    for (int y = 0; y < HEIGHT; y++)
    {
        for (int x = 0; x < WIDTH; x++)
        {
            double real = (x - WIDTH / 2.0) * 4.0 / WIDTH;
            double imag = (y - HEIGHT / 2.0) * 4.0 / HEIGHT;
            int value = mandelbrot(real, imag);
            unsigned char pixel_value = (unsigned char)(value % 256);
            fputc(pixel_value, fp); // Red
            fputc(pixel_value, fp); // Green
            fputc(pixel_value, fp); // Blue
        }
    }

    fclose(fp);
    printf("Mandelbrot set image generated as 'mandelbrot1.ppm'\n");
    end_time = clock(); // Stop measuring time
    cpu_time_used = ((double)(end_time - start_time)) / CLOCKS_PER_SEC;

    printf("Execution Time: %lf seconds\n", cpu_time_used);

    return 0;
}
