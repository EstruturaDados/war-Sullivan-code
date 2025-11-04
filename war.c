// war_mestre.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

#define TAM_STRING 100

// Estrutura para armazenar os dados de um território
typedef struct {
    char nome[TAM_STRING];
    char cor[TAM_STRING];
    int tropas;
} Territorio;

// Estrutura para jogador
typedef struct {
    char nome[TAM_STRING];
    char cor[TAM_STRING];     // cor do jogador (ex: "Vermelho")
    char *missao;            // ponteiro alocado dinamicamente para a missão
    int missaoExibida;      // flag: missão exibida apenas 1 vez
} Jogador;

/* ---------------- Prototipos ---------------- */
void limparBufferEntrada(void);

void inicializarMapa(Territorio **mapa, int *totalTerritorios);
void cadastrarTerritorios(Territorio *mapa, int totalTerritorios);
void listarTerritorios(const Territorio *mapa, int totalTerritorios);

void exibirMissao(const Jogador *j);
void atribuirMissao(char **destino, char *missoes[], int totalMissoes);
int verificarMissao(const char *missao, const Jogador *j, const Territorio *mapa, int tamanho);

void atacar(Territorio *atacante, Territorio *defensor);
void simularAtaque(Territorio *mapa, int totalTerritorios, Jogador jogadores[], int numJogadores);
void liberarMemoria(Territorio *mapa, char *missoesAlocadas[], int numJogadores);

/* ---------------- Utilitários ---------------- */
// converte string para minusculas (uso para comparações simples)
void strToLower(char *s) {
    for (; *s; ++s) *s = tolower((unsigned char)*s);
}

// compara cores ignorando maiúsc/minúsc
int corIguais(const char *a, const char *b) {
    char ta[TAM_STRING], tb[TAM_STRING];
    strncpy(ta, a, TAM_STRING-1); ta[TAM_STRING-1] = '\0';
    strncpy(tb, b, TAM_STRING-1); tb[TAM_STRING-1] = '\0';
    strToLower(ta);
    strToLower(tb);
    return strcmp(ta, tb) == 0;
}

/* ---------------- Implementação ---------------- */

// limpa buffer de stdin
void limparBufferEntrada(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

// atribui uma missão aleatória copiando-a para destino (destino aponta para malloc'd char*)
void atribuirMissao(char **destino, char *missoes[], int totalMissoes) {
    if (totalMissoes <= 0) return;
    int idx = rand() % totalMissoes;
    size_t len = strlen(missoes[idx]);
    *destino = (char*) malloc(len + 1);
    if (*destino == NULL) {
        printf("Erro: sem memoria para atribuir missao.\n");
        exit(1);
    }
    strcpy(*destino, missoes[idx]);
}

// exibe missão — passada por valor (const pointer)
void exibirMissao(const Jogador *j) {
    if (j == NULL) return;
    printf("\n[%s] Missão atribuída: %s\n", j->nome, j->missao ? j->missao : "(nenhuma)");
}

// verifica se uma missão foi cumprida — retorna 1 se cumprida, 0 caso contrário
int verificarMissao(const char *missao, const Jogador *j, const Territorio *mapa, int tamanho) {
    if (missao == NULL || j == NULL || mapa == NULL) return 0;

    // Ex.: missão "Destruir o exército Verde"
    // Ex.: missão "Conquistar 3 territórios"
    // Ex.: missão "Eliminar todas as tropas da cor vermelha"
    // Ex.: missão "Conquistar 5 territórios"
    // Ex.: missão "Possuir 2 territórios com mais de 5 tropas"

    char copia[TAM_STRING];
    strncpy(copia, missao, TAM_STRING-1);
    copia[TAM_STRING-1] = '\0';
    strToLower(copia);

    // 1) Destruir o exército <cor>
    if (strstr(copia, "destruir") && strstr(copia, "exército")) {
        // tenta extrair cor (última palavra)
        char *p = strrchr(copia, ' ');
        if (p) {
            char corAlvo[TAM_STRING];
            strncpy(corAlvo, p+1, TAM_STRING-1);
            corAlvo[TAM_STRING-1] = '\0';
            // verifica se não existe território com essa cor OU todas tropas zero
            for (int i=0;i<tamanho;i++) {
                char corMapa[TAM_STRING];
                strncpy(corMapa, mapa[i].cor, TAM_STRING-1); corMapa[TAM_STRING-1]='\0';
                strToLower(corMapa);
                if (strcmp(corMapa, corAlvo) == 0 && mapa[i].tropas > 0) {
                    return 0; // ainda existe
                }
            }
            return 1;
        }
    }

    // 2) Conquistar N territórios  (procura por "conquistar X territórios")
    if (strstr(copia, "conquistar")) {
        // tenta achar número na string
        int n = 0;
        const char *p = copia;
        while (*p) {
            if (isdigit((unsigned char)*p)) {
                n = atoi(p);
                break;
            }
            p++;
        }
        if (n > 0) {
            int conta = 0;
            for (int i=0;i<tamanho;i++) {
                if (corIguais(mapa[i].cor, j->cor) && mapa[i].tropas > 0) conta++;
            }
            if (conta >= n) return 1;
            else return 0;
        } else {
            // se não achou número, possivelmente "conquistar 3 territórios seguidos" — usa 3 por padrão
            int conta = 0;
            for (int i=0;i<tamanho;i++) {
                if (corIguais(mapa[i].cor, j->cor) && mapa[i].tropas > 0) conta++;
            }
            if (conta >= 3) return 1;
            return 0;
        }
    }

    // 3) Eliminar todas as tropas da cor <X>
    if (strstr(copia, "eliminar") && strstr(copia, "tropas")) {
        // extrai cor alvo
        char *p = strrchr(copia, ' ');
        if (p) {
            char corAlvo[TAM_STRING];
            strncpy(corAlvo, p+1, TAM_STRING-1); corAlvo[TAM_STRING-1] = '\0';
            for (int i=0;i<tamanho;i++) {
                if (corIguais(mapa[i].cor, corAlvo) && mapa[i].tropas > 0) return 0;
            }
            return 1;
        }
    }

    // 4) Possuir X territórios com mais de Y tropas (formato "possuir 2 territórios com mais de 5 tropas")
    if (strstr(copia, "possuir") && strstr(copia, "tropas")) {
        // pegar números sequencialmente: X ... Y
        int nums[2] = {0,0};
        const char *p = copia;
        int found = 0;
        while (*p && found < 2) {
            if (isdigit((unsigned char)*p)) {
                nums[found++] = atoi(p);
                while (isdigit((unsigned char)*p)) p++;
            } else p++;
        }
        if (found == 2) {
            int contador = 0;
            for (int i=0;i<tamanho;i++) {
                if (corIguais(mapa[i].cor, j->cor) && mapa[i].tropas > nums[1]) contador++;
            }
            if (contador >= nums[0]) return 1;
            return 0;
        }
    }

    // Se não reconheceu a missão, retorna 0 (não cumprida)
    return 0;
}

// simula um ataque entre dois territorios (valores 1-6)
void atacar(Territorio *atacante, Territorio *defensor) {
    if (atacante == NULL || defensor == NULL) return;
    printf("\n=== SIMULAÇÃO DE ATAQUE ===\n");
    printf("%s (%s, %d tropas) atacando %s (%s, %d tropas)\n",
           atacante->nome, atacante->cor, atacante->tropas,
           defensor->nome, defensor->cor, defensor->tropas);

    int dado_atacante = rand() % 6 + 1;
    int dado_defensor = rand() % 6 + 1;

    printf("Dado do atacante: %d\n", dado_atacante);
    printf("Dado do defensor: %d\n", dado_defensor);

    if (dado_atacante >= dado_defensor) {
        // Atacante vence
        printf("\n>>> %s VENCEU a batalha!\n", atacante->nome);
        // Conquista: defensor muda de cor para atacante e metade das tropas do defensor (arredondando para cima) vai para atacante
        strcpy(defensor->cor, atacante->cor);
        int tropas_conquistadas = (defensor->tropas + 1) / 2;
        defensor->tropas = defensor->tropas - tropas_conquistadas;
        atacante->tropas += tropas_conquistadas;
        printf(">>> %s conquistou %d tropas de %s e assumiu a cor %s!\n",
               atacante->nome, tropas_conquistadas, defensor->nome, defensor->cor);
    } else {
        // Defensor vence
        printf("\n>>> %s DEFENDEU com sucesso!\n", defensor->nome);
        // Atacante perde uma tropa (se tiver)
        if (atacante->tropas > 0) atacante->tropas--;
        printf(">>> %s perdeu 1 tropa na batalha.\n", atacante->nome);
    }
}

// menu de seleção de ataque e validações
void simularAtaque(Territorio *mapa, int totalTerritorios, Jogador jogadores[], int numJogadores) {
    if (mapa == NULL || totalTerritorios < 2) {
        printf("Mapa inválido ou sem territórios suficientes.\n");
        return;
    }

    listarTerritorios(mapa, totalTerritorios);

    int atacante_idx, defensor_idx;
    printf("Escolha o jogador que atacará (1-%d): ", numJogadores);
    int jogador_idx;
    if (scanf("%d", &jogador_idx) != 1) { limparBufferEntrada(); printf("Entrada invalida.\n"); return; }
    limparBufferEntrada();
    if (jogador_idx < 1 || jogador_idx > numJogadores) { printf("Jogador inválido.\n"); return; }
    jogador_idx--; // base 0

    // selecionar atacante
    printf("Selecione o território ATACANTE (1-%d): ", totalTerritorios);
    if (scanf("%d", &atacante_idx) != 1) { limparBufferEntrada(); printf("Entrada invalida.\n"); return; }
    limparBufferEntrada();
    if (atacante_idx < 1 || atacante_idx > totalTerritorios) { printf("Território inválido!\n"); return; }
    atacante_idx--;

    // validar que atacante pertence ao jogador
    if (!corIguais(mapa[atacante_idx].cor, jogadores[jogador_idx].cor)) {
        printf("Erro: território selecionado não pertence ao jogador %s (cor %s).\n",
               jogadores[jogador_idx].nome, jogadores[jogador_idx].cor);
        return;
    }

    // selecionar defensor
    printf("Selecione o território DEFENSOR (1-%d): ", totalTerritorios);
    if (scanf("%d", &defensor_idx) != 1) { limparBufferEntrada(); printf("Entrada invalida.\n"); return; }
    limparBufferEntrada();
    if (defensor_idx < 1 || defensor_idx > totalTerritorios) { printf("Território inválido!\n"); return; }
    defensor_idx--;

    if (defensor_idx == atacante_idx) {
        printf("Um território não pode atacar a si mesmo!\n");
        return;
    }

    // validar cores diferentes
    if (corIguais(mapa[atacante_idx].cor, mapa[defensor_idx].cor)) {
        printf("Territórios da mesma cor não podem se atacar!\n");
        return;
    }

    // validar tropas mínimo (ex.: precisa ter ao menos 1 tropa)
    if (mapa[atacante_idx].tropas <= 0) {
        printf("Território atacante não tem tropas suficientes.\n");
        return;
    }

    // executar ataque
    atacar(&mapa[atacante_idx], &mapa[defensor_idx]);

    // mostrar situação pós-batalha
    printf("\n=== SITUAÇÃO PÓS-BATALHA ===\n");
    printf("%s: %d tropas (%s)\n", mapa[atacante_idx].nome, mapa[atacante_idx].tropas, mapa[atacante_idx].cor);
    printf("%s: %d tropas (%s)\n", mapa[defensor_idx].nome, mapa[defensor_idx].tropas, mapa[defensor_idx].cor);
}

// libera mapa e missões alocadas
void liberarMemoria(Territorio *mapa, char *missoesAlocadas[], int numJogadores) {
    if (mapa != NULL) free(mapa);
    for (int i=0;i<numJogadores;i++) {
        if (missoesAlocadas[i] != NULL) free(missoesAlocadas[i]);
    }
    printf("Memória liberada com sucesso!\n");
}

/* ---------------- Funções de entrada/saida de mapa ---------------- */
void cadastrarTerritorios(Territorio *mapa, int totalTerritorios) {
    if (mapa == NULL || totalTerritorios <= 0) return;
    printf("\n=== Cadastro de Territórios (inicialização automática disponível) ===\n");
    for (int i = 0; i < totalTerritorios; i++) {
        printf("\nTerritório %d:\n", i + 1);
        printf("Nome do território: ");
        fgets(mapa[i].nome, TAM_STRING, stdin);
        mapa[i].nome[strcspn(mapa[i].nome, "\n")] = '\0';

        printf("Cor (ex: Vermelho, Verde, Azul): ");
        fgets(mapa[i].cor, TAM_STRING, stdin);
        mapa[i].cor[strcspn(mapa[i].cor, "\n")] = '\0';

        printf("Tropas: ");
        scanf("%d", &mapa[i].tropas);
        limparBufferEntrada();
    }
    printf("\nTodos os territórios foram cadastrados com sucesso!\n");
}

void listarTerritorios(const Territorio *mapa, int totalTerritorios) {
    printf("\n=== Lista de Territórios ===\n");
    if (mapa == NULL || totalTerritorios <= 0) {
        printf("Nenhum território cadastrado.\n");
        return;
    }
    for (int i = 0; i < totalTerritorios; i++) {
        printf("\nTerritório %d:\n", i + 1);
        printf("Nome: %s\n", mapa[i].nome);
        printf("Cor: %s\n", mapa[i].cor);
        printf("Tropas: %d\n", mapa[i].tropas);
        printf("-------------------------");
    }
    printf("\n");
}

/* ---------------- Main ---------------- */
int main(void) {
    srand((unsigned int) time(NULL));

    // vetor de missões (mínimo 5 descrições)
    char *missoesPadrao[] = {
        "Destruir o exército Verde",
        "Conquistar 3 territórios",
        "Eliminar todas as tropas da cor vermelha",
        "Possuir 2 territórios com mais de 5 tropas",
        "Conquistar 5 territórios"
    };
    int totalMissoes = sizeof(missoesPadrao)/sizeof(missoesPadrao[0]);

    // Pergunta número de jogadores
    int numJogadores = 2; // defaut
    printf("Quantos jogadores? (2-4): ");
    if (scanf("%d", &numJogadores) != 1) { limparBufferEntrada(); numJogadores = 2; }
    limparBufferEntrada();
    if (numJogadores < 2) numJogadores = 2;
    if (numJogadores > 4) numJogadores = 4;

    Jogador jogadores[4];
    char *missoesAlocadas[4] = {NULL, NULL, NULL, NULL};

    // cadastrar nomes e cores dos jogadores
    for (int i=0;i<numJogadores;i++) {
        printf("\nInforme nome do jogador %d: ", i+1);
        fgets(jogadores[i].nome, TAM_STRING, stdin);
        jogadores[i].nome[strcspn(jogadores[i].nome, "\n")] = '\0';

        printf("Informe cor do jogador %d: ", i+1);
        fgets(jogadores[i].cor, TAM_STRING, stdin);
        jogadores[i].cor[strcspn(jogadores[i].cor, "\n")] = '\0';

        jogadores[i].missao = NULL;
        jogadores[i].missaoExibida = 0;

        // atribui missão dinâmica
        atribuirMissao(&missoesAlocadas[i], missoesPadrao, totalMissoes);
        jogadores[i].missao = missoesAlocadas[i];
    }

    // Criar mapa de territórios
    int totalTerritorios = 0;
    Territorio *mapa = NULL;
    printf("\nQuantos territórios no mapa? (recomendado >= %d): ", numJogadores*3);
    if (scanf("%d", &totalTerritorios) != 1) { limparBufferEntrada(); totalTerritorios = numJogadores*3; }
    limparBufferEntrada();
    if (totalTerritorios <= 0) totalTerritorios = numJogadores*3;

    mapa = (Territorio*) calloc(totalTerritorios, sizeof(Territorio));
    if (mapa == NULL) {
        printf("Erro na alocação de memória para mapa.\n");
        liberarMemoria(mapa, missoesAlocadas, numJogadores);
        return 1;
    }

    // opção: inicializar de forma automática (para facilitar testes) ou manual
    int escolhaInit = 0;
    printf("Inicializar territórios automaticamente? (1-Sim / 0-Nao): ");
    if (scanf("%d", &escolhaInit) != 1) { limparBufferEntrada(); escolhaInit = 1; }
    limparBufferEntrada();

    if (escolhaInit == 1) {
        // distribui territórios entre cores dos jogadores e cria nomes e tropas aleatórias
        for (int i=0;i<totalTerritorios;i++) {
            snprintf(mapa[i].nome, TAM_STRING, "Territorio_%d", i+1);
            // alterna entre jogadores para cores
            int owner = i % numJogadores;
            strncpy(mapa[i].cor, jogadores[owner].cor, TAM_STRING-1);
            mapa[i].cor[TAM_STRING-1] = '\0';
            mapa[i].tropas = (rand() % 8) + 1; // 1..8 tropas
        }
        printf("Mapa inicializado automaticamente.\n");
    } else {
        cadastrarTerritorios(mapa, totalTerritorios);
    }

    // exibe missões (cada jogador apenas UMA vez)
    for (int i=0;i<numJogadores;i++) {
        printf("\n================================================\n");
        printf("Jogador %s (cor %s) - MISSÃO (mostrada apenas uma vez):\n", jogadores[i].nome, jogadores[i].cor);
        exibirMissao(&jogadores[i]);
        jogadores[i].missaoExibida = 1;
        printf("================================================\n");
    }

    // Loop principal de menu: atacar, verificar missão, listar mapa, sair
    int opcao = -1;
    while (opcao != 0) {
        printf("\n====== MENU ======\n");
        printf("1 - Atacar\n");
        printf("2 - Verificar missão de um jogador\n");
        printf("3 - Listar territórios\n");
        printf("0 - Sair\n");
        printf("Escolha uma opcao: ");
        if (scanf("%d", &opcao) != 1) { limparBufferEntrada(); opcao = -1; }
        limparBufferEntrada();

        if (opcao == 1) {
            simularAtaque(mapa, totalTerritorios, jogadores, numJogadores);
            // após cada ataque, verificar silenciosamente se alguém cumpriu a missão
            for (int i=0;i<numJogadores;i++) {
                if (verificarMissao(jogadores[i].missao, &jogadores[i], mapa, totalTerritorios)) {
                    printf("\n>>> JOGADOR %s (cor %s) CUMPRIU A MISSÃO: %s\n", jogadores[i].nome, jogadores[i].cor, jogadores[i].missao);
                    printf(">>> FIM DE JOGO. Parabéns!\n");
                    opcao = 0; // encerrar
                    break;
                }
            }
        } else if (opcao == 2) {
            printf("Verificar missão de qual jogador? (1-%d): ", numJogadores);
            int jidx; if (scanf("%d", &jidx) != 1) { limparBufferEntrada(); printf("Entrada invalida.\n"); continue; }
            limparBufferEntrada();
            if (jidx < 1 || jidx > numJogadores) { printf("Jogador invalido.\n"); continue; }
            jidx--;
            printf("\nMissão de %s: %s\n", jogadores[jidx].nome, jogadores[jidx].missao);
            int ok = verificarMissao(jogadores[jidx].missao, &jogadores[jidx], mapa, totalTerritorios);
            if (ok) {
                printf("Status: MISSÃO CUMPRIDA!\n");
                printf(">>> JOGADOR %s (cor %s) VENCEU!\n", jogadores[jidx].nome, jogadores[jidx].cor);
                break;
            } else {
                printf("Status: ainda NÃO cumprida.\n");
            }
        } else if (opcao == 3) {
            listarTerritorios(mapa, totalTerritorios);
        } else if (opcao == 0) {
            printf("Saindo...\n");
        } else {
            printf("Opcao invalida. Tente novamente.\n");
        }
    }

    // liberar memoria
    liberarMemoria(mapa, missoesAlocadas, numJogadores);

    return 0;
}
