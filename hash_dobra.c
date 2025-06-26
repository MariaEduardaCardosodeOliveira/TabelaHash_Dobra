// hash_unificado.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define TAM 10

//------------------------------------------
// Estruturas e utilitarios comuns
//------------------------------------------
typedef struct ItemEnc {
    int chave;
    char valor[50];
    struct ItemEnc* proximo;
} ItemEnc;

typedef struct ItemEncBin {
    char chave_bin[33];
    char valor[50];
    struct ItemEncBin* proximo;
} ItemEncBin;

typedef struct {
    int chave;
    char valor[50];
    bool ocupado;
} ItemEnd;

typedef struct {
    char chave_bin[33];
    char valor[50];
    bool ocupado;
} ItemBinEnd;

// Tabelas globais
ItemEnc*   tabela_enc_dec[TAM];
ItemEncBin* tabela_enc_bin[TAM];
ItemEnd    tabela_end_dec[TAM];
ItemBinEnd tabela_end_bin[TAM];

// Contadores de colisao
int colisoes_enc_dec = 0;
int colisoes_enc_bin = 0;
int colisoes_end_dec = 0;
int colisoes_end_bin = 0;

//------------------------------------------
// Utilitarios
//------------------------------------------
int bin_para_decimal(const char* bin) {
    int decimal = 0;
    for (int i = 0; bin[i] != '\0'; i++)
        decimal = decimal * 2 + (bin[i] - '0');
    return decimal;
}

int chave_binaria_valida(const char* chave) {
    int len = strlen(chave);
    if (len == 0 || len > 32) return 0;
    for (int i = 0; i < len; i++) {
        if (chave[i] != '0' && chave[i] != '1') return 0;
    }
    return 1;
}

void solicitar_chave_bin(char* chave_bin) {
    do {
        printf("Chave binaria (ate 32 bits): ");
        scanf("%32s", chave_bin);
        if (!chave_binaria_valida(chave_bin))
            printf("Chave invalida. Digite apenas 0s e 1s.\n");
    } while (!chave_binaria_valida(chave_bin));
}

int solicitar_bits_validos(int tam_chave) {
    int bits;
    do {
        printf("Quantos bits por parte deseja usar na dobra? ");
        if (scanf("%d", &bits) != 1 || bits <= 0 || bits > tam_chave) {
            printf("Numero invalido.\n");
            while (scanf("%*[^\n]"), getchar() != '\n');
            bits = -1;
        }
    } while (bits <= 0 || bits > tam_chave);
    return bits;
}

//------------------------------------------
// Hashes com exibicao do metodo da dobra
//------------------------------------------
int mostrar_dobra_decimal(int chave) {
    int soma = 0, partes[10], i = 0, copia = chave;
    while (copia > 0) {
        partes[i++] = copia % 100;
        copia /= 100;
    }
    printf("\n--- Metodo da Dobra (chave decimal: %d) ---\n", chave);
    printf("Partes: ");
    for (int j = i - 1; j >= 0; j--) {
        soma += partes[j];
        printf("%d", partes[j]);
        if (j != 0) printf(" + ");
    }
    int hash = soma % TAM;
    printf(" = %d\n", soma);
    printf("Hash final: %d %% %d = %d\n", soma, TAM, hash);
    return hash;
}

int mostrar_dobra_bin(const char* chave_bin, int bits_por_parte) {
    int soma = 0;
    char parte[33];
    int len = strlen(chave_bin);
    int i = 0, parte_num = 1;
    printf("\n--- Metodo da Dobra (chave binaria: %s) ---\n", chave_bin);
    printf("Dividindo em partes de %d bits:\n  ", bits_por_parte);
    while (i < len) {
        int tam_parte = (len - i >= bits_por_parte) ? bits_por_parte : len - i;
        strncpy(parte, chave_bin + i, tam_parte);
        parte[tam_parte] = '\0';
        int valor = bin_para_decimal(parte);
        soma += valor;
        printf("Parte %d: %s -> %d", parte_num++, parte, valor);
        i += tam_parte;
        if (i < len) printf("\n  ");
    }
    int hash = soma % TAM;
    printf("\nSoma das partes: %d\n", soma);
    printf("Hash final: %d %% %d = %d\n", soma, TAM, hash);
    return hash;
}

//------------------------------------------
// Impressao e estatisticas
//------------------------------------------
void imprimir_tabelas() {
    printf("\n--- Tabela Encadeamento Decimal ---\n");
    for (int i = 0; i < TAM; i++) {
        printf("[%d]: ", i);
        for (ItemEnc* e = tabela_enc_dec[i]; e; e = e->proximo)
            printf("(%d,%s) -> ", e->chave, e->valor);
        printf("NULL\n");
    }
    printf("\n--- Tabela Encadeamento Binario ---\n");
    for (int i = 0; i < TAM; i++) {
        printf("[%d]: ", i);
        for (ItemEncBin* e = tabela_enc_bin[i]; e; e = e->proximo)
            printf("(%s,%s) -> ", e->chave_bin, e->valor);
        printf("NULL\n");
    }
    printf("\n--- Tabela Enderecamento Decimal ---\n");
    for (int i = 0; i < TAM; i++) {
        if (tabela_end_dec[i].ocupado)
            printf("[%d]: (%d,%s)\n", i, tabela_end_dec[i].chave, tabela_end_dec[i].valor);
        else
            printf("[%d]: NULL\n", i);
    }
    printf("\n--- Tabela Enderecamento Binario ---\n");
    for (int i = 0; i < TAM; i++) {
        if (tabela_end_bin[i].ocupado)
            printf("[%d]: (%s,%s)\n", i, tabela_end_bin[i].chave_bin, tabela_end_bin[i].valor);
        else
            printf("[%d]: NULL\n", i);
    }
}

void mostrar_estatisticas() {
    printf("\nColisoes Encadeamento Decimal: %d\n", colisoes_enc_dec);
    printf("Colisoes Encadeamento Binario: %d\n", colisoes_enc_bin);
    printf("Colisoes Enderecamento Decimal: %d\n", colisoes_end_dec);
    printf("Colisoes Enderecamento Binario: %d\n", colisoes_end_bin);
}

//------------------------------------------
// Menus de manipulacao
//------------------------------------------
void menu_encadeamento_decimal() {
    int chave; 
    char valor[50];

    printf("\n[Encadeamento Decimal] Digite chave: ");
    scanf("%d", &chave);

    printf("Valor: ");
    scanf(" %49[^\n]", valor);

    int pos = mostrar_dobra_decimal(chave);

    if (tabela_enc_dec[pos]) {
        printf("ALERTA: Colisao detectada na posicao %d\n", pos);
        colisoes_enc_dec++;
    }

    ItemEnc* novo = malloc(sizeof(ItemEnc));
    novo->chave = chave;
    strcpy(novo->valor, valor);
    novo->proximo = tabela_enc_dec[pos];
    tabela_enc_dec[pos] = novo;
}


void menu_encadeamento_binario() {
    char chave_bin[33], 
    valor[50];
    solicitar_chave_bin(chave_bin);
    printf("Valor: "); 
    scanf(" %49[^\n]", valor);
    int bits = solicitar_bits_validos(strlen(chave_bin));
    int pos = mostrar_dobra_bin(chave_bin, bits);
    if (tabela_enc_bin[pos]) {
        printf("ALERTA: Colisao detectada na posicao %d\n", pos);
        colisoes_enc_bin++;
    }
    ItemEncBin* novo = malloc(sizeof(ItemEncBin));
    strcpy(novo->chave_bin, chave_bin);
    strcpy(novo->valor, valor);
    novo->proximo = tabela_enc_bin[pos];
    tabela_enc_bin[pos] = novo;
}

void menu_enderecamento_decimal() {
    int chave; 
    char valor[50];
    printf("\n[Enderecamento Decimal] Digite chave: "); 
    scanf("%d", &chave);
    printf("Valor: "); 
    scanf(" %49[^\n]", valor);
    int pos = mostrar_dobra_decimal(chave);
    for (int i = 0; i < TAM; i++) {
        int idx = (pos + i) % TAM;
        if (!tabela_end_dec[idx].ocupado) {
            if (i > 0) {
                printf("ALERTA: Colisao na posicao original %d, inserido em %d\n", pos, idx);
                colisoes_end_dec++;
            }
            tabela_end_dec[idx].chave = chave;
            strcpy(tabela_end_dec[idx].valor, valor);
            tabela_end_dec[idx].ocupado = true;
            break;
        }
    }
}

void menu_enderecamento_binario() {
    char chave_bin[33], valor[50];
    solicitar_chave_bin(chave_bin);
    printf("Valor: "); scanf(" %49[^\n]", valor);
    int bits = solicitar_bits_validos(strlen(chave_bin));
    int pos = mostrar_dobra_bin(chave_bin, bits);
    for (int i = 0; i < TAM; i++) {
        int idx = (pos + i) % TAM;
        if (!tabela_end_bin[idx].ocupado) {
            if (i > 0) {
                printf("ALERTA: Colisao na posicao original %d, inserido em %d\n", pos, idx);
                colisoes_end_bin++;
            }
            strcpy(tabela_end_bin[idx].chave_bin, chave_bin);
            strcpy(tabela_end_bin[idx].valor, valor);
            tabela_end_bin[idx].ocupado = true;
            break;
        }
    }
}

void menu_buscar() {
    int tipo;
    char valor[50];

    // Exibe submenu de busca
    printf("\n--- BUSCA ---\n");
    printf("1 - Chave decimal\n2 - Chave binaria\n3 - Valor\nOpcao: ");

    // Lê tipo com tratamento de erro
    if (scanf(" %d", &tipo) != 1) {
        printf("Opcao invalida.\n");
        while (getchar() != '\n');
        return;
    }

    switch (tipo) {
        case 1: {
            int chave;
            printf("Chave decimal: ");
            if (scanf(" %d", &chave) != 1) {
                printf("Entrada invalida.\n");
                while (getchar() != '\n');
                return;
            }
            int pos = mostrar_dobra_decimal(chave);
            // Encadeamento decimal
            for (ItemEnc* e = tabela_enc_dec[pos]; e; e = e->proximo) {
                if (e->chave == chave)
                    printf("[enc_dec] %s\n", e->valor);
            }
            // Enderecamento decimal
            if (tabela_end_dec[pos].ocupado && tabela_end_dec[pos].chave == chave)
                printf("[end_dec] %s\n", tabela_end_dec[pos].valor);
            break;
        }
        case 2: {
            char bin[33];
            solicitar_chave_bin(bin);
            int bits = solicitar_bits_validos(strlen(bin));
            int pos = mostrar_dobra_bin(bin, bits);
            // Encadeamento binário
            for (ItemEncBin* e = tabela_enc_bin[pos]; e; e = e->proximo) {
                if (strcmp(e->chave_bin, bin) == 0)
                    printf("[enc_bin] %s\n", e->valor);
            }
            // Enderecamento binário
            if (tabela_end_bin[pos].ocupado && strcmp(tabela_end_bin[pos].chave_bin, bin) == 0)
                printf("[end_bin] %s\n", tabela_end_bin[pos].valor);
            break;
        }
        case 3: {
            printf("Valor: ");
            scanf(" %49[^\n]", valor);
            // Busca por valor em todas as estruturas
            for (int i = 0; i < TAM; i++) {
                for (ItemEnc* e = tabela_enc_dec[i]; e; e = e->proximo)
                    if (strcmp(e->valor, valor) == 0)
                        printf("[enc_dec] %d\n", e->chave);
                for (ItemEncBin* e = tabela_enc_bin[i]; e; e = e->proximo)
                    if (strcmp(e->valor, valor) == 0)
                        printf("[enc_bin] %s\n", e->chave_bin);
                if (tabela_end_dec[i].ocupado && strcmp(tabela_end_dec[i].valor, valor) == 0)
                    printf("[end_dec] %d\n", tabela_end_dec[i].chave);
                if (tabela_end_bin[i].ocupado && strcmp(tabela_end_bin[i].valor, valor) == 0)
                    printf("[end_bin] %s\n", tabela_end_bin[i].chave_bin);
            }
            break;
        }
        default:
            printf("Opcao invalida.\n");
    }
}


void menu_remover() {
    int tipo;
    char valor[50];
    int removidos = 0;

    // Exibe submenu de remocao
    printf("\n--- REMOCAO ---\n");
    printf("1 - Chave decimal\n2 - Chave binaria\n3 - Valor\nOpcao: ");

    // Lê tipo com tratamento de erro
    if (scanf(" %d", &tipo) != 1) {
        printf("Opcao invalida.\n");
        while (getchar() != '\n');
        return;
    }

    switch (tipo) {
        case 1: {
            int chave;
            printf("Chave decimal: ");
            if (scanf(" %d", &chave) != 1) {
                printf("Entrada invalida.\n");
                while (getchar() != '\n');
                return;
            }
            int pos = mostrar_dobra_decimal(chave);
            // Remocao em encadeamento decimal
            ItemEnc* e = tabela_enc_dec[pos];
            ItemEnc* ant = NULL;
            while (e) {
                if (e->chave == chave) {
                    if (ant) ant->proximo = e->proximo;
                    else tabela_enc_dec[pos] = e->proximo;
                    free(e);
                    removidos = 1;
                    break;
                }
                ant = e;
                e = e->proximo;
            }
            // Remocao em enderecamento decimal
            if (tabela_end_dec[pos].ocupado && tabela_end_dec[pos].chave == chave) {
                tabela_end_dec[pos].ocupado = false;
                removidos = 1;
            }
            break;
        }
        case 2: {
            char bin[33];
            solicitar_chave_bin(bin);
            int bits = solicitar_bits_validos(strlen(bin));
            int pos = mostrar_dobra_bin(bin, bits);
            // Remocao em encadeamento binario
            ItemEncBin* e = tabela_enc_bin[pos];
            ItemEncBin* ant = NULL;
            while (e) {
                if (strcmp(e->chave_bin, bin) == 0) {
                    if (ant) ant->proximo = e->proximo;
                    else tabela_enc_bin[pos] = e->proximo;
                    free(e);
                    removidos = 1;
                    break;
                }
                ant = e;
                e = e->proximo;
            }
            // Remocao em enderecamento binario
            if (tabela_end_bin[pos].ocupado && strcmp(tabela_end_bin[pos].chave_bin, bin) == 0) {
                tabela_end_bin[pos].ocupado = false;
                removidos = 1;
            }
            break;
        }
        case 3: {
            printf("Valor: ");
            scanf(" %49[^\n]", valor);
            // Remocao por valor em todas as estruturas
            for (int i = 0; i < TAM; i++) {
                // Encadeamento decimal
                ItemEnc* e1 = tabela_enc_dec[i];
                ItemEnc* a1 = NULL;
                while (e1) {
                    if (strcmp(e1->valor, valor) == 0) {
                        if (a1) a1->proximo = e1->proximo;
                        else tabela_enc_dec[i] = e1->proximo;
                        free(e1);
                        removidos = 1;
                        break;
                    }
                    a1 = e1;
                    e1 = e1->proximo;
                }
                // Encadeamento binario
                ItemEncBin* e2 = tabela_enc_bin[i];
                ItemEncBin* a2 = NULL;
                while (e2) {
                    if (strcmp(e2->valor, valor) == 0) {
                        if (a2) a2->proximo = e2->proximo;
                        else tabela_enc_bin[i] = e2->proximo;
                        free(e2);
                        removidos = 1;
                        break;
                    }
                    a2 = e2;
                    e2 = e2->proximo;
                }
                // Enderecamento decimal
                if (tabela_end_dec[i].ocupado && strcmp(tabela_end_dec[i].valor, valor) == 0) {
                    tabela_end_dec[i].ocupado = false;
                    removidos = 1;
                }
                // Enderecamento binario
                if (tabela_end_bin[i].ocupado && strcmp(tabela_end_bin[i].valor, valor) == 0) {
                    tabela_end_bin[i].ocupado = false;
                    removidos = 1;
                }
            }
            break;
        }
        default:
            printf("Opcao invalida.\n");
            return;
    }

    // Mensagem de resultado
    if (removidos)
        printf("Remocao concluida com sucesso.\n");
    else
        printf("Nenhum item removido.\n");
}


//------------------------------------------
// Menu Principal
//------------------------------------------
int main() {
    int opc;
    // inicializar tabelas
    for (int i = 0; i < TAM; i++) {
        tabela_enc_dec[i] = NULL;
        tabela_enc_bin[i] = NULL;
        tabela_end_dec[i].ocupado = false;
        tabela_end_bin[i].ocupado = false;
    }
    do {
        printf("\n=========== MENU HASH UNIFICADO ===========\n");
        printf("1-Inserir Encadeamento Decimal\n");
        printf("2-Inserir Encadeamento Binario\n");
        printf("3-Inserir Enderecamento Decimal\n");
        printf("4-Inserir Enderecamento Binario\n");
        printf("5-Imprimir Tabelas\n");
        printf("6-Ver Estatisticas\n");
        printf("7-Buscar\n");
        printf("8-Remover\n");
        printf("0-Sair\n");
        printf("Escolha: ");
        scanf("%d", &opc);

        switch (opc) {
            case 1: menu_encadeamento_decimal(); break;
            case 2: menu_encadeamento_binario(); break;
            case 3: menu_enderecamento_decimal(); break;
            case 4: menu_enderecamento_binario(); break;
            case 5: imprimir_tabelas(); break;
            case 6: mostrar_estatisticas(); break;
            case 7: menu_buscar(); break;
            case 8: menu_remover(); break;
            case 0: printf("Encerrando...\n"); break;
            default: printf("Opc invalida!\n");
        }
    } while (opc != 0);
    return 0;
}
