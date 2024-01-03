/*
======================================================================
File: ex7b.c
======================================================================
Writen by : Rotem Kashani, ID = 209073352
Mahmood Jazmawy, ID = 211345277

* this program, main thread creates 3 sons and saves their id in a globa variable.
* each son is a server that does a certain "job".
* First server: prime server, it goes to sleep  and wait to get SIGUSR1 signal
* when it wakes up it gets the data in the global variable and checks for prime numbers and puts
* the result in the second array.

Second server: th epalindrome server: this server waits for SIGUSR2 and when it gets it it checks the
* data if it's a palindrome it returns 1 in the first cell.

* and the third server is the front end which takes input from the user and send it to the server and receives an answer.
*/

// --------------------------- include section ----------------------
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <stdbool.h>

// --------------------------- consts section ----------------------
#define NUMBER_OF_THREADS 3
#define PRIMES_ARRAY_SIZE 100
#define PALINDROME_ARRAY_SIZE 2
#define PRIME 'p'
#define PALINDROME 'q'
#define NUM 1
#define ANSWER 0

const int PRIMES_THREAD = 0;
const int PALINDROME_THREAD = 1;
const int CLIENT_THREAD = 2;

const int NOT = 0;//not prime/pali
const int IS = 1;//is prime/pali



// --------------------------- global ----------------------
pthread_t threads_array[NUMBER_OF_THREADS];

int primes_array[PRIMES_ARRAY_SIZE];
int primes_answer[PRIMES_ARRAY_SIZE];

int palindrome_array[PALINDROME_ARRAY_SIZE];

// --------------------------- prototypes ----------------------
void create_threads();
void* primes_func(void*);
void* palindrome_func(void*);
void* client_func(void*);
void catch_sigusr1(int sig_num);
void catch_sigusr2(int sig_num);
int is_palindrome();
bool is_prime(int num);


// --------------------------- main ----------------------
int main()
{
	signal(SIGUSR1, catch_sigusr1);
	signal(SIGUSR2, catch_sigusr2);
	create_threads();

	pthread_exit(NULL);
	return (EXIT_SUCCESS);
}

// --------------------------- create_threads ----------------------
void create_threads()
{
	if (pthread_create(&threads_array[PRIMES_THREAD], NULL, primes_func, NULL)
		!= 0)
	{
		fputs("pthread_create primes failed", stderr);
		exit(EXIT_FAILURE);
	}

	if (pthread_create(&threads_array[PALINDROME_THREAD], NULL, palindrome_func,
		NULL) != 0)
	{
		fputs("pthread_create plaindrome failed", stderr);
		exit(EXIT_FAILURE);
	}

	if (pthread_create(&threads_array[CLIENT_THREAD], NULL, client_func, NULL)
		!= 0)
	{
		fputs("pthread_create client failed", stderr);
		exit(EXIT_FAILURE);
	}
}

// --------------------------- primes_func ----------------------
void* primes_func(void* data)
{
	signal(SIGUSR1, catch_sigusr1);

	while (1)
	{
		pause();
		for (int i = 0; i < PRIMES_ARRAY_SIZE; i++)
		{
			if (primes_array[i] == 0)
				break;

			if (is_prime(primes_array[i]))
			{
				primes_answer[i] = IS;
			}
			else
				primes_answer[i] = NOT;
		}
		pthread_kill(threads_array[CLIENT_THREAD], SIGUSR1);

	}
	pthread_exit(NULL);
}

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

// --------------------------- palindrome_func ----------------------
void* palindrome_func(void* data)
{
	signal(SIGUSR2, catch_sigusr2);
	while (1)
	{
		pause();
		palindrome_array[ANSWER] = is_palindrome();
		pthread_kill(threads_array[CLIENT_THREAD], SIGUSR2);
	}
	pthread_exit(NULL);
}

int is_palindrome()
{
	int original = palindrome_array[NUM], remainder, reversed = 0;
	int num = palindrome_array[NUM];
	while (num != 0) {
		remainder = num % 10;
		reversed = reversed * 10 + remainder;
		num /= 10;
	}

	// palindrome if orignal and reversed are equal
	if (original == reversed)
		return IS;
	else
		return NOT;
}

// ------------------------- client_func -------------------
void* client_func(void* data)
{
	char c;

	while (1)
	{
		scanf("%c", &c);
		if (c == PRIME)
		{
			for (int i = 0; i < PRIMES_ARRAY_SIZE; i++)
			{
				primes_array[i] = 0;
			}

			for (int i = 0; i < PRIMES_ARRAY_SIZE; i++)
			{
				scanf("%d", &primes_array[i]);
				if (primes_array[i] == 0)
					break;
			}
			pthread_kill(threads_array[PRIMES_THREAD], SIGUSR1);
			pause();
			for (int i = 0; i < PRIMES_ARRAY_SIZE; i++)
			{
				if (primes_answer[i] == IS)
				{
					printf("%d ", primes_array[i]);
				}
				else if (primes_answer[i] == NOT)
				{
					continue;
				}
				else
					break;
			}
			printf("\n");
		}
		else if (c == PALINDROME)
		{
			for (int i = 0; i < PALINDROME_ARRAY_SIZE; i++)
			{
				palindrome_array[i] = 0;
			}
			scanf("%d", &palindrome_array[NUM]);
			pthread_kill(threads_array[PALINDROME_THREAD], SIGUSR2);
			pause();
			if (palindrome_array[ANSWER] == IS)
			{
				printf("is palindrome\n");
			}
			else if (palindrome_array[ANSWER] == NOT)
			{
				printf("is not palindrome\n");
			}

		}
	}
}

// --------------------------- catch_sigusr ----------------------------
void catch_sigusr1(int sig_num)
{
	signal(SIGUSR1, catch_sigusr1);

}

void catch_sigusr2(int sig_num)
{
	signal(SIGUSR2, catch_sigusr2);

}
