#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>

enum { READ, WRITE };

int N;
char hijo_listo;
int pipe_interno[2];
int resultado_hijo;

int dameNumero(int pid) { return pid;}

void informarResultado(int numero, int resultado) {
    printf("Numero: %i, resultado: %i\n", numero, resultado);
}

int calcular() {
    int pid = getpid();
    sleep(pid % 3);
    return pid;
}

void atender_signal() {
    printf("He sido invocado!\n");
    read(pipe_interno[READ], &resultado_hijo, sizeof(resultado_hijo));
    hijo_listo = 1;
    printf("Resultado leído!\n");
}

void ejecutarHijo(int i, int pipes[][2]) {
    pipe(pipe_interno);
    signal(SIGCHLD, atender_signal);
    int numero_secreto;
    printf("%i | Esperando para leer mi número secreto\n", i);
    read(pipes[i][READ], &numero_secreto, sizeof(numero_secreto));
    printf("%i | Número secreto leído! (Shh, es %i)\n", i, numero_secreto);

    if (0 != fork()) {
        hijo_listo = 0;
        close(pipe_interno[WRITE]);

        int llamada;
        while(read(pipes[i][READ], &llamada, sizeof(llamada))) {
            // printf("%i | He sido consultado\n", i);
            if (hijo_listo) {
                write(pipes[N+i][WRITE], &hijo_listo, sizeof(hijo_listo));
                write(pipes[N+i][WRITE], &numero_secreto, sizeof(numero_secreto));
                write(pipes[N+i][WRITE], &resultado_hijo, sizeof(resultado_hijo));
                exit(EXIT_SUCCESS);
            } else {
                // printf("Todavía no estoy listo\n");
                write(pipes[N+i][WRITE], &hijo_listo, sizeof(hijo_listo));
            }
        }
    } else {
        close(pipe_interno[READ]);
        int resultado = calcular();
        printf("%i | Resultado calculado: %i\n", i, resultado);
        write(pipe_interno[WRITE], &resultado, sizeof(resultado));
    }

    exit(EXIT_SUCCESS);
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("Debe ejecutar con la cantidad de hijos como parametro\n");
        return 0;
    }
    N = atoi(argv[1]);
    int pipes[N*2][2];
    for (int i = 0; i < N*2; i++) {
        pipe(pipes[i]);
    }
    for (int i = 0; i < N; i++) {
        int pid = fork();
        if (pid == 0) {
            ejecutarHijo(i, pipes);
            return 0;
        } else {
            int numero = dameNumero(pid);
            write(pipes[i][WRITE], &numero, sizeof(numero));
        }
    }
    int cantidadTerminados = 0;
    char hijoTermino[N];
    for (int i = 0; i < N; i++) {
        hijoTermino[i] = 0;
    }
    while (cantidadTerminados < N) {
        for (int i = 0; i < N; i++) {
            if (hijoTermino[i]) {
                continue;
            }
            char termino = 0;
            // printf("Pasando tiempo padre-hijo con %i\n", i);
            write(pipes[i][WRITE], &termino, sizeof(termino));
            read(pipes[N+i][READ], &termino, sizeof(termino));
            if (termino) {
                int numero;
                int resultado;
                read(pipes[N+i][READ], &numero, sizeof(numero));
                read(pipes[N+i][READ], &resultado, sizeof(resultado));
                informarResultado(numero, resultado);
                hijoTermino[i] = 1;
                cantidadTerminados++;
            }
        }
    }
    wait(NULL);
    return 0;
}