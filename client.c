#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/time.h>

#define PORT 5005
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
    int sockfd;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);

    printf("Initial Clock Drift: %.6f seconds\n\n", drift);

    for(int i=1;i<=5;i++)
    {
        double T1 = get_local_time();

        sprintf(buffer, "%lf", T1);

        sendto(sockfd, buffer, strlen(buffer), 0,
               (struct sockaddr *)&server_addr, sizeof(server_addr));

        ssize_t n = recvfrom(sockfd, buffer, BUFFER_SIZE - 1, 0, NULL, NULL);
        if (n < 0)
            continue;
        buffer[n] = '\0';

        double T4 = get_local_time();

        double T1_recv, T2, T3;
        sscanf(buffer, "%lf %lf %lf", &T1_recv, &T2, &T3);

        double delay = (T4 - T1_recv) - (T3 - T2);
        double offset = ((T2 - T1_recv) + (T3 - T4)) / 2;

        printf("----- Sync Cycle %d -----\n", i);

        printf("Round Trip Delay: %.6f\n", delay);
        printf("Calculated Offset: %.6f\n", offset);

        // Drift correction
        drift += offset;

        printf("Updated Drift After Correction: %.6f\n", drift);

        // Accuracy evaluation
        double server_time = T3;
        double client_time = get_local_time();

        double error = client_time - server_time;

        printf("Synchronization Error: %.6f seconds\n\n", error);

        sleep(3);
    }

    close(sockfd);
}