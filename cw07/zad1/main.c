#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <limits.h>
#include <pthread.h>
#include <string.h>
#include <signal.h>

char **buffer;
pthread_mutex_t *mutexes;
pthread_cond_t conditionP;
pthread_cond_t conditionK;
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

    //mutexes destroy
    for (int i = 0; i < N + 2; i++)
    {
        pthread_mutexattr_destroy(&mutexes[i]);
    }
    free(mutexes);

    //destroy Condition Variables
    pthread_cond_destroy(&conditionP);
    pthread_cond_destroy(&conditionK);
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

void *producer(void *pVoid)
{
    int currentIndex;
    char lines[LINE_MAX];

    while (fgets(lines, LINE_MAX, file)) 
    {
        if (writeMode)
        {
            printf("Producer no %ld wants to take a fileline\n", pthread_self());
        }

        pthread_mutex_lock(&mutexes[N]); 

        while (buffer[nextProducerIndex] != NULL)
        {
            pthread_cond_wait(&conditionP, &mutexes[N]);
        }

        currentIndex = nextProducerIndex;

        if (writeMode)
        {
            printf("Producer no %ld wants to take %d buffer index\n", pthread_self(), currentIndex);
        }
        nextProducerIndex = (nextProducerIndex + 1) % N;

        pthread_mutex_lock(&mutexes[currentIndex]);
        buffer[currentIndex] = malloc((strlen(lines) + 1) * sizeof(char));
        strcpy(buffer[currentIndex], lines);

        if (writeMode)
        {
            printf("Producer no %ld has copied line to buffer at index %d\n", pthread_self(), currentIndex);
        }

        pthread_mutex_unlock(&mutexes[currentIndex]);
        pthread_mutex_unlock(&mutexes[N]);
        pthread_cond_broadcast(&conditionK);
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
        pthread_mutex_lock(&mutexes[N + 1]);

        //waiting when buffer of nextConsumerIndex is empty
        while (buffer[nextConsumerIndex] == NULL)
        {
            if (isFinished)
            {
                pthread_mutex_unlock(&mutexes[N + 1]);
                if (writeMode)
                {
                    printf("Consumer no %ld has finished work\n", pthread_self());
                }
                return NULL;
            }
            pthread_cond_wait(&conditionK, &mutexes[N + 1]);
        }

        currentIndex = nextConsumerIndex;
        if (writeMode)
        {
            printf("Consumer no %ld wants to take %d buffer index\n", pthread_self(), currentIndex);
        }
        nextConsumerIndex = (nextConsumerIndex + 1) % N;

        pthread_mutex_lock(&mutexes[currentIndex]);
        lines = buffer[currentIndex];
        buffer[currentIndex] = NULL;
        if (writeMode)
        {
            printf("Consumer no %ld has read line to buffer at index %d\n", pthread_self(), currentIndex);
        }

        pthread_mutex_unlock(&mutexes[currentIndex]);
        pthread_mutex_unlock(&mutexes[N + 1]);
        pthread_cond_broadcast(&conditionP);

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
        usleep(10);
    }
}

void createThreads(){
    //create threads
    producersThreads = malloc(P * sizeof(pthread_t));
    consumentsThreads = malloc(K * sizeof(pthread_t));
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

void initialize(){
    //allocate memory for buffer
    buffer = calloc((size_t)N, sizeof(char *));

    //create array of mutexes
    mutexes = malloc((N + 2) * sizeof(pthread_mutex_t));
    for (int i = 0; i < N + 2; i++)
    {
        pthread_mutex_init(&mutexes[i], NULL);
    }

    //Condition Variables init
    pthread_cond_init(&conditionP, NULL); 
    pthread_cond_init(&conditionK, NULL);
}

void joinThreads(){
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
