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
    char nome[MAX_NOME_ARQUIVO];
    int fragmentos;
} Arquivo;

typedef struct
{
    char nome[10];
    Arquivo arquivos[MAX_ARQUIVOS];
    Arquivo ausentes[MAX_ARQUIVOS];
    int num_arquivos;
    int num_ausentes;
} Usuario;

typedef struct
{
    Arquivo arquivo;
    Usuario *usuario;
    struct solicitacoes *proximo;
} Solicitacao;

typedef struct
{
    Usuario *usuario;
    int tmn_fragmento;
    int tmn_buffer;
} ThreadArgs;

Arquivo total_arquivos[MAX_ARQUIVOS];

Solicitacao solicitacoes;

Usuario usuarios[MAX_USUARIOS];



/**
 * to do
 * Função que divide o arquivo em fragmentos iguais
 * 
*/
/*void divide_fragmentos(int tmn_fragmento, char *nome){
    

}
*/


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
/**
 *
 * Entende quais arquivos o usuario não tem
 *
 */

void ausentes_diretorio(Usuario *usuario)
{

    /*
        // Entende quais arquivos estão ausentes
        for (int i = 0; i < MAX_ARQUIVOS; i++)
        {
            bool ausente = true;
            for (int j = 0; j < usuario->num_arquivos; j++)
            {
                if (strcmp(usuario->arquivos[j].nome, arquivos_desejados[i]) == 0)
                {
                    ausente = false;
                    break;
                }
            }
            if (ausente)
            {
                strcpy(usuario->ausentes[usuario->num_arquivos].nome, arquivos_desejados[i]);
                usuario->num_arquivos++;

                pthread_mutex_lock(&mutex);
                printf("[%s] Arquivos Desejados -> %s\n", diretorio, usuario->ausentes->nome);
                pthread_mutex_unlock(&mutex);
            }
        }*/
}

/**
 *
 * Thread de Usuario
 *
 */
void *usuario_thread(void *arg)
{
    ThreadArgs *args = (ThreadArgs *)arg;
    Usuario *usuario = args->usuario;
    // int tmn_fragmento = args->tmn_fragmento;
    // int tmn_buffer = args->tmn_buffer;

    char *diretorio = usuario->nome;
    // pthread_t threads_arquivos[MAX_ARQUIVOS];
    //   Abre o diretório

    DIR *dir = opendir(diretorio);

    if (dir == NULL)
    {
        printf("Erro ao abrir o diretorio");
        free(usuario->nome); // Libera a memória alocada
        pthread_exit(NULL);
    }

    struct dirent *entry;

    // Lê cada entrada do diretório usando a função readdir
    while ((entry = readdir(dir)) != NULL)
    {
        // Se houver a entrada de um arquivo regular
        if (entry->d_type == DT_REG)
        {
            pthread_mutex_lock(&mutex); // Bloqueia o mutex antes de imprimir
            // Teste obs. Excluir depois
            //divide_fragmentos(tmn_fragmento, usuario->arquivos->nome);
            printf("[%s] Arquivos Existentes -> %s\n", usuario->nome, entry->d_name);
            strcpy(usuario->arquivos[usuario->num_arquivos].nome, entry->d_name);
            usuario->num_arquivos++;

            pthread_mutex_unlock(&mutex); // Desbloqueia o mutex após imprimir
        }
    }
    ausentes_diretorio(usuario);

    /*
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
    */

    // Diretorio fechado
    closedir(dir);
    free(diretorio);
    pthread_exit(NULL);
    return NULL;
}

/**
 *
 * Avalia quantidade de cada argumento de acordo com os limites pré-estabelecidos
 *
 */
int avaliacao_requisitos(int num_usuarios, int max_arquivos, int tmn_fragmento, int tmn_buffer)
{
    if (num_usuarios <= 0 || num_usuarios > MAX_USUARIOS)
    {
        printf("Quantidade de usuários invalida!\n");
        return 1;
    }
    if (max_arquivos <= 0 || max_arquivos > MAX_ARQUIVOS)
    {
        printf("Quantidade máxima de arquivos invalida!\n");
        return 1;
    }
    if (tmn_fragmento <= 0)
    {
        printf("tamanho dos fragmentos invalido!\n");
        return 1;
    }
    if (tmn_buffer <= 0)
    {
        printf("Tamanho do buffer invalido!\n");
        return 1;
    }
    return 0;
}

void arquivos_totais(int num_usuarios)
{
    int numero_total_arquivos = 0;

    for (int i = 0; i < num_usuarios; i++)
    {
        Usuario *usuario = &usuarios[i];
        for (int j = 0; j < usuario->num_arquivos; j++)
        {
            int encontrado = 0;
            for (int k = 0; k < MAX_ARQUIVOS; k++)
            {
                if (strcmp(usuario->arquivos[j].nome, total_arquivos[k].nome) == 0)
                {
                    encontrado = 1;
                    break;
                }
            }
            if (!encontrado)
            {
                pthread_mutex_lock(&mutex); // Bloqueia o mutex antes de imprimir
                strcpy(total_arquivos[numero_total_arquivos].nome, usuario->arquivos[j].nome);
                numero_total_arquivos++;

                /*Teste Ausentes*/
                printf("Arquivos Totais -> %s\n", total_arquivos[numero_total_arquivos].nome);

                pthread_mutex_unlock(&mutex); // Desbloqueia o mutex após imprimir
            }
        }
    }
}

int main(int argc, char *argv[])
{
    if (argc < 4)
    {
        printf("Quantidade de argumentos incorreta!\n");
        return 1;
    }
    int num_usuarios = atoi(argv[1]);
    int max_arquivos = atoi(argv[2]);
    int tmn_fragmento = atoi(argv[3]);
    int tmn_buffer = atoi(argv[4]);

    if (avaliacao_requisitos(num_usuarios, max_arquivos, tmn_fragmento, tmn_buffer) != 0)
    {
        return 1;
    }

    arquivos_totais(num_usuarios);

    srand(time(0)); // Inicializa a semente de números aleatórios
    pthread_t threads[num_usuarios];
    ThreadArgs threadArgs[num_usuarios];

    pthread_mutex_init(&mutex, NULL); // Mutex iniciado
    // Criação das threads dos usuários
    for (int i = 0; i < num_usuarios; i++)
    {
        sprintf(usuarios[i].nome, "U%d", i + 1);
        usuarios[i].num_arquivos = 0;
        usuarios[i].num_ausentes = 0;

        threadArgs[i].usuario = &usuarios[i];
        threadArgs[i].tmn_fragmento = tmn_fragmento;
        threadArgs[i].tmn_buffer = tmn_buffer;

        pthread_create(&threads[i], NULL, usuario_thread, &threadArgs[i]);
    }
    // Aguarda a finalização das threads dos usuários
    for (int i = 0; i < num_usuarios; i++)
    {
        pthread_join(threads[i], NULL);
    }

    pthread_mutex_destroy(&mutex); // Destroi o mutex

    return 0;
}
