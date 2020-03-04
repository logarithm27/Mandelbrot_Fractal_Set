cmake_minimum_required(VERSION 3.15)
project(Mandelbrot_Fractal_Set_ C)

set(CMAKE_C_STANDARD 11)

add_library(Mandelbrot_Fractal_Set_ library.c library.h mandelbrot_sequential.c mandelbrot_parallel.c mystack.h)
add_executable(mandelbrot_parallel mandelbrot_parallel.c)
add_executable(mandelbrot_sequential mandelbrot_sequential.c)
add_executable(mandelbrot_open_mp.c)