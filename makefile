# definindo as variáveis do projeto (MAIN é o nome do arquivo principal, que contém a função main, sem a extensão)
MAIN := main

# objetos a serem gerados na compilação
OBJECTS := $(MAIN).o functions.o

# Flags de compilação para C
FLAGS := -Wall -Wextra -std=c11 -pthread

# necessário apenas quando se incluir a biblioteca <math.h> em algum arquivo fonte no projeto
MATH := -lm

# definição do compilador: para C, use gcc
CC := gcc

# ajustando alguns parâmetros/comandos ao sistema operacional
ifeq ($(OS), Windows_NT)
	OUTPUTMAIN := $(MAIN).exe
	RM := del /q /f
else
	OUTPUTMAIN := $(MAIN).out
	RM := rm -f
endif

# ponto de compilação principal
all: $(OUTPUTMAIN)
	@echo Compiling 'all' complete!

# gerando o arquivo executável
$(OUTPUTMAIN): $(OBJECTS)
	$(CC) $(FLAGS) $(OBJECTS) -o $(OUTPUTMAIN) $(MATH)

# gerando o arquivo objeto da função principal... adicionar as dependências (se houverem)
$(MAIN).o: $(MAIN).c definitions.h functions.h
	$(CC) $(FLAGS) -c $(MAIN).c

# gerando o arquivo objeto da função functions
functions.o: functions.c functions.h
	$(CC) $(FLAGS) -c functions.c

clean:
	$(RM) $(OBJECTS)
	$(RM) $(OUTPUTMAIN)
	$(RM) *.dat
	@echo Cleanup complete!!!

run: all
	./$(OUTPUTMAIN) $(ARGS)
	@echo Executing 'all' complete!!!
