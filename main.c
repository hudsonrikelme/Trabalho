#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>

#define MAX_USUARIOS 5
#define MAX_ARQUIVOS 12

void criarDiretoriosUsuarios();
void distribuiArquivos();

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("Informe a quantidade de usuarios como argumento!\n");
        return 1;
    }

    int usuarios = atoi(argv[1]);
    int max_arquivos = atoi(argv[2]);

    if (usuarios <= 0 || usuarios > MAX_USUARIOS)
    {
        printf("Quantidade de usuarios invalida!\n");
        return 1;
    }
    if (max_arquivos <= 0 || max_arquivos > MAX_ARQUIVOS)
    {
        printf("Quantidade de arquivos invalida!\n");
        return 1;
    }

    criarDiretoriosUsuarios(usuarios);
    distribuiArquivos(usuarios);

    return 0;
}

void criarDiretoriosUsuarios(int usuarios)
{
    char primeiraparte[] = "U";
    char nome[10];

    for (int c = 1; c <= usuarios; c++)
    {
        // armazena no buffer o nome do arquivo
        sprintf(nome, "%s%d", primeiraparte, c);
        char comando[50];
        sprintf(comando, "mkdir %s", nome);
        system(comando);
    }
}

void distribuiArquivos(int usuarios)
{
    // representa uma entrada no diretório
    struct dirent *entry;
    char userDiretorio[10];
    // Inicializa a semente do gerador de números aleatórios
    //garante que sempre serão diferentes em cada inicialização
    srand(time(NULL)); 

    // Abre o diretório ArquivosBase através de um ponteiro, onde estão os arquivos.
    DIR *dir = opendir("ArquivosBase");
    // verifica se foi aberto corretamente
    if (dir == NULL)
    {
        perror("Erro ao abrir o diretorio 'ArquivosBase'");
        exit(1);
    }

    // Lê os arquivos do diretório "ArquivosBase", através da funcão readdir
    while ((entry = readdir(dir)) != NULL)
    {
        // verifica se é um arquivo regular
        if (entry->d_type == DT_REG)
        {
            char nomeArquivo[100];
            sprintf(nomeArquivo, "ArquivosBase/%s", entry->d_name);

            // Número de usuário aleatório para colocar o arquivo
            int random = (rand() % usuarios) + 1;
            // Nome do Diretório destino com base no usuário aleatório
            sprintf(userDiretorio, "U%d", random);

            char arquivoDestino[100];
            sprintf(arquivoDestino, "%s/%s", userDiretorio, entry->d_name);

            FILE *src = fopen(nomeArquivo, "rb");
            FILE *dest = fopen(arquivoDestino, "wb");

            if (src == NULL || dest == NULL)
            {
                perror("Erro ao copiar o arquivo");
                exit(1);
            }

            unsigned char buffer[1024];
            size_t lerBytes;
            while ((lerBytes = fread(buffer, sizeof(unsigned char), sizeof(buffer), src)) > 0)
            {
                size_t escBytes = fwrite(buffer, sizeof(unsigned char), lerBytes, dest);
                if (escBytes != lerBytes)
                {
                    perror("Erro ao copiar o arquivo");
                    exit(1);
                }
            }

            fclose(src);
            fclose(dest);
        }
    }
    closedir(dir);
}