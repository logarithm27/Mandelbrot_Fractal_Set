//
// Created by maftoul on 09/02/2020.
//

#ifndef TRAINING_1_MYSTACK_H
#define TRAINING_1_MYSTACK_H
#include <stdio.h>
#include <stdlib.h>
#include <values.h>

struct stack_of_iteration_intervals {
    int top;
    unsigned capacity;
    int* array;
};

struct stack_of_iteration_intervals * createStack(unsigned capacity)
{
    struct stack_of_iteration_intervals* stack = (struct stack_of_iteration_intervals*)malloc(sizeof(struct stack_of_iteration_intervals));
    stack->capacity = capacity;
    stack->top = -1;
    stack->array = (int*)malloc(stack->capacity * sizeof(int));
    return stack;
}

// Stack is empty when top is equal to -1
int isEmpty(struct stack_of_iteration_intervals* stack)
{
    return stack->top == -1;
}
// Stack is full when top is equal to the last index
int isFull(struct stack_of_iteration_intervals* stack)
{
    return stack->top == stack->capacity - 1;
}
// Function to remove an item from stack.  It decreases top by 1
int pop(struct stack_of_iteration_intervals* stack)
{
    if (isEmpty(stack))
        return INT_MIN;
    return stack->array[stack->top--];
}
void push(struct stack_of_iteration_intervals* stack, int item)
{
    if (isFull(stack))
        return;
    stack->array[++stack->top] = item;
}
#endif //TRAINING_1_MYSTACK_H
