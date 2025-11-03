#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define TAM_STRING 100

// Estrutura para armazenar os dados de um território
struct Territorio {
    char nome[TAM_STRING];
    char cor[TAM_STRING];
    int tropas;
};

// Protótipos das funções
void limparBufferEntrada();
void cadastrarTerritorios(struct Territorio *mapa, int totalTerritorios);
void listarTerritorios(struct Territorio *mapa, int totalTerritorios);
void atacar(struct Territorio *atacante, struct Territorio *defensor);
void liberarMemoria(struct Territorio *mapa);
void simularBatalha(struct Territorio *mapa, int totalTerritorios);

int main() {
    struct Territorio *mapa = NULL;
    int totalTerritorios = 0;
    int opcao;

    // Inicializar semente para números aleatórios
    srand(time(NULL));

    // --- Laço Principal do Menu ---
    do {
        // Exibe o menu de opções
        printf("\n=================================\n");
        printf("        WAR - NÍVEL AVENTUREIRO\n");
        printf("=================================\n");
        printf("1 - Cadastrar territórios\n");
        printf("2 - Listar todos os territórios\n");
        printf("3 - Simular ataque\n");
        printf("0 - Sair\n");
        printf("---------------------------------\n");
        printf("Escolha uma opcao: ");
        scanf("%d", &opcao);
        limparBufferEntrada();

        switch (opcao) {
            case 1: // CADASTRO DE TERRITÓRIOS
                if (mapa != NULL) {
                    liberarMemoria(mapa);
                }
                
                printf("\nQuantos territórios deseja cadastrar? ");
                scanf("%d", &totalTerritorios);
                limparBufferEntrada();

                if (totalTerritorios <= 0) {
                    printf("Número inválido de territórios!\n");
                    break;
                }

                // Alocação dinâmica de memória
                mapa = (struct Territorio*) calloc(totalTerritorios, sizeof(struct Territorio));
                if (mapa == NULL) {
                    printf("Erro na alocação de memória!\n");
                    exit(1);
                }

                cadastrarTerritorios(mapa, totalTerritorios);
                break;

            case 2: // LISTAR TERRITÓRIOS
                listarTerritorios(mapa, totalTerritorios);
                break;

            case 3: // SIMULAR ATAQUE
                if (mapa != NULL && totalTerritorios >= 2) {
                    simularBatalha(mapa, totalTerritorios);
                } else {
                    printf("\nÉ necessário ter pelo menos 2 territórios cadastrados para atacar!\n");
                }
                break;

            case 0: // SAIR
                printf("\nSaindo do sistema...\n");
                if (mapa != NULL) {
                    liberarMemoria(mapa);
                }
                break;

            default:
                printf("\nOpcao invalida! Tente novamente.\n");
                break;
        }

    } while (opcao != 0);

    return 0;
}

// Função para limpar o buffer de entrada
void limparBufferEntrada() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

// Função para cadastrar territórios
void cadastrarTerritorios(struct Territorio *mapa, int totalTerritorios) {
    printf("\n=== Cadastro de Territórios ===\n");
    for (int i = 0; i < totalTerritorios; i++) {
        printf("\nTerritório %d:\n", i + 1);
        
        printf("Nome do território: ");
        fgets(mapa[i].nome, TAM_STRING, stdin);
        mapa[i].nome[strcspn(mapa[i].nome, "\n")] = '\0';

        printf("Cor: ");
        fgets(mapa[i].cor, TAM_STRING, stdin);
        mapa[i].cor[strcspn(mapa[i].cor, "\n")] = '\0';

        printf("Tropas: ");
        scanf("%d", &mapa[i].tropas);
        limparBufferEntrada();
    }
    printf("\nTodos os territórios foram cadastrados com sucesso!\n");
}

// Função para listar territórios
void listarTerritorios(struct Territorio *mapa, int totalTerritorios) {
    printf("\n=== Lista de Territórios ===\n");
    if (totalTerritorios == 0 || mapa == NULL) {
        printf("Nenhum território cadastrado.\n");
    } else {
        for (int i = 0; i < totalTerritorios; i++) {
            printf("\nTerritório %d:\n", i + 1);
            printf("Nome: %s\n", mapa[i].nome);
            printf("Cor: %s\n", mapa[i].cor);
            printf("Tropas: %d\n", mapa[i].tropas);
            printf("-------------------------");
        }
    }
    printf("\n");
}

// Função para simular batalha entre territórios
void atacar(struct Territorio *atacante, struct Territorio *defensor) {
    printf("\n=== SIMULAÇÃO DE ATAQUE ===\n");
    printf("%s (%s) atacando %s (%s)\n", 
           atacante->nome, atacante->cor, 
           defensor->nome, defensor->cor);

    // Simulação de dados de batalha (1-6)
    int dado_atacante = rand() % 6 + 1;
    int dado_defensor = rand() % 6 + 1;

    printf("Dado do atacante: %d\n", dado_atacante);
    printf("Dado do defensor: %d\n", dado_defensor);

    if (dado_atacante >= dado_defensor) {
        // Atacante vence
        printf("\n>>> %s VENCEU a batalha!\n", atacante->nome);
        
        // Transferir cor e metade das tropas (arredondando para cima)
        strcpy(defensor->cor, atacante->cor);
        int tropas_conquistadas = (defensor->tropas + 1) / 2;
        defensor->tropas = defensor->tropas - tropas_conquistadas;
        atacante->tropas += tropas_conquistadas;

        printf(">>> %s conquistou %d tropas de %s!\n", 
               atacante->nome, tropas_conquistadas, defensor->nome);
    } else {
        // Defensor vence
        printf("\n>>> %s DEFENDEU com sucesso!\n", defensor->nome);
        
        // Atacante perde uma tropa
        atacante->tropas--;
        printf(">>> %s perdeu 1 tropa na batalha.\n", atacante->nome);
    }
}

// Função para simular batalha entre territórios
void simularBatalha(struct Territorio *mapa, int totalTerritorios) {
    int atacante_idx, defensor_idx;

    printf("\n=== SELECÃO DE ATAQUE ===\n");
    
    // Listar territórios disponíveis para ataque
    listarTerritorios(mapa, totalTerritorios);

    // Selecionar território atacante
    printf("Selecione o território ATACANTE (1-%d): ", totalTerritorios);
    scanf("%d", &atacante_idx);
    limparBufferEntrada();

    if (atacante_idx < 1 || atacante_idx > totalTerritorios) {
        printf("Território inválido!\n");
        return;
    }

    // Selecionar território defensor
    printf("Selecione o território DEFENSOR (1-%d): ", totalTerritorios);
    scanf("%d", &defensor_idx);
    limparBufferEntrada();

    if (defensor_idx < 1 || defensor_idx > totalTerritorios) {
        printf("Território inválido!\n");
        return;
    }

    // Ajustar índices para base 0
    atacante_idx--;
    defensor_idx--;

    // Validar se não é o mesmo território
    if (atacante_idx == defensor_idx) {
        printf("Um território não pode atacar a si mesmo!\n");
        return;
    }

    // Validar se as cores são diferentes
    if (strcmp(mapa[atacante_idx].cor, mapa[defensor_idx].cor) == 0) {
        printf("Territórios da mesma cor não podem se atacar!\n");
        return;
    }

    // Executar ataque
    atacar(&mapa[atacante_idx], &mapa[defensor_idx]);

    // Mostrar situação pós-batalha
    printf("\n=== SITUAÇÃO PÓS-BATALHA ===\n");
    printf("Atacante - %s: %d tropas\n", 
           mapa[atacante_idx].nome, mapa[atacante_idx].tropas);
    printf("Defensor - %s: %d tropas (%s)\n", 
           mapa[defensor_idx].nome, mapa[defensor_idx].tropas, mapa[defensor_idx].cor);
}

// Função para liberar memória alocada
void liberarMemoria(struct Territorio *mapa) {
    free(mapa);
    printf("Memória liberada com sucesso!\n");
}