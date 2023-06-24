
//-------------------- Diretivas de preprocessamento-----------------------------------------------
#define _DEFAULT_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h> // Essa library é parte da "posix library" que permite abir, ler conteúdo, etc de diretórios
#include "functions.h"
#include "definitions.h"
#include <sys/stat.h> // util para recuperar o tamanho, em bytes, de um arquivo
#include <unistd.h>
#include <time.h>

#define MAX_USUARIOS 5
#define MAX_ARQUIVOS 12
#define MAX_NOME_ARQUIVO 250

//-------------------- Definição de variáveis globais---------------------------------------------

Usuario usuarios[MAX_USUARIOS];       // vetor com as informações dos usuários
Arquivo total_arquivos[MAX_ARQUIVOS]; // ver
ListaDeArquivos listaGeralDeArquivos;
//Solicitacao solicitacoes[MAX_ARQUIVOS];


//==================== INICIO DO PROGRAMA ========================================================

int main(int argc, char *argv[])
{

    //-------------------- Teste de validade dos argumentos passados para o main-----------------------

    // printf("O valor do do argc eh %d \n", argc);

    if (argc != 5)
    {

        printf("Quantidade de argumentos inválida.\n");
        printf("Esta versão do programa requer argc=6 e mais 4 argumentos na execução.\n");
    }

    int num_usuarios = atoi(argv[1]);     // variável guarda o número máximo de usuarios
    int num_max_arquivos = atoi(argv[2]); // variavel guarda o numero maximo de arquivos
    int sizeof_fragmento = atoi(argv[3]); // variavel guarda o tamanho em bytes de um fragmento do buffer
    int sizeof_buffer = atoi(argv[4]);    // variavel guarda o tamanho em bytes de um buffer

    if (avaliacao_requisitos(num_usuarios, num_max_arquivos, sizeof_fragmento, sizeof_buffer) != 0)
    { // F001
        return 1;
    }

    /**
     *  Cada usuário será identificado pela letra U seguida da sua própria numeração: U1,N2,U3,...Unum_usuarios.
     *  De acordo com o valor informado para "num_usuarios" informado, existirá uma quantidade equivalente de pastas
     *  nomeadas com o nome do usuario a que pertence.
     *  Nestas pastas serão colocados os arquivos que cada usuário possui.
     *  Existe um número finito de arquivos toatais e ao final da simulação todos os usuários devem possuir todos os arquivos.
     */

    //-------------------- Leitura das informações de cada usuário-------------------------------------

    define_usuarios(num_usuarios, usuarios, num_max_arquivos);

    //-------------------- Relatorio do estado inicial dos usuários-------------------------------------

    report_initial_status(num_usuarios, usuarios);

    srand(time(0)); // Inicializa a semente de números aleatórios

    //------------------------- Criação das Threads de Usuarios -----------------------------------------

    thread_user_create(usuarios, sizeof_fragmento, sizeof_buffer, num_usuarios);


    return (0);
}