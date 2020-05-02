//
// Created by maftoul on 1/27/2020.
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

#define pointer_on_my_struct typeof(pointer_on_plot_data)
#define generic_pointer void *

#define Y_MIN -2
#define Y_MAX 2
#define X_MIN -2
#define X_MAX 2
#define RESOLUTION 0.001
#define NUMBER_OF_THREADS 4
#define OUTFILE "mandelbrot_parallel.out"
#define N_ITER_MAX 400


struct data {
    int n_iter_min;
    int n_iter_max;
}*pointer_on_plot_data;

typedef struct data my_data;

// Global Variables
int * iter_tab;
int nb_pixel_y;





/* the structure that holds all the essential components
of our thread
*/

/*
instead of declaring a little complex pointer on our struct
 when we aim to define a variable of type " (1) struct data *pointer_on_plot_data ",
 we add after the closing curly brace of our struct a pointer,
 it's used to declare (1) on our thread function
 */

generic_pointer mandelbrot_thread(generic_pointer input)
{
    /*
     * pointer_on_plot_data is equivalent to " struct data *pointer_on_plot_data
     * the typeof(pointer_on_plot_data) will have a pointer on a data struct
     * */
    pointer_on_plot_data = (pointer_on_my_struct) input;

    int n_iter_min = pointer_on_plot_data->n_iter_min;
    int n_iter_max = pointer_on_plot_data->n_iter_max;

    for(int x_pixel = n_iter_min ; x_pixel < n_iter_max; x_pixel++)
        for(int y_pixel = 0; y_pixel < nb_pixel_y; y_pixel++)
        {
            double x_init = X_MIN + x_pixel * RESOLUTION;
            double y_init = Y_MIN + y_pixel * RESOLUTION;
            double x = x_init;
            double y = y_init;
            int iter = 0;
            for(iter = 0; iter < N_ITER_MAX; iter++)
            {
                double previous_y = y;
                double previous_x = x;

                if( (x*x + y*y) > 4 )
                    break;
                x = previous_x * previous_x - previous_y*previous_y + x_init;
                y = 2 * previous_x * previous_y + y_init;
            }
            iter_tab[ x_pixel * nb_pixel_y + y_pixel ] = iter;
        }
    pthread_exit(NULL);
    return EXIT_SUCCESS;

}

int main(int argc, char *argv[])
{
    int nb_pixel_x = (int) ceil((X_MAX - X_MIN) / RESOLUTION);
    nb_pixel_y = (int) ceil((Y_MAX - Y_MIN) / RESOLUTION);

    if( (iter_tab=malloc(sizeof(int)*nb_pixel_x*nb_pixel_y)) == NULL)
    {
        printf("Memory allocation error, error Nb : %d (%s) .\n",errno,strerror(errno));
        return EXIT_FAILURE;
    }
    struct stack_of_iteration_intervals * stack_of_intervals_iter = createStack(NUMBER_OF_THREADS*NUMBER_OF_THREADS);
    int j = 0;
    int previous_min_is_max_now = 0;
    int n_iter_max = N_ITER_MAX;
    int step = N_ITER_MAX/NUMBER_OF_THREADS;
    while(j < NUMBER_OF_THREADS)
    {
        if( j == 0 ) {
            push(stack_of_intervals_iter, n_iter_max);
            previous_min_is_max_now = n_iter_max - step;
            push(stack_of_intervals_iter,previous_min_is_max_now);
        }
        if(j > 0) {
            int max = previous_min_is_max_now;
            push(stack_of_intervals_iter, max);
            int min = max - step;
            push(stack_of_intervals_iter, min);
            previous_min_is_max_now = min;
        }

        j++;
    }

    pthread_t * thread_array_id = malloc(NUMBER_OF_THREADS * sizeof(pthread_t));
    /*allocating memory of the pointer on the struct*/
    /*reminder : pointer_on_my_struct is equivalent to typeof(pointer_on_plot_data) and also equivalent to (struct data *pointer_on_plot_data */
    // we can do also

    pointer_on_my_struct my_data_struct = malloc(NUMBER_OF_THREADS * sizeof(my_data));
    clock_t begin = clock();

    for(int i = 0; i < NUMBER_OF_THREADS; i++)
    {
        my_data_struct[i].n_iter_min = pop(stack_of_intervals_iter);
        my_data_struct[i].n_iter_max = pop(stack_of_intervals_iter);

        pthread_t pth;
        pthread_create(&pth, NULL, mandelbrot_thread, (generic_pointer) &my_data_struct[i]);
        printf("Thread  %d created and working on the interval [%d,%d]\n", (int)pth, my_data_struct[i].n_iter_min, my_data_struct[i].n_iter_max);
        thread_array_id[i] = pth;
    }

    for(int i = 0; i < NUMBER_OF_THREADS; i++)
    {
        pthread_join(thread_array_id[i], NULL);
        printf("Thread %d terminated\n",(int)thread_array_id[i]);
    }

    clock_t end = clock();
    double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
    printf("time execution %f seconds while calculating points : ", time_spent);
    FILE * output_file;
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
}
