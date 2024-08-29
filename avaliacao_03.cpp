#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#define N 5

typedef struct Data{
    int dia;
    int mes;
    int ano;
} date;

typedef struct Medicamentos {
    int codigo_barra;
    char nome_medicamento[20];
    date data_cadastro;
    int quantidade;
    float valor;
} medicamentos;

typedef struct page { // Uma pagina é um vetor em si
    int qtd;
    bool folha;
    int referencia[N - 1]; // Nos de referencia
    struct page *pai;
    medicamentos *medicine[N - 1];
    struct page *filhos[N];
    struct page *prox;
} pagina; 

// Os nós devem ser inseridos apenas em folhas
// Nós superiores servem apenas como ponteiros
// Os nós formam um caminho sequencial
// Em vetor com no maximo 4 posições(ordem 5), onde no meio de cada vetor se possui um ponteiro ligando se a ele
// Os vetores vão formar uma especie de lista encadeada horizontal, sempre de forma crescente

// Criando a pagina
pagina *create_page(){
    pagina *nova_pagina = (pagina *)malloc(sizeof(pagina));

    if(nova_pagina == NULL) return NULL;

    // Inicializando vazio os filhos
    for(int i = 0; i < 5; i++){
        nova_pagina->filhos[i] = NULL;
    }

    // Inicializando vazio os medicamentos
    for(int i = 0; i < 4; i++){
        nova_pagina->medicine[i] = NULL;
    }

    nova_pagina->qtd = 0;
    nova_pagina->folha = false;
    nova_pagina->pai = NULL;
    nova_pagina->prox = NULL;

    return nova_pagina;
}

medicamentos *info_medicamentos(medicamentos *novo){

    printf("Codigo de barras: ");
    scanf("%d", &novo->codigo_barra);

    printf("Nome do medicamento: ");
    scanf("%s", novo->nome_medicamento);

    printf("Quantidade: ");
    scanf("%d", &novo->quantidade);

    printf("Valor do medicamento: ");
    scanf("%f", &novo->valor);

    printf("Data de cadastro (dd mm aaaa): ");
    scanf("%d %d %d", &novo->data_cadastro.dia, &novo->data_cadastro.mes, &novo->data_cadastro.ano);

    return novo;
}

// Segmentation pode ser aqui
pagina *quebra_vetor(pagina *page){
    int meio = N / 2;

    pagina *nova_pagina = create_page();
    nova_pagina->folha = page->folha;

    if(page->folha == true){
        int i;
        for(i = meio; i < N - 1; i++){
            nova_pagina->medicine[i - meio] = page->medicine[i]; // Transferindo metade dos filhos de um vetor ao novo vetor
            nova_pagina->referencia[i - meio] = page->referencia[i];
            page->medicine[i] = NULL;
            page->referencia[i] = 0;
        }
        page->qtd = meio;
        nova_pagina->qtd = N - 1 - meio;

        nova_pagina->prox = page->prox;
        page->prox = nova_pagina;
    } else { // Caso não seja folha

    }
    
    // Atualizar dados do ponteiro
    if(page->pai == NULL){
        // Na raiz
        pagina *nova_raiz = create_page();
        nova_raiz->referencia[0] = nova_pagina->referencia[0];
        nova_raiz->qtd++;
        nova_raiz->filhos[0] = page;
        nova_raiz->filhos[1] = nova_pagina;
        nova_pagina->pai = nova_raiz;
        page->pai = nova_raiz;

        return nova_raiz;
    } else {
        // Já tem raiz (pai)

    }
}

// Em cada filho, no caso 5, se possui um vetor, ou seja, 5 vetores (um vetor indicando a existencia de 5 vetores)
void inserir_medicamento(pagina *nova_pagina, medicamentos *novo){
    int i = nova_pagina->qtd - 1;

    if(nova_pagina->folha == true){
        while(i >= 0 && nova_pagina->referencia[i] > novo->codigo_barra){
            nova_pagina->medicine[i + 1] = nova_pagina->medicine[i]; // Passa a ser o posterior (como se fosse o anterior)
            nova_pagina->referencia[i + 1] = nova_pagina->referencia[i]; // Mesma coisa
            i--; // Reduzo um para realizar novamente a comparação
        }
        i++; // Volto i ao seu estado normal
        // Atualizo valores e realizo a inserção
        nova_pagina->medicine[i] = novo; 
        nova_pagina->referencia[i] = novo->codigo_barra;
        nova_pagina->qtd++;

    } else {
        while(i >= 0 && nova_pagina->referencia[i] > novo->codigo_barra){
            i--;
        }
        i++;

        if(nova_pagina->filhos[i]->qtd == N - 1){
            nova_pagina->filhos[i] = quebra_vetor(nova_pagina->filhos[i]);
            if(nova_pagina->referencia[i] < novo->codigo_barra){
                i++;
            }
        }
        inserir_medicamento(nova_pagina->filhos[i], novo);
    }
} 

// Erro ao inserir segundo elemento, Segmentation Fault
void inserir_raiz(pagina **raiz){
    medicamentos *novo = (medicamentos *)malloc(sizeof(medicamentos));
    info_medicamentos(novo);

    if((*raiz) == NULL){
        (*raiz) = create_page();
        (*raiz)->medicine[0] = novo;
        (*raiz)->folha = true;
        (*raiz)->referencia[0] = novo->codigo_barra;
        (*raiz)->qtd++;
    } else {
        if((*raiz)->qtd == N - 1 && (*raiz)->folha == true){
            *raiz = quebra_vetor(*raiz);
        }
        inserir_medicamento((*raiz), novo);
    }
}

// Função recursiva para imprimir a árvore B+
void imprimir_arvore(pagina *node, int nivel) {
    if (node != NULL) {
        // Imprime os valores e os filhos
        for (int i = 0; i < node->qtd; i++) {
            if (!node->folha) {
                imprimir_arvore(node->filhos[i], nivel + 1);
            }
            // Imprime o código do medicamento (ou qualquer outra informação desejada)
            printf("%*sCodigo: %d\n", nivel * 4, "", node->medicine[i]->codigo_barra);
        }

        // Imprime o último filho (se houver)
        if (!node->folha) {
            imprimir_arvore(node->filhos[node->qtd], nivel + 1);
        }
    }
}

long buscar_codigo(pagina *node, int codigo){

}

void menu(){
    printf("##########################################\n");
    printf("################## MENU ##################\n");
    printf("# 1 - Inserir Elemento                   #\n");
    printf("# 2 - Imprimir Arvore                    #\n");
    printf("# 3 - Buscar Elemento                    #\n");
    printf("# 4 - Remover                            #\n");
    printf("# 0 - Sair                               #\n");
    printf("##########################################\n");
    printf("Selecione: ");
}

int main(){

    pagina *raiz = NULL;

    int op;

    int num, toRemove;

    do{
        menu();
        scanf("%d", &op);
        system("cls || clear");

        switch(op){
            case 1: 
                inserir_raiz(&raiz); // Funcionando
                break;
            case 2:
                imprimir_arvore(raiz, 0);
                break;
            case 3:
                printf("Numero a se buscar: "); // Funcionando
                scanf("%d", &num);
                buscar_codigo(raiz, num);
                break;
            case 4:
                printf("Elemento a remover: "); // Funcionando
                scanf("%d", &toRemove);
                //node = remover_no(node, toRemove);
                break;
            case 0:
                printf("Closing...\n");
                break;
            default:
                printf("Invalid option!\n");
                break;
        }

    } while (op != 0);

    return 0;
}