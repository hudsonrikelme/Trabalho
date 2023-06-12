#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>

#define MAX_USUARIOS 5
#define MAX_ARQUIVOS 12
#define MAX_NOME_ARQUIVO 256

pthread_mutex_t mutex; // Declaração do mutex

void *usuario_thread(void *arg)
{
    char *diretorio = (char *)arg;

    // Abre o diretório
    DIR *dir = opendir(diretorio);
    if (dir == NULL)
    {
        printf("Erro ao abrir o diretorio");
        free(diretorio); // Libera a memória alocada
        pthread_exit(NULL);
    }

    struct dirent *entry;
    // Define os nomes de todos os arquivos existentes
    char arquivos_ausentes[MAX_ARQUIVOS][MAX_NOME_ARQUIVO] = {
        "file1.csv", "file2.csv", "file3.csv", "file4.csv",
        "file5.csv", "file6.csv", "file7.csv", "file8.csv",
        "file9.csv", "file10.csv", "file11.csv", "file12.csv"};

    // Lê cada entrada do diretório usando a função readdir
    while ((entry = readdir(dir)) != NULL)
    {
        // Se houver a entrada de um arquivo regular
        if (entry->d_type == DT_REG)
        {
            pthread_mutex_lock(&mutex); // Bloqueia o mutex antes de imprimir
            printf("[%s] Arquivos Existentes -> %s\n", diretorio, entry->d_name);
            pthread_mutex_unlock(&mutex); // Desbloqueia o mutex após imprimir

            // Compara arquivo com a lista de todos os arquivos
            pthread_mutex_lock(&mutex); // Bloqueia o mutex antes de imprimir
            
            for (int i = 0; i < MAX_ARQUIVOS; i++)
            {
                if (strcmp(entry->d_name, arquivos_ausentes[i]) == 0)
                {
                    arquivos_ausentes[i][0] = '\0';
                    break;
                }
                
            }
            pthread_mutex_unlock(&mutex); // Desbloqueia o mutex após imprimir
        }
    }

    // Teste Ausentes
    pthread_mutex_lock(&mutex); // Bloqueia o mutex antes de imprimir

    for (int i = 0; i < MAX_ARQUIVOS; i++)
    {
        if(arquivos_ausentes[i][0] != '\0')
        printf("[%s] Arquivos Ausentes -> %s\n", diretorio, arquivos_ausentes[i]);
    }
    printf("\n");

    pthread_mutex_unlock(&mutex); // Desbloqueia o mutex após imprimir

    // Diretorio fechado
    closedir(dir);
    free(diretorio);
    pthread_exit(NULL);
    return NULL;
}

int main(int argc, char *argv[])
{
    if (argc < 4)
    {
        printf("Informe a quantidade de usuarios, quantidade de arquivos, tamanho do fragmento de arquivo e tamanho do buffer como argumento!\n");
        return 1;
    }

    int usuarios = atoi(argv[1]);
    int max_arquivos = atoi(argv[2]);
    /*int tmn_Fragmento = atoi(argv[3]);
    int tmn_Buffer = atoi(argv[4]);*/

    if (usuarios <= 0 || usuarios > MAX_USUARIOS)
    {
        printf("Quantidade de usuários invalida!\n");
        return 1;
    }
    if (max_arquivos <= 0 || max_arquivos > MAX_ARQUIVOS)
    {
        printf("Quantidade de arquivos invalida!\n");
        return 1;
    }

    pthread_t threads[MAX_USUARIOS];
    char diretorio[4];
    pthread_mutex_init(&mutex, NULL); // Mutex iniciado
    // Criação das threads dos usuários
    for (int i = 0; i < usuarios; i++)
    {
        sprintf(diretorio, "U%d", i + 1);
        char *diretorio_thread = malloc(strlen(diretorio) + 1); // Aloca memória para o nome do diretório
        strcpy(diretorio_thread, diretorio);
        pthread_create(&threads[i], NULL, usuario_thread, diretorio_thread);
    }
    // Aguarda a finalização das threads dos usuários
    for (int i = 0; i < usuarios; i++)
    {
        pthread_join(threads[i], NULL);
    }

    pthread_mutex_destroy(&mutex); // Destroi o mutex

    return 0;
}
