#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>

#define MAX_USUARIOS 5
#define MAX_ARQUIVOS 12

void *usuario_thread(void *arg) {
    char *diretorio = (char *)arg;

    DIR *dir = opendir(diretorio);
    if (dir == NULL) {
        printf("Erro ao abrir o diretorio %s\n", diretorio);
        pthread_exit(NULL);
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_REG) {
            printf("%s, Arquivo: %s\n", diretorio, entry->d_name);
        }
    }

    closedir(dir);
    return NULL;
}

int main(int argc, char *argv[]) {
    if (argc < 5) {
        printf("Informe a quantidade de usuarios, quantidade de arquivos, tamanho do fragmento de arquivo e tamanho do buffer como argumento!\n");
        return 1;
    }

    int usuarios = atoi(argv[1]);
    int max_arquivos = atoi(argv[2]);
    /*int tmn_Fragmento = atoi(argv[3]);
    int tmn_Buffer = atoi(argv[4]);*/

    if (usuarios <= 0 || usuarios > MAX_USUARIOS) {
        printf("Quantidade de usuários invalida!\n");
        return 1;
    }
    if (max_arquivos <= 0 || max_arquivos > MAX_ARQUIVOS) {
        printf("Quantidade de arquivos invalida!\n");
        return 1;
    }

    pthread_t threads[MAX_USUARIOS];

    // Criação das threads dos usuários
    for (int i = 0; i < usuarios; i++) {
        char diretorio[10];
        sprintf(diretorio, "U%d", i+1);
        if (pthread_create(&threads[i], NULL, usuario_thread, (void *)diretorio) != 0) {
            printf("Erro ao criar a thread do usuario %s\n", diretorio);
            return 1;
        }
    }

    // Aguarda a finalização das threads dos usuários
    for (int i = 0; i < usuarios; i++) {
        pthread_join(threads[i], NULL);
    }

    return 0;
}
