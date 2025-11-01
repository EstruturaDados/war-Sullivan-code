#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_TERRITORIOS 5
#define TAM_STRING 100

// Estrutura para armazenar os dados de um território
struct Territorio {
    char nome[TAM_STRING];
    char cor[TAM_STRING];
    int tropas;
};

// Função para limpar o buffer de entrada
void limparBufferEntrada() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

int main() {
    struct Territorio Territorios[MAX_TERRITORIOS];
    int totalTerritorios = 0;
    int opcao;

    // --- Laço Principal do Menu ---
    do {
        // Exibe o menu de opções
        printf("=================================\n");
        printf("        WAR - NÍVEL NOVATO\n");
        printf("=================================\n");
        printf("1 - Cadastrar novo território\n");
        printf("2 - Listar todos os territórios\n");
        printf("0 - Sair\n");
        printf("---------------------------------\n");
        printf("Escolha uma opcao: ");
        scanf("%d", &opcao);
        limparBufferEntrada();

        switch (opcao) {
            case 1: // CADASTRO DE TERRITÓRIOS
                if (totalTerritorios < MAX_TERRITORIOS) {
                    printf("\n=== Cadastro de Novo Território ===\n");
                    printf("Nome do território: ");
                    fgets(Territorios[totalTerritorios].nome, TAM_STRING, stdin);
                    Territorios[totalTerritorios].nome[strcspn(Territorios[totalTerritorios].nome, "\n")] = '\0';

                    printf("Cor: ");
                    fgets(Territorios[totalTerritorios].cor, TAM_STRING, stdin);
                    Territorios[totalTerritorios].cor[strcspn(Territorios[totalTerritorios].cor, "\n")] = '\0';

                    printf("Tropas: ");
                    scanf("%d", &Territorios[totalTerritorios].tropas);
                    limparBufferEntrada();

                    totalTerritorios++;
                    printf("\nTerritório cadastrado com sucesso!\n\n");
                } else {
                    printf("\nLimite de territórios atingido!\n\n");
                }
                break;

            case 2:
                printf("\n=== Lista de Territórios ===\n");
                if (totalTerritorios == 0) {
                    printf("Nenhum território cadastrado.\n\n");
                } else {
                    for (int i = 0; i < totalTerritorios; i++) {
                        printf("Território %d:\n", i + 1);
                        printf("Nome: %s\n", Territorios[i].nome);
                        printf("Cor: %s\n", Territorios[i].cor);
                        printf("Tropas: %d\n", Territorios[i].tropas);
                        printf("-------------------------\n");
                    }
                }
                break;

            case 0:
                printf("\nSaindo do sistema...\n");
                break;

            default:
                printf("\nOpcao invalida! Tente novamente.\n\n");
                printf("\nPressione Enter para continuar...");
                getchar();
                break;
        }

    } while (opcao != 0);

    return 0;
}
