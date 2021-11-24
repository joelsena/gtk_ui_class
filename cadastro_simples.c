#include <gtk/gtk.h>
#include <stdlib.h>
#include <stdbool.h>

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

int id = 0;
User* cabecalho_user;
User* proximo_user;

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
void on_button_login_clicked(GtkWidget *widget, gpointer data) {
    // variavel que aponta para o input com o nome email
    char *email = gtk_entry_get_text(gtk_builder_get_object(builder, "email"));
    char *senha = gtk_entry_get_text(gtk_builder_get_object(builder, "senha"));

    // Verifica se está ativo ou não
    bool *lembrar = gtk_toggle_button_get_active(gtk_builder_get_object(builder, "lembrar"));
    login(email, senha, lembrar);
}

void on_main_window_destroy(GtkWidget *widget, gpointer data) {
    gtk_main_quit(); // Comando para fechar um programa GTK
}

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

void on_button_cad_voltar_clicked(GtkWidget *widget, gpointer data) {
    gtk_stack_set_visible_child_name(stack, "view_inicial");
}

void on_button_listar_voltar_clicked(GtkWidget *widget, gpointer data) {
    gtk_stack_set_visible_child_name(stack, "view_inicial");
}

// TO-DO: impedir cadastros repetidos
void on_button_cadastrar_clicked(GtkWidget *widget, gpointer data) {
    char *cad_nome = gtk_entry_get_text(gtk_builder_get_object(builder, "cad_nome"));
    char *cad_email = gtk_entry_get_text(gtk_builder_get_object(builder, "cad_email"));

    if(strcmp(cad_nome, "") == 0) {
        display_message("Aviso!!!", "Campo 'Nome' obrigatorio!!", "dialog-error");
    } else {
        // Continua o cadastro
        id++;
        proximo_user->id = id;
        strcpy(proximo_user->nome, cad_nome);
        strcpy(proximo_user->email, cad_email);

        char texto[100];
        g_snprintf(texto, 100, "%s%s%s", "Usuario: ", proximo_user->nome, " cadastrado!");
        display_message("Aviso", texto, "dialog-emblem-default");

        // Aloca o espaço de memória para o próximo item da lista
        proximo_user->prox = (User*) malloc(sizeof(User));
        proximo_user = proximo_user->prox;
    }
}

void on_button_listar_clicked(GtkWidget *widget, gpointer data) {
    // Seta o proximo item da lista para null
    proximo_user->prox = NULL;
    // E inicializo minha lista novamente
    proximo_user = cabecalho_user;

    GtkTreeIter *iter;
    // Limpa o list storage para n repetir na UI
    gtk_list_store_clear(modelo_armazenamento);

    while(proximo_user->prox != NULL) {
        /* g_print("id: %d | nome: %s | email: %s\n",
            proximo_user->id,
            proximo_user->nome,
            proximo_user->email
        ); */

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
}

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
