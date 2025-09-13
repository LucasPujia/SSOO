#include <math.h>
#include "ej19.h"

bool es_primo(long long numero, int num) {
    if (numero < 2) return -1;
    long root = sqrt(numero);
    sleep(num % 4 + 1);
    for (long i = 2; i <= root; i++) {
        if (numero % i == 0) return false;
    }

    return true;
}

int main() {
    int server_socket = socket(AF_UNIX, SOCK_STREAM, 0);
    struct sockaddr_un server_addr = {
        .sun_family = AF_UNIX,
        .sun_path = "socket_primo"
    };
    safe_bind(server_socket, &server_addr, sizeof(server_addr));    
    
    safe_listen2(server_socket, &server_addr, 10);
    while(true) {
        int client_addr;
        socklen_t len = sizeof(client_addr);
        int client_socket = accept(server_socket, (sockaddr_t) &client_addr, &len);
        printf("%i | Conexión aceptada!\n", client_socket);

        if(fork() == 0) {
            long long numero;
            if (read(client_socket, &numero, sizeof(numero)) == -1) {
                perror("Error reading");
                exit(EXIT_FAILURE);
            }
            printf("%i | Recibí el número %lli\n", client_socket, numero);
    
            
            int primo = es_primo(numero, client_socket);
            if (write(client_socket, &primo, sizeof(primo)) == -1) {
                perror("Error writing");
                exit(EXIT_FAILURE);
            };
            printf("%i | Listo!\n", client_socket);
    
            close(client_socket);
            exit(EXIT_SUCCESS);
        }
    }

}