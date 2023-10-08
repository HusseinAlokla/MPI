#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define WIDTH 1000
#define HEIGHT 1000
#define MAX_ITER 1000

// Function to compute the Mandelbrot set for a given pixel
int mandelbrot(double real, double imag) {
    int n;
    double r = 0.0;
    double i = 0.0;

    for (n = 0; n < MAX_ITER; n++) {
        double r2 = r * r;
        double i2 = i * i;
        if (r2 + i2 > 4.0) {
            return n;
        }
        i = 2.0 * r * i + imag;
        r = r2 - i2 + real;
    }

    return MAX_ITER;
}

int main(int argc, char **argv) {
    MPI_Init(&argc, &argv);
    double start_time, end_time;
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    start_time = MPI_Wtime();
    // Calculate the number of rows to process per MPI process
    int rows_per_process = HEIGHT / size;

    // Allocate memory for the local portion of the image
    unsigned char *local_image = (unsigned char *)malloc(rows_per_process * WIDTH * sizeof(unsigned char));

    // Compute the Mandelbrot set for the local portion
    for (int y = rank * rows_per_process; y < (rank + 1) * rows_per_process; y++) {
        for (int x = 0; x < WIDTH; x++) {
            double real = (x - WIDTH / 2.0) * 4.0 / WIDTH;
            double imag = (y - HEIGHT / 2.0) * 4.0 / HEIGHT;
            int value = mandelbrot(real, imag);
            local_image[(y - rank * rows_per_process) * WIDTH + x] = (unsigned char)(value % 256);
        }
    }

    // Gather local images to the root process
    unsigned char *global_image = NULL;
    if (rank == 0) {
        global_image = (unsigned char *)malloc(HEIGHT * WIDTH * sizeof(unsigned char));
    }
    MPI_Gather(local_image, rows_per_process * WIDTH, MPI_UNSIGNED_CHAR, global_image, rows_per_process * WIDTH, MPI_UNSIGNED_CHAR, 0, MPI_COMM_WORLD);
    end_time = MPI_Wtime();
    double total_execution_time = end_time - start_time;
    // Save the image on the root process
    if (rank == 0) {
        printf("Total execution time: %lf seconds\n", total_execution_time);

        FILE *fp = fopen("mandelbrot.ppm", "wb");
        if (fp == NULL) {
            fprintf(stderr, "Error: Unable to open the file for writing.\n");
            MPI_Finalize();
            return 1;
        }

        fprintf(fp, "P6\n%d %d\n255\n", WIDTH, HEIGHT);

        for (int i = 0; i < HEIGHT; i++) {
            for (int j = 0; j < WIDTH; j++) {
                unsigned char pixel_value = global_image[i * WIDTH + j];
                fputc(pixel_value, fp); // Red
                fputc(pixel_value, fp); // Green
                fputc(pixel_value, fp); // Blue
            }
        }

        fclose(fp);

        // Clean up memory
        free(global_image);
    }

    free(local_image);
    MPI_Finalize();

    return 0;
}
