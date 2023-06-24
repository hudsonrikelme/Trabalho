#define _DEFAULT_SOURCE
#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include "functions.h"
#include "definitions.h"
#include <sys/stat.h> // util para recuperar o tamanho, em bytes, de um arquivo
#include <unistd.h>
#include <pthread.h>

//-------------------- Definição de variáveis globais---------------------------------------------

extern Usuario usuarios[MAX_USUARIOS];       // vetor com as informações dos usuários
extern Arquivo total_arquivos[MAX_ARQUIVOS]; // ver
extern ListaDeArquivos listaGeralDeArquivos;

extern Solicitacao solicitacoes[MAX_ARQUIVOS]; // Um Usuário realiza a solicitação de 0 ou vários Arquivos
pthread_mutex_t mutex;
ListNode *nodoZero; // declaração da lista de solicitações do alg

ListNode *listHead = NULL;        // inicializa o nodoZero da lista de solicitações
ListNode *listHeadReverse = NULL; // inicializa o nodoZero da lista de solicitações invertida
//--------------------------------------------------------------------------------------------------------------
//      F001-Função que avalia a validade dos argumentos fornecido ao main
//--------------------------------------------------------------------------------------------------------------
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

    printf("========================Parametros da simulação=========================\n\n");

    printf("O numero de usuarios eh: %d. \n", num_usuarios);
    printf("O numero maximo de arquivos eh %d. \n", max_arquivos);
    printf("O tamanho de um fragmento. eh %d \n", tmn_fragmento);
    printf("O tamanho do buffer eh %d. \n", tmn_buffer);

    return 0;
}

//--------------------------------------------------------------------------------------------------------------
//      F002-Função que avalia a validade dos argumentos fornecido ao main
//--------------------------------------------------------------------------------------------------------------

void define_usuarios(int num_usuarios, Usuario *usuarios, int num_max_arquivos)
{

    // ler e armazenar a quantidade de arquivos presentes e ausentes para cada usuário
    // ler e armazenar os nomes dos arquivos presentes e ausentes para cada usuário
    // ler e armazenar os tamanhos dos arquivos (Falta)
    // calcular através do tamanho do arquivo a quantidade de fragmentos de buffer necessários para copiá-lo (falta)

    for (int i = 0; i < num_usuarios; i++)
    {

        usuarios[i].num_arquivos = 0; // inicializa o contador de arquivos do usuário i.

        //----------Definição do nome e abertura do diretório do usuário---------------------

        sprintf(usuarios[i].nome, "U%d", i + 1);
        // printf("o nome do usuario %s \n",usuarios[i].nome);

        char *nomeDoDiretorioDoUsuario = usuarios[i].nome;

        DIR *directory;                                // directory aponta para uma stream de diretorio: permite abrir um diretorio, ler arquivos e subdiretorios, fechar o diretorio
        struct dirent *entry;                          // entry aponta para uma struct que representa os elementos dentro de um diretorio (arquivos ou subdiretorios)
        directory = opendir(nomeDoDiretorioDoUsuario); // abre o diretório atual e o atribui à stream directory. a função opendir retorna uma stream de diretorio

        if (directory == NULL)
        {
            printf("Erro ao tentar abrir o diretorio.\n");
            // return 1;
        }

        //----------Inserção dos arquivos encontrados na lista de arquivos de um usuario---------------------

        while ((entry = readdir(directory)) != NULL)
        { // while valido enquanto houver elementos para serem "abertos na stream de diretorio"

            // printf("%s \n",entry->d_name);          // d_name é um membro da struct entry (um ponteiro) que retorna o nome do elemento seja ele arquico ou diretorio

            if (entry->d_type == DT_REG)
            { // Se a entry for um arquivo regular

                // copie para o membro"nome" do usuario na posição i do vetor usuarios o nome do arquivo encontrado no diretorio
                strcpy(usuarios[i].arquivos[usuarios[i].num_arquivos].nome, entry->d_name);
                usuarios[i].num_arquivos += 1; // conta quantos arquivos foram encontrados para o usuario i
            }
        }
        if (usuarios[i].num_arquivos >= num_max_arquivos)
        {
            perror("Quantidade Maxima de arquivos em cada diretorio ultrapassada!");
            exit(EXIT_FAILURE);
        }

        if (closedir(directory) == -1)
        { // a função closedir retorna -1 quando nao consegue fechar um diretorio

            printf("Erro ao tentar fechar o diretorio.\n");
            // return 1;
        }
    }

    func_fileSize(num_usuarios, usuarios);
    func_ListaGeral(num_usuarios); // Cria alista geral de arquivos
    func_ArquivosAusentes(num_usuarios, usuarios);
}

//--------------------------------------------------------------------------------------------------------------
//      F003-Função que cria a lista geral de arquivos da simulação
//--------------------------------------------------------------------------------------------------------------

void func_ListaGeral(int num_usuarios)
{

    listaGeralDeArquivos.counter = 0; // inicializa o contador geral de arquivos na lista que armazena as infos dos arquivos

    for (int z = 0; z < num_usuarios; z++)
    { // loop sobre os usuarios

        for (int k = 0; k < usuarios[z].num_arquivos; k++)
        { // loop sobre os arquivos dos usuarios

            // copie para o membro"nome" do arquivo na posição counter do vetor listaGeralDeArquivos o nome do arquivo encontrado no diretorio

            int teste = 0; // guarda o resultado do teste para saber se um arquivo que será adicionado já existe na lista geral

            for (int w = 0; w < listaGeralDeArquivos.counter; w++)
            {

                if (strcmp(usuarios[z].arquivos[k].nome, listaGeralDeArquivos.itens[w].nome) == 0)
                {
                    teste = 1;
                }
            }

            if (teste == 0)
            {
                // copia para a lista geral o nome o arquivo
                strcpy(listaGeralDeArquivos.itens[listaGeralDeArquivos.counter].nome, usuarios[z].arquivos[k].nome);
                listaGeralDeArquivos.itens[listaGeralDeArquivos.counter].tamanho = usuarios[z].arquivos[k].tamanho;

                listaGeralDeArquivos.counter += 1; // incrementa o contador de arquivos da lista geral
            }
        }
    }
}

//--------------------------------------------------------------------------------------------------------------
//      F004-Função que, para cada usuario, cria a lista de arquivos ausentes
//--------------------------------------------------------------------------------------------------------------

void func_ArquivosAusentes(int num_usuarios, Usuario *usuarios)
{

    for (int z = 0; z < num_usuarios; z++)
    {

        usuarios[z].num_ausentes = 0;

        for (int w = 0; w < listaGeralDeArquivos.counter; w++)
        {

            int teste = 0;

            for (int k = 0; k < usuarios[z].num_arquivos; k++)
            {

                if (strcmp(usuarios[z].arquivos[k].nome, listaGeralDeArquivos.itens[w].nome) == 0)
                {
                    teste = 1;
                }
            }

            if (teste == 0)
            {

                // copia para a lista de arquivos ausentes do usuario Z
                strcpy(usuarios[z].ausentes[usuarios[z].num_ausentes].nome, listaGeralDeArquivos.itens[w].nome);
                // Copia o tamanho do arquivo para ausentes
                usuarios[z].ausentes[usuarios[z].num_ausentes].tamanho = listaGeralDeArquivos.itens[w].tamanho;
                usuarios[z].num_ausentes += 1; // incrementa o contador de arquivos ausents do usuario Z
            }
        }
    }
}

//--------------------------------------------------------------------------------------------------------------
//      F004-Função que determina o tamanho dos arquivos dos usuarios
//--------------------------------------------------------------------------------------------------------------

void func_fileSize(int num_usuarios, Usuario *usuarios)
{

    // char s[100];

    for (int i = 0; i < num_usuarios; i++)
    {

        // printf("DIRETORIO ATUAL: %s\n", getcwd(s, 100));

        if (chdir(usuarios[i].nome) != 0)
        {
            printf("chdir() to /%s failed", usuarios[i].nome);
        }
        // printf("DIRETORIO ATUAL: %s\n", getcwd(s, 100));

        for (int j = 0; j < usuarios[i].num_arquivos; j++)
        {

            FILE *fp = fopen(usuarios[i].arquivos[j].nome, "r");

            if (fp == NULL)
            {
                printf("Cannot open source file.\n");
                exit(1);
            }

            struct stat st;
            int fd = fileno(fp); // get file descriptor
            fstat(fd, &st);
            off_t size = st.st_size;

            usuarios[i].arquivos[j].tamanho = size;

            // printf("tamanho do arquivo %s eh %ld.\n",usuarios[i].arquivos[j].nome, size);
        }

        if (chdir("..") != 0)
            perror("chdir() to /usr failed");
        // printf("DIRETORIO ATUAL: %s\n\n", getcwd(s, 100));
    }
}

//--------------------------------------------------------------------------------------------------------------
//      F006-Função que cria um relatorio do estado inicial dos usuarios
//--------------------------------------------------------------------------------------------------------------

void report_initial_status(int num_usuarios, Usuario *usuarios)
{

    printf("\n================Relatorio da condição inicial dos usuarios==============");

    for (int z = 0; z < num_usuarios; z++)
    {

        printf("\n\n >>>O usuario %d tem %d arquivos \n", z + 1, usuarios[z].num_arquivos);

        for (int k = 0; k < usuarios[z].num_arquivos; k++)
        {

            printf("O nome do arquivo %d do U%d eh %s \n", k + 1, z + 1, usuarios[z].arquivos[k].nome);
        }
    }

    printf("\n--------------Lista geral de arquivos---------------------------------\n\n");

    printf("Existem %d arquivos distintos na lista geral\n\n", listaGeralDeArquivos.counter);

    for (int k = 0; k < listaGeralDeArquivos.counter; k++)
    {

        printf("O nome e tamanho do arquivo %d da lista geral sao %s eh %ld bytes\n", k + 1, listaGeralDeArquivos.itens[k].nome, listaGeralDeArquivos.itens[k].tamanho);
    }

    printf("\n--------------Lista de arquivos ausentes---------------------------------");

    for (int z = 0; z < num_usuarios; z++)
    {

        printf("\n\n >>>O usuario %d tem %d arquivos ausentes \n\n", z + 1, usuarios[z].num_ausentes);

        for (int k = 0; k < usuarios[z].num_ausentes; k++)
        {

            printf("Para o usuario U%d falta o arquivo %s \n", z + 1, usuarios[z].ausentes[k].nome);
        }
    }
}

//--------------------------------------------------------------------------------------------------------------
//      F007-Função que invoca a criação da thread de usuario
//--------------------------------------------------------------------------------------------------------------

void thread_user_create(Usuario *usuarios, int sizeof_fragmento, int sizeof_buffer, int num_usuarios)
{

    pthread_t threads[num_usuarios];
    pthread_mutex_init(&mutex, NULL); // Mutex iniciado
    ThreadArgs threadArgs[num_usuarios];

    for (int i = 0; i < num_usuarios; i++)
    {
        threadArgs[i].usuario = &usuarios[i];
        threadArgs[i].sizeof_fragmento = sizeof_fragmento;
        threadArgs[i].sizeof_buffer = sizeof_buffer;
        pthread_create(&threads[i], NULL, user_thread, &threadArgs[i]);
    }
    // Impressão da lista encadeada OBSSERVAR QUE AS SOLICITAÇÕES ESTÃO INDO DO FINAL DO ARQUIVO PARA O INICIO
    //printList(listHead);
    for (int i = 0; i < num_usuarios; i++)
    {
        pthread_join(threads[i], NULL);
    }
}

//--------------------------------------------------------------------------------------------------------------
//      F008-Função que está dentro da Thread de Usuário e Realiza a Solicitação e Fornecimento de Arquivos
//--------------------------------------------------------------------------------------------------------------

void *user_thread(void *arg)
{
    ThreadArgs *args = (ThreadArgs *)arg;
    Usuario *usuario = args->usuario;
    int sizeof_fragmento = args->sizeof_fragmento;
    int sizeof_buffer = args->sizeof_buffer;
    // Thread de Arquivos para solicitação de Arquivos
    pthread_t file_Request_threads[usuario->num_ausentes];
    ThreadUserArgs threadUserArgs[usuario->num_ausentes];

    //-------------------- Usuario Realiza a solicitacao de Arquivos-------------------------------------
    int num_ausentes = usuario->num_ausentes;
    int counter = 0;

    int* ausentes_indices = (int*)malloc(num_ausentes * sizeof(int));
    for(int i = 0; i < num_ausentes; i++){
        ausentes_indices[i] = i;
    }
    shuffleArray(ausentes_indices, num_ausentes);

    while (num_ausentes > 0)
    {
        int num_solicitacao = rand() % (num_ausentes + 1);
        if (num_solicitacao > num_ausentes)
        {
            printf("Numero de Solicitações é maior que o de ausentes");
            num_solicitacao = num_ausentes;
        }
        pthread_mutex_lock(&mutex);
        //>>>>>>>>>>>>>>>>>Teste Quantidade de Arquivos Solicitados por cada usuario
        printf(">>>> %d Arquivos solicitados pelo usuario [%s]\n", num_solicitacao, usuario->nome);
        // pthread_mutex_unlock(&mutex);
        for (int i = 0; i < num_solicitacao; i++)
        {
            inicializaSolicitacao(&threadUserArgs[i]);

            threadUserArgs[i].solicitacao.nomeDoArquivo = usuario->ausentes[counter].nome;
            threadUserArgs[i].solicitacao.nomeDoSolicitante = usuario->nome;
            threadUserArgs[i].tamanhoArquivo = usuario->ausentes[counter].tamanho;
            threadUserArgs[i].sizeof_buffer = sizeof_buffer;
            pthread_create(&file_Request_threads[i], NULL, func_User_Request_Thread, &threadUserArgs[i]);
            counter++;
        }
        pthread_mutex_unlock(&mutex);
        num_ausentes = num_ausentes - num_solicitacao;
    }
    for(int i = 0; i < num_ausentes;i++){
            pthread_join(file_Request_threads[i], NULL);
        }
        free(ausentes_indices);
    return NULL;
}
//--------------------------------------------------------------------------------------------------------------
//      F009-Função para embaralhar um array de inteiros usando o algoritmo de Fisher-Yates
//--------------------------------------------------------------------------------------------------------------


void shuffleArray(int* array, int size) {

    for (int i = size - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        int temp = array[i];
        array[i] = array[j];
        array[j] = temp;
    }
}

//--------------------------------------------------------------------------------------------------------------
//      F009-Função chamada para Solicitar os fragmentos de um arquivo
//--------------------------------------------------------------------------------------------------------------

void *func_User_Request_Thread(void *arg)
{
    ThreadUserArgs *args = (ThreadUserArgs *)arg;
    Solicitacao solicitacao = args->solicitacao;
    int tamanhoArquivo = args->tamanhoArquivo;
    int sizeof_Buffer = args->sizeof_buffer;
    int frag = (tamanhoArquivo / sizeof_Buffer) + (tamanhoArquivo % sizeof_Buffer != 0);
    int counter_IniByte = 0;
    int counter_FinalByte = sizeof_Buffer;

    //pthread_mutex_lock(&mutex);
    //printf("Usuario %s solicita o arquivo: %s, de tamanho: %d bytes\n", solicitacao.nomeDoSolicitante, solicitacao.nomeDoArquivo, tamanhoArquivo);
    //printf("Tamanho do Buffer: %d, com %d fragmentos\n", sizeof_Buffer, frag);
    //pthread_mutex_unlock(&mutex);
    for (int i = 0; i < frag; i++)
    {
        solicitacao.iniByte = counter_IniByte;
        solicitacao.finalbyte = counter_FinalByte;
        pthread_mutex_lock(&mutex);
        printf("[%s] Solicitacao do fragmento (%d - %d) do Arquivo: %s\n", solicitacao.nomeDoSolicitante, solicitacao.iniByte, solicitacao.finalbyte, solicitacao.nomeDoArquivo);
        

        inserirNodo(&listHead, solicitacao);
        pthread_mutex_unlock(&mutex);
        counter_IniByte = solicitacao.finalbyte;
        counter_FinalByte = counter_FinalByte + sizeof_Buffer;
    }
    

    // INVERSÃO DA LISTA; UMA NOVA LISTA "listHeadReverse" É CRIADA COM A SEQUENCIA DAS SOLICITAÇÕES NA ORDEM CORRETA
    //ReverseList(listHead, listHeadReverse);

    return NULL;
}

//--------------------------------------------------------------------------------------------------------------
//      F010-Função que inicializa a struct Solicitação
//--------------------------------------------------------------------------------------------------------------

void inicializaSolicitacao(void *arg)
{
    ThreadUserArgs *args = (ThreadUserArgs *)arg;
    Solicitacao solicitacao = args->solicitacao;

    solicitacao.nomeDoSolicitante = "dummy";
    solicitacao.nomeDoArquivo = "dummy";
    solicitacao.iniByte = 0;
    solicitacao.finalbyte = 0;
    solicitacao.statusDaSolicitacao = 0;
    solicitacao.nomeDoServidor = "dummy";
}

//--------------------------------------------------------------------------------------------------------------
//      F011-Função relacionada à lista encadeada de solicitações
//--------------------------------------------------------------------------------------------------------------

//---------------------Função para inserir um elemento no início da lista--------------------------------------

void inserirNodo(ListNode **N, Solicitacao solicitacao)
{

    // Argumentos da função são:
    //   N.......do tipo ponteiro para ListNode: Representa o novo nó da lista.
    //   solicitacao....do tipo Solicitação: carrega a solicitação que deve ser armanezada no nó N.

    ListNode *newNode = (ListNode *)malloc(sizeof(ListNode)); // Cria um novo ponteiro para um espaço de memoria equivalente a um ListNode

    newNode->slct = solicitacao; // ao membro solicitacao do endereço apontado por newNode é atribuido o valor da solicitação slct passada nos argumentos da função
    newNode->next = listHead;    // o ponteiro do nó é apontado para a atual cabeça da lista
    *N = newNode;                // o espaço de memoria N assume o papel de nova cabeça de lista.

    listHead = newNode;
    // return newNode;
}
//-------------------Função para percorrer e imprimir os elementos da lista----------------------------------

void printList(ListNode *head)
{

    ListNode *current = head;
    pthread_mutex_lock(&mutex);
    printf("\n\n Solicitações da lista encadeada \n\n");

    while (current != NULL)
    {

        printf(" Solicitação do cliente %s.\n", current->slct.nomeDoSolicitante);
        printf(" Fragamentos de bytes solicitados: do %d ao %d.\n", current->slct.iniByte, current->slct.finalbyte);
        printf(" Arquivo de origem: %s.\n\n", current->slct.nomeDoArquivo);

        current = current->next;
    }
    printf("\n");
    pthread_mutex_unlock(&mutex);
}