#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <limits.h>
#include <pthread.h>
#include <semaphore.h>
#include <string.h>
#include <signal.h>

char **buffer;
sem_t *b_sem;
int P, K, N, L, nk, searchMode, writeMode;
FILE *file;
char fileName[FILENAME_MAX];
int nextProducerIndex = 0;
int nextConsumerIndex = 0;
bool isFinished = false;
pthread_t *producersThreads;
pthread_t *consumentsThreads;

void cancelThreads()
{
    //cancel producers' threads
    for (int i = 0; i < P; i++)
    {
        pthread_cancel(producersThreads[i]); 
    }
    free(producersThreads);

    //cancel consumers' threads
    for (int i = 0; i < K; i++)
    {
        pthread_cancel(consumentsThreads[i]); 
    }
    free(consumentsThreads);
}

void clean()
{
    //close file
    if (file)
        fclose(file);

    //free buffer
    for (int i = 0; i < N; ++i)
    {
        if (buffer[i])
        {
            free(buffer[i]);
        }
    }
    free(buffer);

    //destroy semaphores
    for (int i = 0; i < N + 4; ++i)
    {
        sem_destroy(&b_sem[i]);
    }
    free(b_sem);
}

bool openConfigurationFile(char *configFilePath)
{
    FILE *configFile;
    configFile = fopen(configFilePath, "r");
    if (configFile == NULL)
    {
        printf("Cannot open config file\n");
        return false;
    }
    int fscanfRespond = fscanf(configFile, "%d %d %d %s %d %d %d %d", &P, &K, &N, fileName, &L, &searchMode, &writeMode, &nk); //P, K, N, nazwa pliku, L, tryb wyszukiwania, tryb wypisywania informacji oraz nk
    if (fscanfRespond < 8)
    {
        printf("Cannot scan from config file\n");
        return false;
    }
    printf("CONFIGURATION FILE\nP: %d\nK: %d\nN: %d\nFile Name: %s\nL: %d\nSearch Mode: %d\nWrite status: %d\nnk: %d\n", P, K, N, fileName, L, searchMode, writeMode, nk);
    fclose(configFile);
    return true;
}

void handlerSIGINTandSIGALRM(int sig)
{
    //in case of SIGINT and SIGALRM cancel existing threads
    if (sig == SIGINT)
    {
        printf("Signal SIGINT.\n");
        cancelThreads();
    }
    else if (sig == SIGALRM)
    {
        printf("Signal SIGINT.\n");
        cancelThreads();
    }
    else
    {
        printf("Signal no %d: \n, sig");
    }
    exit(0);
}

void *producer(void *pVoid)
{
    int currentIndex;
    char line[LINE_MAX];

    while (fgets(line, LINE_MAX, file) != NULL) 
    {
        if (writeMode)
        {
            printf("Producer no %ld wants to take a fileline\n", pthread_self());
        }
        sem_wait(&b_sem[N]);

        sem_wait(&b_sem[N + 2]);

        currentIndex = nextProducerIndex;
        if (writeMode)
        {
            printf("Producer no %ld wants to take %d buffer index\n", pthread_self(), currentIndex);
        }
        nextProducerIndex = (nextProducerIndex + 1) % N;

        sem_wait(&b_sem[currentIndex]);
        sem_post(&b_sem[N]);

        buffer[currentIndex] = malloc((strlen(line) + 1) * sizeof(char));
        strcpy(buffer[currentIndex], line);
        if (writeMode)
        {
            printf("Producer no %ld has copied line to buffer at index %d\n", pthread_self(), currentIndex);
        }

        sem_post(&b_sem[currentIndex]);
    }
    if (writeMode)
    {
        printf("Producer no %ld has finished work\n", pthread_self());
    }
    return NULL;
}

void *consumer(void *pVoid)
{
    char *lines;
    int currentIndex;
    int lineLength;
    while (1)
    {
        sem_wait(&b_sem[N + 1]);
        while (buffer[nextConsumerIndex] == NULL)
        {
            sem_post(&b_sem[N + 1]);
            if (isFinished)
            {
                if (writeMode)
                {
                    printf("Consumer no %ld has finished work\n", pthread_self());
                }
                return NULL;
            }
            sem_wait(&b_sem[N + 1]);
        }

        currentIndex = nextConsumerIndex;
        if (writeMode)
        {
            printf("Consumer no %ld wants to take %d buffer index\n", pthread_self(), currentIndex);
        }
        nextConsumerIndex = (nextConsumerIndex + 1) % N;

        sem_wait(&b_sem[currentIndex]);

        lines = buffer[currentIndex];
        buffer[currentIndex] = NULL;
        if (writeMode)
        {
            printf("Consumer no %ld has read line to buffer at index %d\n", pthread_self(), currentIndex);
        }

        sem_post(&b_sem[N + 2]);
        sem_post(&b_sem[N + 1]);
        sem_post(&b_sem[currentIndex]);

        lineLength = (int)strlen(lines);
        if (lineLength > L)
        {
            if (writeMode)
            {
                printf("Consumer no %ld got line %s with length > L, index %d\n", pthread_self(), lines, currentIndex);
            }
        }
        else if (lineLength < L)
        {
            if (writeMode)
            {
                printf("Consumer no %ld got line %s with length < L, index %d\n", pthread_self(), lines, currentIndex);
            }
        }
        else
        {
            if (writeMode)
            {
                printf("Consumer no %ld got line %s with length == L, index %d\n", pthread_self(), lines, currentIndex);
            }
        }
        free(lines);
    }
}

void initialize()
{
    //allocate memory for buffer
    buffer = calloc((size_t)N, sizeof(char *));

    //create array of semaphores
    b_sem = malloc((N + 3) * sizeof(sem_t));
    for (int i = 0; i < N + 2; ++i)
    {
        sem_init(&b_sem[i], 0, 1);
    }
    sem_init(&b_sem[N + 2], 0, (unsigned int)N);

    //create threads
    producersThreads = malloc(P * sizeof(pthread_t));
    consumentsThreads = malloc(K * sizeof(pthread_t));
}

void createThreads()
{
    for (int i = 0; i < P; i++)
    {
        pthread_create(&producersThreads[i], NULL, producer, NULL);
    }

    for (int i = 0; i < K; i++)
    {
        pthread_create(&consumentsThreads[i], NULL, consumer, NULL);
    }
    if (nk > 0)
        alarm(nk);
}

void joinThreads()
{
    //wait for terminate threads
    for (int i = 0; i < P; i++)
    {
        pthread_join(producersThreads[i], NULL);
    }
    isFinished = true;
    for (int i = 0; i < K; i++)
    {
        pthread_join(consumentsThreads[i], NULL);
    }
}

int main(int argc, char **argv)
{

    //check that is there enough arguments
    if (argc < 2)
    {

        perror("Not enough arguments! \n");
        exit(-1);
    }

    //function clean will be called while program ending
    if (atexit(clean) == -1)
    {
        perror("Function atexit failed\n");
        exit(-2);
    }

    //examine and change signal action
    struct sigaction act;
    act.sa_handler = handlerSIGINTandSIGALRM;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    //SIGINT
    if (sigaction(SIGINT, &act, NULL) == -1)
    {
        perror("SIGINT catching failed\n");
        exit(-2);
    }
    //SIGALRM
    if (nk > 0)
    {
        if (sigaction(SIGALRM, &act, NULL) == -1)
        {
            perror("SIGALRM catching failed\n");
            exit(-2);
        }
    }

    //open configuration file
    if (openConfigurationFile(argv[1]) == false)
    {
        perror("Open config file failed\n");
        exit(-3);
    }

    //open source file (with  book text)
    file = fopen(fileName, "r");
    if (file == NULL)
    {
        perror("Cannot open source file\n");
        exit(-3);
    }

    initialize();

    createThreads();

    joinThreads();

    return 0;
}
