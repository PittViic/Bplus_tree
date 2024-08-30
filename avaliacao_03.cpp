// Created By Pedro Victor - @pitt_viic - https://github.com/PittViic?tab=overview&from=2024-08-01&to=2024-08-29

/*
1. Suponha que uma grande rede de farmácias precisa organizar os dados sobre os
medicamentos em estoque armazenados em todas as suas filiais. Para isso foi decidido
construir um sistema para unificar todo seu estoque em uma estrutura de Árvore B+ de ordem
5, portanto implemente os seguintes requisitos abaixo:
    a. Dados a serem armazenados em cada registro de medicamento
    i. Código de barras (chave)
    ii. Nome do medicamento
    iii. Data do cadastro
    iv. Quantidade
    v. Preço
b. O sistema deve realizar a busca de medicamentos pelo código de barras e exibir
suas informações.

c. A inserção de novos medicamentos deve obedecer as regras das Árvores B+ e o
acréscimo de produtos já cadastrados deve apenas incrementar sua quantidade.

d. O procedimento de retirada de medicamentos deve solicitar a quantidade, e sempre
que ela chegar a zero, o registro deve ser removido da Árvore B+.

e. Imprimir uma relação com todos os produtos ordenados pelo código de barras.

typedef struct page {
    int qnt = 0;
    bool folha = false;
    int codigos[N - 1];
    struct page* pai = NULL;
    medicamento* medicamentos[N - 1];
    struct page* filhos[N];
    struct page* prox = NULL;
} pagina;
*/

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
    bool leaf;
    int referencia[N - 1]; // Nos de referencia
    struct page *father;
    medicamentos *medicine[N - 1];
    struct page *filhos[N];
    struct page *next;
} pagina; 

// Os nós devem ser inseridos apenas em folhas
// Nós superiores servem apenas como ponteiros
// Os nós formam um caminho sequencial
// Em vetor com no maximo 4 posições(ordem 5), onde no meio de cada vetor se possui um ponteiro ligando se a ele
// Os vetores vão formar uma especie de lista encadeada horizontal, sempre de forma crescente

medicamentos *search_medicine(pagina *root, int element);

// Criando a pagina
pagina *create_page(){
    pagina *new_element = (pagina *)malloc(sizeof(pagina));

    if(new_element == NULL) return NULL;

    // Inicializando vazio os filhos
    for(int i = 0; i < 5; i++){
        new_element->filhos[i] = NULL;
    }

    // Inicializando vazio os medicamentos
    for(int i = 0; i < N - 1; i++){
        new_element->medicine[i] = NULL;
        new_element->referencia[i] = 0;
    }

    new_element->qtd = 0;
    new_element->leaf = false;
    new_element->father = NULL;
    new_element->next = NULL;

    return new_element;
}

medicamentos *info_medicamentos(pagina *root, medicamentos *novo){
    medicamentos *temp;

    printf("Codigo de barras: ");
    scanf("%d", &novo->codigo_barra);

    temp = search_medicine(root, novo->codigo_barra);

    if(temp != NULL){
        int quant;
        printf("Esse medicamento ja esta registrado no sistema.\n Quantas unidades do produto deseja adicionar? ");
        scanf("%d", &quant);

        temp->quantidade = temp->quantidade + quant;

        return NULL;
    }

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

void quebra_vetor(pagina **root, pagina *page, int value){
    int middle = N / 2;

    pagina *new_element = create_page();
    new_element->leaf = page->leaf;

    if(page->leaf == true){
        int i;
        for(i = middle; i < N - 1; i++){
            new_element->medicine[i - middle] = page->medicine[i]; // Transferindo metade dos filhos de um vetor ao novo vetor
            new_element->referencia[i - middle] = page->referencia[i];
            page->medicine[i] = NULL;
            page->referencia[i] = 0;
        }
        page->qtd = middle;
        new_element->qtd = N - 1 - middle;

        new_element->next = page->next;
        page->next = new_element;
    } else { // Caso não seja folha (tratando dos nós de referencia)
        // Transferência de referências e filhos para a nova página
        int i;
        int j = 0;
        for(i = middle; i < N; i++, j++){
            new_element->filhos[j] = page->filhos[i + 1];
            new_element->referencia[j] = page->referencia[i];
            page->referencia[i] = 0;
            page->filhos[i + 1] = NULL;
        }
        new_element->qtd = N - 1 - middle;
        page->qtd = middle;
    }
    
    // Atualizar dados do ponteiro
    if(page->father == NULL){
        // Na raiz
        pagina *nova_raiz = create_page();
        nova_raiz->referencia[0] = new_element->referencia[0];
        nova_raiz->qtd++;
        nova_raiz->filhos[0] = page;
        nova_raiz->filhos[1] = new_element;
        new_element->father = nova_raiz;
        page->father = nova_raiz;
        *root = nova_raiz;

    } else {
        // Inserindo nova referência e ponteiro na página pai (caso tenha pai)

        if(page->father->qtd == N - 1){
            int i = 0;

            if(page->father->father == NULL){
                quebra_vetor(root, page->father, 0);
            } else {
                quebra_vetor(root, page->father, page->father->father->qtd);
            }

            pagina *father = page->father->father->filhos[page->father->father->qtd];

            while(i < father->qtd){
                father->referencia[i] = father->referencia[i + 1];
                i++;
            }

            if(father->referencia[0] > new_element->referencia[0]){
                father->qtd--;
                father = page->father;
            }

            father->referencia[i - 1] = new_element->referencia[0];
            father->filhos[i] = new_element;
            new_element->father = father;

            return; 
        }

        int i;
        for(i = page->father->qtd; i > value; i--){
            page->father->referencia[i] = page->father->referencia[i - 1];
            page->father->filhos[i + 1] = page->father->filhos[i];
        }
        page->father->referencia[value] = new_element->referencia[0];
        page->father->filhos[value + 1] = new_element;
        page->father->qtd++;
        new_element->father = page->father;
    }
}

// Em cada filho, no caso 5, se possui um vetor, ou seja, 5 vetores (um vetor indicando a existencia de 5 vetores)
void inserir_medicamento(pagina **page, medicamentos *novo){
    int i = (*page)->qtd - 1;

    if((*page)->leaf == true){
        while(i >= 0 && (*page)->referencia[i] > novo->codigo_barra){
            (*page)->medicine[i + 1] = (*page)->medicine[i]; // Passa a ser o posterior (como se fosse o anterior)
            (*page)->referencia[i + 1] = (*page)->referencia[i]; // Mesma coisa
            i--; // Reduzo um para realizar novamente a comparação
        }
        i++; // Volto i ao seu estado normal
        // Atualizo valores e realizo a inserção
        (*page)->medicine[i] = novo;
        (*page)->referencia[i] = novo->codigo_barra;
        (*page)->qtd++;

    } else {
        while(i >= 0 && (*page)->referencia[i] > novo->codigo_barra){
            i--;
        }
        i++;

        if((*page)->filhos[i]->qtd == N - 1 && (*page)->filhos[i]->leaf == true){
            quebra_vetor(&(*page), (*page)->filhos[i], i);
            if((*page)->father == NULL){
                inserir_medicamento(page, novo);
            } else {
                inserir_medicamento(&(*page)->father, novo);
            }
        } else {
            inserir_medicamento(&(*page)->filhos[i], novo);
        }
    }
} 

void inserir_raiz(pagina **raiz){
    medicamentos *novo = (medicamentos *)malloc(sizeof(medicamentos));
    novo = info_medicamentos(*raiz, novo);

    // O elemento novo só teve sua quantidade de elementos aumentada
    if(novo == NULL){
        free(novo);
        return;
    }

    if((*raiz) == NULL){
        (*raiz) = create_page();
        (*raiz)->medicine[0] = novo;
        (*raiz)->leaf = true;
        (*raiz)->referencia[0] = novo->codigo_barra;
        (*raiz)->qtd++;
    } else {
        if((*raiz)->qtd == N - 1 && (*raiz)->leaf == true){
            quebra_vetor(raiz, *raiz, 0);
        }
        inserir_medicamento(&(*raiz), novo);
    }
}

// Letra B
medicamentos *search_medicine(pagina *root, int element) {
    if (root == NULL) {
        return NULL; // Retorna NULL se a árvore estiver vazia ou a página não existir
    }

    int i = 0;
    
    // Encontra a posição do elemento na página atual
    while (i < root->qtd && element > root->medicine[i]->codigo_barra) {
        i++;
    }
    
    // Verifica se o elemento foi encontrado
    if (i < root->qtd && element == root->medicine[i]->codigo_barra) {

        printf("  \nNome: %s\n",root->medicine[i]->nome_medicamento); 
            printf("  Codigo de Barras: %d\n",root->medicine[i]->codigo_barra);
            printf("  Quantidade: %d\n", root->medicine[i]->quantidade);
            printf("  Preco: %.2f\n", root->medicine[i]->valor);
            printf("  Data: %02d/%02d/%04d\n", root->medicine[i]->data_cadastro.dia, root->medicine[i]->data_cadastro.mes, root->medicine[i]->data_cadastro.ano);
        return root->medicine[i]; // Retorna a página onde o elemento foi encontrado
    }
    
    // Verifica se a página é uma folha
    if (root->leaf) {
        return NULL; // Se for folha e não encontrou, retorna NULL
    }

    // Busca recursiva no filho apropriado
    return search_medicine(root->filhos[i], element);
}

pagina *search_page(pagina *root, int codigo_barra){
    if(root == NULL) return NULL; 

    pagina *current = root;

    while(current != NULL){
        int i  = 0;

        while(i < current->qtd && codigo_barra > current->medicine[i]->codigo_barra){
            i++;
        }

        if(i < current->qtd && codigo_barra == current->medicine[i]->codigo_barra){
            return current;
        }

        if(current->leaf){
            return NULL;
        } else {
            current = current->filhos[i];
        }
    }
    return NULL;   
}

// Letra E
void print_nivel(pagina *page, int nivel) {
    if (page == NULL) return;

    // Espaçamento para representar os níveis na árvore
    for (int i = 0; i < nivel; i++) {
        printf("   ");
    }

    // Imprime as chaves do nível atual
    printf("Nivel %d [", nivel);
    for (int i = 0; i < page->qtd; i++) {
        printf("%d", page->referencia[i]);
        if (i < page->qtd - 1) {
            printf(", ");
        }
    }
    printf("]");

    if (page->leaf) {
        // Imprime os dados dos medicamentos nas folhas
        printf(" -> ");
        for (int i = 0; i < page->qtd; i++) {
            medicamentos *med = page->medicine[i];
            printf("{Codigo: %d, Nome: %s, Qtde: %d, Preco: %.2f}", 
                   med->codigo_barra, med->nome_medicamento, med->quantidade, med->valor);
            if (i < page->qtd - 1) {
                printf(", ");
            }
        }
    }

    printf("\n");

    // Recursivamente imprime os filhos (se existirem)
    if (!page->leaf) {
        for (int i = 0; i <= page->qtd; i++) {
            print_nivel(page->filhos[i], nivel + 1);
        }
    }
}

void print_arvore(pagina *root) {
    print_nivel(root, 0);
}

// Fica para imaginação do professor
pagina *gather_pages(pagina *root){
    // Função seria realizada para junção de paginas
}

void remover(pagina **root, int toRemove){
    if(root == NULL) return;

    printf("Que remedio deseja remover?");
    scanf("%d", &toRemove);

    medicamentos *remove = search_medicine(*root, toRemove);

    if(remove != NULL){
        int tam = (N - 1)/2;
        int qtd;
        printf("Quantas unidades deseja remover? ");
        scanf("%d", &qtd);

        if(qtd < remove->quantidade){
            remove->quantidade -= qtd;
            printf("%d unidades foram removidas!", qtd);
            return;

        } else { // Remover o nó inteiro
            pagina *page_remove = search_page(*root, remove->codigo_barra);

            int i;
            for(i = 0; i < page_remove->qtd; i++){ // Transferindo elementos
                page_remove->referencia[i] = page_remove->referencia[i + 1];
                page_remove->medicine[i] = page_remove->medicine[i + 1];
            }

            if(page_remove->qtd < tam && page_remove != *root){
                //gather_pages(*root);
            }
        }
    } else {
        printf("Nao ha elemento para ser removido! Raiz vazia.");
        return;
    }
}

void menu(){
    printf("\n");
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
                inserir_raiz(&raiz);
                break;
            case 2:
                print_arvore(raiz);
                break;
            case 3:
                printf("Numero a se buscar: ");
                scanf("%d", &num);
                search_medicine(raiz, num);
                break;
            case 4:
                printf("Elemento a remover: ");
                scanf("%d", &toRemove);
                remover(&raiz, toRemove);
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