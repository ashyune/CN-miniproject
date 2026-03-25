#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/time.h>

#define BUFFER_SIZE 1024

double drift = 0.5;   // simulated clock drift (seconds)

double get_real_time()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec + tv.tv_usec / 1000000.0;
}

double get_local_time()
{
    return get_real_time() + drift;
}

int main()
{
    char buffer[BUFFER_SIZE];

    for(int i = 1; i <= 5; i++)
    {
        // Time when request is sent
        double T1 = get_local_time();

        // networking send/receive here 

        // Time when response is received
        double T4 = get_local_time();

        // These will be filled from received data
        double T1_recv, T2, T3;

        // buffer parsing happens here

        // Core synchronization calculations
        double delay = (T4 - T1_recv) - (T3 - T2);
        double offset = ((T2 - T1_recv) + (T3 - T4)) / 2;

        // Drift correction
        drift += offset;

        // Accuracy evaluation
        double server_time = T3;
        double client_time = get_local_time();

        double error = client_time - server_time;

        // printing happens here
        // sleep / loop control handled here
    }
}