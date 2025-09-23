#include "ej19.h"
#include <signal.h>
#include <sys/wait.h>

enum { READ, WRITE };
#define MAX_CONNECTIONS 2

int connections_len = 0;
int connections[MAX_CONNECTIONS] = {0};
int pipes[MAX_CONNECTIONS][2];
int pipe_master[2];

int safe(int response_code) {
    if (response_code == -1) {
        perror("Safe error");
        exit(EXIT_FAILURE);
    }
    return response_code;
}

// MASTER functions
void connection_closed(int) {
    printf("Se ha cerrado una conexión\n");
    int child_pid = wait(NULL);
    printf("Chau zombie\n");
    for (int i = 0; i < MAX_CONNECTIONS; i++) {
        if (connections[i] == child_pid) { 
            connections[i] = 0;
            printf("Removing manager %i\n", i);
        }
    }
    connections_len--;
}

void message_received(int) {
    int mensaje;
    safe(read(pipe_master[READ], &mensaje, sizeof(mensaje)));
    printf("Transmitiendo el mensaje %i a los managers\n", mensaje);
    for (int i = 0; i < MAX_CONNECTIONS; i++) {
        if (connections[i] != 0) {
            printf("Manager %i\n", i);
            safe(write(pipes[i][WRITE], &mensaje, sizeof(mensaje)));
        }
    }
}

// MANAGER functions
void manage_client(int client_socket, int index){
    int reading_child_pid = fork();
    if (reading_child_pid == 0) {
        int broadcast;
        while(true) {
            safe(read(pipes[index][READ], &broadcast, sizeof(broadcast)));
            printf("Soy manager, leí %i\n", broadcast);
            safe(write(client_socket, &broadcast, sizeof(broadcast)));
            printf("Soy manager, escribí\n");
        }
    }

    int mensaje;
    int read_response;
    while(true) {
        read_response = read(client_socket, &mensaje, sizeof(mensaje));
        if (read_response == -1) {
            perror("Error reading");
            close(client_socket);
            exit(EXIT_FAILURE);
        } else if (read_response == 0) {
            printf("%i | Conexión finalizada.\n", client_socket);
            break;
        }

        printf("%i | Recibí el mensaje %i\n", client_socket, mensaje);

        safe(write(pipe_master[WRITE], &mensaje, sizeof(mensaje)));
        kill(getppid(), SIGUSR1);
        printf("%i | Listo!\n", client_socket);
    }

    printf("%i | Me fui\n", client_socket);
    close(client_socket);
    kill(reading_child_pid, SIGKILL);
    exit(EXIT_SUCCESS);
};

int main() {
    int server_socket = socket(AF_UNIX, SOCK_STREAM, 0);
    struct sockaddr_un client_addr;
    struct sockaddr_un server_addr = {
        .sun_family = AF_UNIX,
        .sun_path = "socket_wpp"
    };
    safe_bind(server_socket, &server_addr, sizeof(server_addr));    
    
    for (int i = 0; i < MAX_CONNECTIONS; i++) safe(pipe(pipes[i]));
    safe(pipe(pipe_master));

    signal(SIGCHLD, connection_closed);
    signal(SIGUSR1, message_received);

    safe_listen2(server_socket, &server_addr, 1);
    socklen_t len = sizeof(client_addr);
    while (true) {
        printf("Esperando conexiones...\n");
        int client_socket = accept(server_socket, (sockaddr_t) &client_addr, &len);
        printf("¡Conexión recibida con el número %i!\n", client_socket);
        if (connections_len == MAX_CONNECTIONS) {
            printf("Conexión rechazada debido a que se ha alcanzado el límite.\n");
            close(client_socket);
            continue;
        }
        if (client_socket == -1) {
            printf("error raro\n");
            continue;
        };

        connections_len++;
        printf("%i | Conexión aceptada!\n", client_socket);
        
        int index = -1;
        for (int i = 0; i < MAX_CONNECTIONS && index == -1; i++) {
            if (connections[i] == 0) {
                index = i;
                printf("index %i\n", i);
            }
        }

        if (index == -1) {
            printf("Error guardando la conexión, cerrando...\n");
            close(client_socket);
        } else {
            safe(write(client_socket, &index, sizeof(index)));
            int pid = fork();
            if (pid == 0) {
                manage_client(client_socket, index);
                printf("No creí llegar tan lejos\n");
                exit(EXIT_SUCCESS);
            } else {
                close(client_socket);
                connections[index] = pid;
            }
        }
    }

    printf("No creí llegar tan lejos\n");
    return 0;
}