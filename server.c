#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define PORT 8888
#define MAX_BUFFER_SIZE 1024
#define MAX_DATA_SIZE 200

typedef struct {
    int id;
    int row_number;
    int bookshelf_number;
    int book_number;
    int code;
} Book;

int main(int argc, char *argv[]) {
    int sockfd = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sockfd < 0) {
        perror("Error creating sockets");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    if (argc == 3 ) {
        server_addr.sin_port = htons(atoi(argv[2]));
        server_addr.sin_addr.s_addr = inet_addr(argv[1]);
    } else {
        server_addr.sin_port = htons(PORT);
        server_addr.sin_addr.s_addr = INADDR_ANY;
    }
    if (bind(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Error binding socket");
        exit(EXIT_FAILURE);
    }
    Book received_data;
    char buffer[MAX_BUFFER_SIZE];
    struct sockaddr_in client_addr;
    int client_len = sizeof(client_addr);
    int data_count = 0;
    Book data_array[MAX_DATA_SIZE];
    while (1) {
        if (data_count == MAX_DATA_SIZE) {
            printf("Каталог заполнен. запись окончена");
            break;
        }
        client_len = sizeof(client_addr);
        memset(buffer, 0, MAX_BUFFER_SIZE);
        int bytes_received = recvfrom(sockfd, buffer, MAX_BUFFER_SIZE, 0, (struct sockaddr*)&client_addr, &client_len);
        if (bytes_received < 0) {
            perror("Ошибка получения данных");
            exit(EXIT_FAILURE);
        }
        memcpy(&received_data, buffer, sizeof(received_data));
        sleep(2);
        if (received_data.code == 0) {
            Book message;
            message.code = 1;
            memset(buffer, 0, MAX_BUFFER_SIZE);
            memcpy(buffer, &message, sizeof(message));
            sendto(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr*)&client_addr, sizeof(client_addr));
            data_array[data_count] = received_data;
            data_count++;
            printf("Получены данные от клиента: ");
            printf("%d %d %d %d\n", received_data.id, received_data.row_number, received_data.bookshelf_number, received_data.book_number);
        }
    }
    close(sockfd);
    return 0;
}