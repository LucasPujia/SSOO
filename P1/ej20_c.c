#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/wait.h>
#include "ej19.h"

int main(int argc, char* argv[]) {
    for (int i = 0; i < 5; i++) {
        if (fork()) continue;
        long long numero;
        if (argc < 2) {
            numero = 37 * i;
        } else {
            numero = atoll(argv[1]) * (i+1);
        }
        numero = (numero ^ (0x5bd1e991 * numero)) | 0b1;
    
        int server_socket;
        struct sockaddr_un server_addr = {
            .sun_family = AF_UNIX,
            .sun_path = "socket_primo"
        };
    
        server_socket = socket(AF_UNIX, SOCK_STREAM, 0);
        if (connect(server_socket, (sockaddr_t) &server_addr, sizeof(server_addr)) == -1) {
            perror("Error");
            exit(EXIT_FAILURE);
        }
    
        if (write(server_socket, &numero, sizeof(numero)) == -1) {
            perror("Error");
            exit(EXIT_FAILURE);
        }
    
        bool es_primo;
        if (read(server_socket, &es_primo, sizeof(es_primo)) == 0) {
            perror("Error");
            exit(EXIT_FAILURE);
        }
        printf("%i | Me han respondido. El número %lli%s es primo\n", i, numero, es_primo ? "" : " no"); 
    
        close(server_socket);
        exit(EXIT_SUCCESS);
    }

    for (int i = 0; i < 5; i++) wait(NULL);
    exit(EXIT_SUCCESS);
}
