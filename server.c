#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/time.h>

#define PORT 5005
#define BUFFER_SIZE 1024

double get_time()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec + tv.tv_usec / 1000000.0;
}

int main()
{
    int sockfd;
    char buffer[BUFFER_SIZE];
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr));

    printf("Clock Server Running...\n");

    while (1)
    {
        ssize_t n = recvfrom(sockfd, buffer, BUFFER_SIZE - 1, 0,
                             (struct sockaddr *)&client_addr, &addr_len);
        if (n < 0)
            continue;
        buffer[n] = '\0';

        double T2 = get_time(); // server receive time
        double T3 = get_time(); // server send time

        double T1;
        sscanf(buffer, "%lf", &T1);

        char response[BUFFER_SIZE];
        sprintf(response, "%lf %lf %lf", T1, T2, T3);

        sendto(sockfd, response, strlen(response), 0,
               (struct sockaddr *)&client_addr, addr_len);

        printf("Client synchronized\n");
    }

    close(sockfd);
}