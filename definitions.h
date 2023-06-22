
//-------------------- Diretivas de preprocessamento-----------------------------------------------
#ifndef DEFINITIONS
#define DEFINITIONS

#define MAX_USUARIOS 5
#define MAX_ARQUIVOS 12
#define MAX_NOME_ARQUIVO 250



//-------------------- Definição dos Structs do projeto--------------------------------------------


typedef struct{
    char nome[MAX_NOME_ARQUIVO];        // Vetor guarda o nome do arquivo
    int fragmentos[1000];
    long int tamanho;                   // inteiro longo que guarda o tamanho do arquivo   
    int numeroDeFragmentos;             // inteiro que guarda o numero de fragmentos, de acordo com o tamanho do buffer
} Arquivo;



typedef struct{

    Arquivo itens[MAX_ARQUIVOS];
    int counter;                      //inicializa o contador de entradas 

} ListaDeArquivos;



typedef struct{
    
    char nome[10];                      // nome do usuário
    int num_arquivos;                   // numero de arquivos do usuario
    int num_ausentes;                   // numero de arquivos ausentes
    Arquivo arquivos[MAX_ARQUIVOS];     // vetor com as informações dos arquivos pertencentes ao usuário
    Arquivo ausentes[MAX_ARQUIVOS];     // vetor com a lista dos arquivos ausentes

} Usuario;

typedef struct
{
    Arquivo arquivo;
    Usuario *usuarioCliente;
    int iniByte;
    int finalByte;
    Usuario usuarioServidor;
    int statusDaSolicitacao;
} Solicitacao;

typedef struct
{
    Usuario * usuario;
    int sizeof_fragmento;
    int sizeof_buffer;
}ThreadArgs;

typedef struct
{
    Solicitacao solicitacao;
}ThreadUserArgs;









#endif