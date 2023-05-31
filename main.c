#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#define MAX_USUARIOS 5

void criarDiretoriosUsuarios(int quantidade){
    char primeiraparte[] = "U";
    char nome[10];

    for (int c = 1; c < quantidade; c++){
        //armazena no buffer o nome do arquivo
        sprintf(nome, "%s%d", primeiraparte, c);
        char comando[50];
        sprintf(comando, "mkdir %s", nome);
        system(comando);
    }
    



}


int main(int argc, char *argv[]){
    if (argc < 2) {
        printf("Informe a quantidade de usuários como argumento!\n");
        return 1;
    }

    int quantidade = atoi(argv[1]);
    if(quantidade <= 0 || quantidade > MAX_USUARIOS){
        printf("Quantidade de usuários inválida!\n");
        return 1;
    }
    criarDiretoriosUsuarios(quantidade);
   
}