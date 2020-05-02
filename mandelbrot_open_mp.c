//
// Created by MftOmar on 1/27/2020.
// CLion Jetbrains
//
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <pthread.h>
#include "mystack.h"
#include "omp.h"

#define pointer_on_my_struct typeof(pointer_on_plot_data)
#define generic_pointer void *

#define Y_MIN -2
#define Y_MAX 2
#define X_MIN -2
#define X_MAX 2
#define RESOLUTION 0.001
#define OUTFILE "mandelbrot_parallel_open_mp.out"
#define N_ITER_MAX 1000

// Global Variables
int * iter_tab;
int nb_pixel_y;
int nb_pixel_x;

/* the structure that holds all the essential components
of our thread
*/

/*
instead of declaring a little complex pointer on our struct
 when we aim to define a variable of type " (1) struct data *pointer_on_plot_data ",
 we add after the closing curly brace of our struct a pointer,
 it's used to declare (1) on our thread function
 */
int main(int argc, char *argv[])
{
    /*
     * pointer_on_plot_data is equivalent to " struct data *pointer_on_plot_data
     * the typeof(pointer_on_plot_data) will have a pointer on a data struct
     * */
    nb_pixel_x = (int) ceil((X_MAX - X_MIN) / RESOLUTION);
    nb_pixel_y = (int) ceil((Y_MAX - Y_MIN) / RESOLUTION);

    if( (iter_tab=malloc(sizeof(int)*nb_pixel_x*nb_pixel_y)) == NULL)
    {
        printf("Memory allocation error, error Nb : %d (%s) .\n",errno,strerror(errno));
        return EXIT_FAILURE;
    }

    FILE * output_file;
    int thread_count = strtol(argv[1], NULL, 10);
    clock_t begin = clock();

//    omp_set_num_threads(4);
    #pragma omp parallel for num_threads(thread_count)
    for (int x_pixel = 0; x_pixel < nb_pixel_x; x_pixel++)
        for (int y_pixel = 0; y_pixel < nb_pixel_y; y_pixel++) {
            double x_init = X_MIN + x_pixel * RESOLUTION;
            double y_init = Y_MIN + y_pixel * RESOLUTION;
            double x = x_init;
            double y = y_init;
            int iter = 0;
            for (iter = 0; iter < N_ITER_MAX; iter++) {
                double previous_y = y;
                double previous_x = x;
                if ((x * x + y * y) > 4)
                    break;
                x = previous_x * previous_x - previous_y * previous_y + x_init;
                y = 2 * previous_x * previous_y + y_init;
            }
            iter_tab[x_pixel * nb_pixel_y + y_pixel] = iter;
    }
    clock_t end = clock();
    double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
    printf("time execution %f seconds while calculating points : ", time_spent);
    if( (output_file = fopen(OUTFILE,"w")) == NULL )
    {
        printf("Error while opening output file : Error Nb %d (%s) .\n",errno,strerror(errno));
        return EXIT_FAILURE;
    }

    for(int x_pixel = 0; x_pixel < nb_pixel_x ; x_pixel++)
    {
        for(int y_pixel = 0 ; y_pixel < nb_pixel_y; y_pixel++)
        {
            double x = X_MIN + x_pixel * RESOLUTION;
            double y = Y_MIN + y_pixel * RESOLUTION;
            fprintf(output_file,"%f %f %d\n", x, y,iter_tab[x_pixel*nb_pixel_y+y_pixel]);
        }
        fprintf(output_file,"\n");
    }

    /*clean*/

    free(iter_tab);
    fclose(output_file);
    return EXIT_SUCCESS;

}

