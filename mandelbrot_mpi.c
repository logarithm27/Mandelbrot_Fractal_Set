//
// Created by maftoul on 4/25/2020.
// CLion Jetbrains
//
  #include "mpi.h"
  #include <stdio.h>
  #define SIZE 2
  #define STARTEND 2
  #define START 0
  #define END 1
  #include <string.h>
  #include <errno.h>
  #include <time.h>
  #include <stdlib.h>
  #include <errno.h>
  #define Y_MIN -2
  #define Y_MAX 2
  #define X_MIN -2
  #define X_MAX 2
  #define RESOLUTION 0.001
  #define OUTFILE "mandelbrot_paral_mpi.out"
  #define N_ITER_MAX 400
  int * iter_tab;
  int nb_pixel_y;
  int nb_pixel_x;

   int main(int argc, char *argv[])  {
   nb_pixel_x = (int) ((X_MAX - X_MIN) / RESOLUTION);
   nb_pixel_y = (int) ((Y_MAX - Y_MIN) / RESOLUTION);
	if( (iter_tab= malloc(sizeof(int)*nb_pixel_x*nb_pixel_y)) == NULL)
    	{
        	printf("Memory allocation error, error Nb : %d (%s) .\n",errno,strerror(errno));
        	return EXIT_FAILURE;
    	}
   int numtasks, rank, sendcount, recvcount, source;  
 
   MPI_Init(&argc,&argv);
   MPI_Comm_rank(MPI_COMM_WORLD, &rank);
   MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
   int sendbuf[numtasks][STARTEND];
   int recvbuf[numtasks];	
   for(int i= 0;i <numtasks;i++)
   {
	for(int j=0;j<STARTEND;j++)
		{	
			int a = i+1;	
			if(j==0)
				sendbuf[i][j] = (int)((float)(nb_pixel_x) * ((float)(a-1)/(float)numtasks));
			else if(j==1)
				sendbuf[i][j] = (int)((float)(nb_pixel_x) * ((float)(a)/(float)numtasks));	
		}
   }

   if (numtasks >= 2) {
     sendcount = 2;
     recvcount = 2;
     MPI_Scatter(sendbuf,sendcount,MPI_INT,recvbuf,recvcount, MPI_INT,1,MPI_COMM_WORLD);
	 printf("Rank = %d  computing on the Interval = [%d,%d] \n",rank,recvbuf[0],recvbuf[1]);
	 for(int x_pixel = recvbuf[0]; x_pixel < recvbuf[1]; x_pixel++)
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
     }
   else
     printf("Must specify at least %d processors.\n",SIZE);

   MPI_Finalize();
   return 0;
   }
