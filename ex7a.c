/*
======================================================================
File: ex7a.c
======================================================================
Writen by : Rotem Kashani, ID = 209073352
Mahmood Jazmawy, ID = 211345277

* this program fills an array with prime numbers in an ascending order.
* we initialize a global array with the size of 101. the main thread zeros the cells.
* the first cell is used as a lockign mechanism, 0=locked.
* the main thread creates three other threads, each thread draws a random number
* and tries to add it to the array. and after they try 100 times, doesn't matter if they
* succeeded it insert numbers or not. they finish and print to the console how many 
* numbers they managed to insert
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <math.h>
#include <stdbool.h>

#define ARR_SIZE 101
#define NUM_OF_THREADS 3
#define SEED 17
#define CLOSE 0
#define OPEN 1
#define MY_LOCK 0
#define RUN 1

int prime_arr[ARR_SIZE] = { 0 };
int max_prime = 0;
int index_run = 1;
bool end = false;

void* rand_num(void* arg);
int find_prime();
bool is_prime(int num);
void check_add_prime(int* new_primes, int* failed_counter);

int main()
{
	pthread_t thread_data[NUM_OF_THREADS];
	int status;
	int i;
	srand(SEED);
	for (i = 0; i < NUM_OF_THREADS; i++)
	{
		status = pthread_create(&thread_data[i], NULL, rand_num, NULL);
		if (status != 0)
		{
			fputs("pthread create failed from main procces", stderr);
			exit(EXIT_FAILURE);
		}
	}
	prime_arr[MY_LOCK] = OPEN;
	for (i = 0; i < NUM_OF_THREADS; i++)
	{
		pthread_join(thread_data[i], NULL);
	}
	printf("size is: %d, min is: %d, max is:%d\n", index_run - 1, prime_arr[1], max_prime);
	return EXIT_SUCCESS;
}

/*
* creates and tries to insert prime numbers
*/
void* rand_num(void* arg)
{
	int new_primes;
	int failed_counter;

	while (1)
	{
		if (prime_arr[MY_LOCK] == OPEN)
		{
			prime_arr[MY_LOCK] = CLOSE;
			check_add_prime(&new_primes, &failed_counter);
			if (index_run < ARR_SIZE - 1 && failed_counter < 100)
			{
				prime_arr[MY_LOCK] = OPEN;
			}
			else
				end = true;
		}
		if (end)
		{
			printf("found %d primes\n", new_primes);
			pthread_exit(NULL);
		}

	}
	pthread_exit(NULL);
}

/*
* creates abd returns a prime number
*/
int find_prime()
{
	int num;
	while (RUN)
	{
		num = rand() % ((int)(pow(10, 6)) - 1) + 2;
		if (is_prime(num))
		{
			return num;
		}

	}
}

/*
* checks if a number is prime
*/
bool is_prime(int num) {
	int mid = num / 2;
	if (num < 2) {
		return false;
	}
	for (int i = 2; i <= mid; i++) {
		if (num % i == 0) {
			return false;
		}
	}
	return true;
}

/*
* checks if the prime number was added
*/
void check_add_prime(int* new_primes, int* failed_counter)
{
	int num;
	num = find_prime();
	if (*failed_counter < ARR_SIZE - 1)
	{
		if (num >= max_prime)
		{
			max_prime = num;
			(*new_primes)++;
			prime_arr[index_run] = num;
			index_run++;

		}

		else
			(*failed_counter)++;
	}
}
