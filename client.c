#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/time.h>

#define PORT 12345
#define BUFFER_SIZE 1024

double drift = 0.5;   // initial artificial drift

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

if (sockfd < 0)
{
    perror("Socket creation failed");
    exit(1);
}

server_addr.sin_family = AF_INET;
server_addr.sin_port = htons(PORT);
inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);

printf("Initial Drift: %.6f sec\n", drift);

int i = 1;

while (1)
{
    double T1, T2, T3, T4;
    double delay, offset;

    // T1 (client send time)
    T1 = get_local_time();

    // Send T1 to server
    sprintf(buffer, "%lf", T1);
    sendto(sockfd, buffer, strlen(buffer), 0,
           (struct sockaddr *)&server_addr, sizeof(server_addr));

    // Receive T1, T2, T3 from server
    ssize_t n = recvfrom(sockfd, buffer, BUFFER_SIZE - 1, 0, NULL, NULL);
    if (n < 0)
        continue;

    buffer[n] = '\0';

    // T4 (client receive time)
    T4 = get_local_time();

    double T1_recv;
    sscanf(buffer, "%lf %lf %lf", &T1_recv, &T2, &T3);

    // NTP calculations
    delay = (T4 - T1_recv) - (T3 - T2);
    offset = ((T2 - T1_recv) + (T3 - T4)) / 2;

    // Smooth drift correction
    drift += offset * 0.5;

    // Accuracy evaluation
    double server_time = T3;
    double client_time = get_local_time();
    double error = client_time - server_time;

    printf("\n--- Iteration %d ---\n", i++);
    printf("Delay          : %.6f sec\n", delay);
    printf("Offset         : %.6f sec\n", offset);
    printf("Updated Drift  : %.6f sec\n", drift);
    printf("Server Time    : %.6f\n", server_time);
    printf("Client Time    : %.6f\n", client_time);
    printf("Sync Error     : %.6f sec\n", error);

    sleep(2);
}

close(sockfd);
return 0;
}
