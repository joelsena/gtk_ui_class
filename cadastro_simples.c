#include <gtk/gtk.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

GtkBuilder *builder;
GtkWidget *window;
GtkStack *stack;
GtkListStore *modelo_armazenamento;

typedef struct {
    int id;
    char nome[100];
    char email[100];
    struct User* prox;
} User;

typedef struct {
    int id;
    char nome[100];
    char email[100];
} UserFile;

User* cabecalho_user;
User* proximo_user;

// window
void on_main_window_destroy(GtkWidget *widget, gpointer data) {
    gtk_main_quit(); // Comando para fechar um programa GTK
}


// Shared
void display_message(char text[100], char secondary_text[100], char icon_name[100]) {


    GtkMessageDialog *mensagem_dialogo = gtk_builder_get_object(builder, "mensagem");

    g_object_set(mensagem_dialogo, "text", text, NULL);
    g_object_set(mensagem_dialogo, "secondary_text", secondary_text, NULL);
    g_object_set(mensagem_dialogo, "icon_name", icon_name, NULL);

    // Mostra na tela
    gtk_widget_show_all(mensagem_dialogo);
    // Mantém a janela rodando
    gtk_dialog_run(mensagem_dialogo);
    // Esconde a janelinha
    gtk_widget_hide(mensagem_dialogo);
}
void login(char *email, char *senha, bool *lembrar) {
    if(strcmp(email, "admin") == 0 && strcmp(senha, "admin") == 0) {
        display_message("Bem vindo", "Usuario logado com sucesso!!!", "emblem-default");
        g_object_set(window, "icon_name", "avatar-default", NULL);
        gtk_stack_set_visible_child_name(stack, "view_inicial");
    } else {
        display_message("Aviso", "Email ou senha incorretos!", "dialog-error");
    }
}

// "view_login" stack
void on_button_login_clicked(GtkWidget *widget, gpointer data) {
    // variavel que aponta para o input com o nome email
    char *email = gtk_entry_get_text(gtk_builder_get_object(builder, "email"));
    char *senha = gtk_entry_get_text(gtk_builder_get_object(builder, "senha"));

    // Verifica se está ativo ou não
    bool *lembrar = gtk_toggle_button_get_active(gtk_builder_get_object(builder, "lembrar"));
    login(email, senha, lembrar);
}



// "view_inicial" stack
void on_button_cadastrar_inicial_clicked(GtkWidget *widget, gpointer data) {
    gtk_stack_set_visible_child_name(stack, "view_cadastro");
}
void on_button_listar_inicial_clicked(GtkWidget *widget, gpointer data) {
    gtk_stack_set_visible_child_name(stack, "view_listar");
}
void on_button_sair_inicial_clicked(GtkWidget *widget, gpointer data) {
    gtk_stack_set_visible_child_name(stack, "view_login");
    g_object_set(window, "icon_name", "changes-prevent", NULL);
}



// "view_cadastro" stack
void on_button_cad_voltar_clicked(GtkWidget *widget, gpointer data) {
    gtk_stack_set_visible_child_name(stack, "view_inicial");
}
// TO-DO: impedir cadastros repetidos
void on_button_cadastrar_clicked(GtkWidget *widget, gpointer data) {
    char *cad_nome = gtk_entry_get_text(gtk_builder_get_object(builder, "cad_nome"));
    char *cad_email = gtk_entry_get_text(gtk_builder_get_object(builder, "cad_email"));
    UserFile cad_user;
    int rand_id;


    if(strcmp(cad_nome, "") == 0) {
        display_message("Aviso!!!", "Campo 'Nome' obrigatorio!!", "dialog-error");
    } else {
        // Continua o cadastro
        // Retorna um número aleatório de 0 à 300
        rand_id = random_number(300);
        // proximo_user->id = id;
        // strcpy(proximo_user->nome, cad_nome);
        // strcpy(proximo_user->email, cad_email);
        cad_user.id = rand_id;
        strcpy(cad_user.nome, cad_nome);
        strcpy(cad_user.email, cad_email);
        writeFile("lista_de_usuarios.txt", &cad_user);

        char texto[100];
        // g_snprintf(texto, 100, "%s%s%s", "Usuario: ", cad_user.nome, " cadastrado!");
        // display_message("Aviso", texto, "dialog-emblem-default");

        // Aloca o espaço de memória para o próximo item da lista
        // proximo_user->prox = (User*) malloc(sizeof(User));
        // proximo_user = proximo_user->prox;
    }
}



// "view_listar" stack
void on_button_listar_voltar_clicked(GtkWidget *widget, gpointer data) {
    gtk_stack_set_visible_child_name(stack, "view_inicial");
}
void on_button_listar_clicked(GtkWidget *widget, gpointer data) {
    // Seta o proximo item da lista para null
    // proximo_user->prox = NULL;
    // E inicializo minha lista novamente
    // proximo_user = cabecalho_user;

    GtkTreeIter *iter;
    // Limpa o list storage para n repetir na UI
    gtk_list_store_clear(modelo_armazenamento);

    /*
    while(proximo_user->prox != NULL) {
         g_print("id: %d | nome: %s | email: %s\n",
            proximo_user->id,
            proximo_user->nome,
            proximo_user->email
        );

        // Somente adiciona uma linha na nossa lista
        gtk_list_store_append(modelo_armazenamento, &iter);
        // Atribui os dados nas colunas respectivas configuradas no glade anteriormente
        gtk_list_store_set(modelo_armazenamento, &iter,
            0, proximo_user->id,
            1, proximo_user->nome,
            2, proximo_user->email,
            -1
        );

        proximo_user = proximo_user->prox;
    }
    */

    FILE *arq;
    int result;

    arq = fopen("lista_de_usuarios.txt", "rt");

    if(arq == NULL) {
        display_message("Aviso", "Problemas na abertura do arquivo!", "dialog-error");
    } else {
        char linha[150];

        // Enquanto não chegar no final do arquivo
        while(!feof(arq)) {
            result = fgets(linha, 150, arq); // o 'fgets' lê até 99 caracteres ou até o '\n'
            if(result) { // foi possível ler a linha
                char *pt;
                int cont = 1;
                UserFile r_user;

                // Quebra a string retornando o ponteiro para o primeiro token
                pt = strtok(linha, "/\n");
                while(pt) {  //enquanto pt != NULL
                    // Para cada sequencia de aparição essa string deve ser armazenda em diferentes atributos do r_user
                    // printf("Token: %s\n", pt);
                    switch(cont) {
                        case 1:
                            r_user.id = atoi(pt);
                            break;
                        case 2:
                            strcpy(r_user.nome, pt);
                            break;
                        case 3:
                            strcpy(r_user.email, pt);
                            break;
                    }

                    // Retorna o ponteiro para o próximo token
                    pt = strtok(NULL,"/\n");
                    cont++;
                }
                // Acaba a linha
                // Armazenar no list_store
                // Somente adiciona uma linha na nossa lista
                gtk_list_store_append(modelo_armazenamento, &iter);
                // Atribui os dados nas colunas respectivas configuradas no glade anteriormente
                gtk_list_store_set(modelo_armazenamento, &iter,
                    0, r_user.id,
                    1, r_user.nome,
                    2, r_user.email,
                    -1
                );
            }
        }
    }
    fclose(arq);
}

int random_number(int max_number) {
    srand(time(NULL));
    return rand() % max_number;
}

void readFile(char *filename);
void writeFile(char *filename, UserFile *user);

int main(int argc, char *argv[]) {
    // Aloca memória para nossa lista encadeada
    cabecalho_user = (User*) malloc(sizeof(User));
    // Inicializa a lista
    proximo_user = cabecalho_user;

    gtk_init(&argc, &argv); // Inicia o GTK

    builder = gtk_builder_new_from_file("glade_ui.glade"); // Carrega nosso arquivo de UI criado no glade

    // Manipuladores de "sinais" ou funções
    gtk_builder_add_callback_symbols(
        builder,
        "on_main_window_destroy",               G_CALLBACK(on_main_window_destroy),
        "on_button_login_clicked",              G_CALLBACK(on_button_login_clicked),
        "on_button_cadastrar_inicial_clicked",  G_CALLBACK(on_button_cadastrar_inicial_clicked),
        "on_button_listar_inicial_clicked",     G_CALLBACK(on_button_listar_inicial_clicked),
        "on_button_sair_inicial_clicked",       G_CALLBACK(on_button_sair_inicial_clicked),
        "on_button_cad_voltar_clicked",         G_CALLBACK(on_button_cad_voltar_clicked),
        "on_button_cadastrar_clicked",          G_CALLBACK(on_button_cadastrar_clicked),
        "on_button_listar_voltar_clicked",      G_CALLBACK(on_button_listar_voltar_clicked),
        "on_button_listar_clicked",             G_CALLBACK(on_button_listar_clicked),
        NULL);

    gtk_builder_connect_signals(builder, NULL);

    // Atrubui a stack o ponteiro para o objeto stack do glade
    // Assim as outras funções podem utiliza-los
    stack = GTK_STACK(gtk_builder_get_object(builder, "stack"));
    window = GTK_WIDGET(gtk_builder_get_object(builder, "main_window"));
    modelo_armazenamento = GTK_LIST_STORE(gtk_builder_get_object(builder, "liststore1"));

    gtk_widget_show_all(window);
    gtk_main();
    return 0;
}

void writeFile(char *filename, UserFile *user) {
    FILE *arq;
    int result;

    // "wt" abertura para gravação, arquivo texto. Obs: sobrescreve
    // "rt" abertura para leitura, arquivo texto
    // "at" abertura para adicionar conteúdo, arquivo texto
    arq = fopen(filename, "a");

    if(arq == NULL) {
        display_message("Aviso", "Problemas na abertura do arquivo!", "dialog-error");
    } else {
        result = fprintf(arq, "%d/%s/%s\n", user->id, user->nome, user->email);

        if(result == EOF)
            display_message("Erro", "Erro no cadastro do usuario", "dialog-error");

        display_message("Sucesso", "Usuario cadastrado com sucesso!!!", "emblem-default");
    }
    fclose(arq);
}
