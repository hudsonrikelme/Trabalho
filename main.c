#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <stdbool.h>

#define MAX_USUARIOS 5
#define MAX_ARQUIVOS 12
#define MAX_NOME_ARQUIVO 250

pthread_mutex_t mutex; // Declaração do mutex

typedef struct
{
    char nome[10];
    char arquivos[MAX_ARQUIVOS][100];
    char ausentes[MAX_ARQUIVOS][100];
    int num_arquivos;
} Usuario;

Usuario usuarios[MAX_USUARIOS];

void solicitar_arquivo(const char *nome_arquivo)
{
    pthread_mutex_lock(&mutex); // Bloqueia o mutex antes de imprimir
    printf("TESTE Solicitar arquivo: %s\n", nome_arquivo);
    pthread_mutex_unlock(&mutex); // Desbloqueia o mutex após imprimir
}

void *receber_arquivo_thread(void *arg)
{
    char *nome_arquivo = (char *)arg;
    // Realiza a solicitação do arquivo que o usuario nao possui
    solicitar_arquivo(nome_arquivo);
    free(nome_arquivo);
    pthread_exit(NULL);
    return NULL;
}

void *usuario_thread(void *arg)
{
    Usuario *usuario = (Usuario *)arg;
    char *diretorio = usuario->nome;
    //pthread_t threads_arquivos[MAX_ARQUIVOS];
    //  Abre o diretório

    DIR *dir = opendir(diretorio);
    if (dir == NULL)
    {
        printf("Erro ao abrir o diretorio");
        free(usuario->nome); // Libera a memória alocada
        pthread_exit(NULL);
    }

    struct dirent *entry;
    // Define os nomes de todos os arquivos existentes
    char arquivos_desejados[MAX_ARQUIVOS][MAX_NOME_ARQUIVO] = {
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
            // Teste obs. Excluir depois
            printf("[%s] Arquivos Existentes -> %s\n", diretorio, entry->d_name);
            strcpy(usuario->arquivos[usuario->num_arquivos], entry->d_name);
            usuario->num_arquivos++;
            pthread_mutex_unlock(&mutex); // Desbloqueia o mutex após imprimir
        }
    }

    // Entende quais arquivos estão ausentes
    for (int i = 0; i < MAX_ARQUIVOS; i++)
    {
        bool ausente = true;
        for (int j = 0; j < usuario->num_arquivos; j++)
        {
            if (strcmp(usuario->arquivos[j], arquivos_desejados[i]) == 0)
            {
                ausente = false;
                break;
            }
        }
        if (ausente)
        {
            strcpy(usuario->ausentes[usuario->num_arquivos], arquivos_desejados[i]);
            usuario->num_arquivos++;

            pthread_mutex_lock(&mutex);
            printf("[%s] Arquivos Desejados -> %s\n", diretorio, arquivos_desejados[i]);
            pthread_mutex_unlock(&mutex);
        }
    }

    pthread_t threads_arquivos[MAX_ARQUIVOS];
    // Analisa quais arquivos o usuário não tem e cria as threads de solicitação de arquivos
    int count = 0;
    int num_solicitacao = rand() % MAX_ARQUIVOS + 1;

    for (int i = 0; i < num_solicitacao; i++)
    {
        int random_index = rand() % MAX_ARQUIVOS;
        if (arquivos_desejados[random_index][0] != '\0')
        {

            char *arquivo_ausente = malloc(strlen(arquivos_desejados[random_index]) + 1);
            strcpy(arquivo_ausente, arquivos_desejados[random_index]);
            pthread_create(&threads_arquivos[i], NULL, receber_arquivo_thread, arquivo_ausente);
            count++;
        }
    }
    printf("\n");

    // Aguarda a finalização das threads dos arquivos
    for (int i = 0; i < count; i++)
    {
        pthread_join(threads_arquivos[i], NULL);
    }

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

    int num_usuarios = atoi(argv[1]);
    int max_arquivos = atoi(argv[2]);
    /*int tmn_Fragmento = atoi(argv[3]);
    int tmn_Buffer = atoi(argv[4]);*/

    if (num_usuarios <= 0 || num_usuarios > MAX_USUARIOS)
    {
        printf("Quantidade de usuários invalida!\n");
        return 1;
    }
    if (max_arquivos <= 0 || max_arquivos > MAX_ARQUIVOS)
    {
        printf("Quantidade de arquivos invalida!\n");
        return 1;
    }

    srand(time(0)); // Inicializa a semente de números aleatórios

    pthread_t threads[num_usuarios];
    Usuario usuarios[num_usuarios];

    pthread_mutex_init(&mutex, NULL); // Mutex iniciado
    // Criação das threads dos usuários
    for (int i = 0; i < num_usuarios; i++)
    {
        sprintf(usuarios[i].nome, "U%d", i + 1);
        usuarios[i].num_arquivos = 0;
        pthread_create(&threads[i], NULL, usuario_thread, &usuarios[i]);
    }
    // Aguarda a finalização das threads dos usuários
    for (int i = 0; i < num_usuarios; i++)
    {
        pthread_join(threads[i], NULL);
    }

    pthread_mutex_destroy(&mutex); // Destroi o mutex

    return 0;
}
