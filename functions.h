#ifndef FUNCTIONS
#define FUNCTIONS

#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include "definitions.h"

//-------------------- Definição de variáveis globais---------------------------------------------

extern Usuario usuarios[MAX_USUARIOS];       // vetor com as informações dos usuários
extern Arquivo total_arquivos[MAX_ARQUIVOS]; // ver
extern Solicitacao solicitacoes[MAX_ARQUIVOS];

int avaliacao_requisitos(int num_usuarios, int max_arquivos, int tmn_fragmento, int tmn_buffer);

void define_usuarios(int num_usuarios, Usuario *usuarios, int num_max_arquivos);

void adicionaArquivoNaListaGeral(int pos, char *nomeDoArquivo);

void func_ListaGeral(int num_usuarios);

void func_ArquivosAusentes(int num_usuarios, Usuario *usuarios);

void report_initial_status(int num_usuarios, Usuario *usuarios);

void func_ArquivosAusentes(int num_usuarios, Usuario *usuarios);

void func_fileSize(int num_usuarios, Usuario *usuarios);

void thread_user_create(Usuario *usuarios, int sizeof_fragmento, int sizeof_buffer, int num_usuarios);

void shuffleArray(int* array, int size);

void *user_thread(void *arg);

void *func_User_Request_Thread(void *arg);

void inicializaSolicitacao(void *arg);

void inserirNodo(ListNode** N, Solicitacao solicitacao);

void printList(ListNode *head);

#endif