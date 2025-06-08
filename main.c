#include <gtk/gtk.h>
#include "tasks.h"

GtkWidget *listbox;
void on_task_toggled(GtkToggleButton *toggle_button, gpointer user_data);
void refresh_task_list() {
    gtk_container_foreach(GTK_CONTAINER(listbox), (GtkCallback)gtk_widget_destroy, NULL);

    for (int i = 0; i < task_count; i++) {
        char buffer[512];
        sprintf(buffer, "%s | %s | Dif: %d | %s",
                task_list[i].title,
                task_list[i].deadline,
                task_list[i].difficulty,
                task_list[i].description);

        GtkWidget *check = gtk_check_button_new_with_label(buffer);
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check), task_list[i].completed);

        g_signal_connect(check, "toggled", G_CALLBACK(on_task_toggled), GINT_TO_POINTER(i));

        gtk_list_box_insert(GTK_LIST_BOX(listbox), check, -1);
    }
    gtk_widget_show_all(listbox);
}
void on_task_toggled(GtkToggleButton *toggle_button, gpointer user_data) {
    int index = GPOINTER_TO_INT(user_data);
    task_list[index].completed = gtk_toggle_button_get_active(toggle_button);
    save_tasks(); 
    refresh_task_list();
}


void open_new_task_dialog(GtkWidget *widget, gpointer data);

void on_sort_alpha(GtkButton *btn, gpointer user_data) {
    sort_tasks_by_title();
    refresh_task_list();
}

void on_sort_deadline(GtkButton *btn, gpointer user_data) {
    sort_tasks_by_deadline();
    refresh_task_list();
}

void on_sort_difficulty(GtkButton *btn, gpointer user_data) {
    sort_tasks_by_difficulty();
    refresh_task_list();
}

int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);
    
    load_tasks();

    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Gerenciador de Tarefas");
    gtk_window_set_default_size(GTK_WINDOW(window), 500, 400);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);

    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER(window), vbox);

    GtkWidget *btn_new_task = gtk_button_new_with_label("Nova Tarefa");
    GtkWidget *hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);

    g_signal_connect(btn_new_task, "clicked", G_CALLBACK(open_new_task_dialog), window);
    gtk_box_pack_start(GTK_BOX(vbox), btn_new_task, FALSE, FALSE, 0);


    GtkWidget *btn_alpha = gtk_button_new_with_label("Ordenar A-Z");
    GtkWidget *btn_deadline = gtk_button_new_with_label("Por Data");
    GtkWidget *btn_difficulty = gtk_button_new_with_label("Por Dificuldade");

    g_signal_connect(btn_alpha, "clicked", G_CALLBACK(on_sort_alpha), NULL);
    g_signal_connect(btn_deadline, "clicked", G_CALLBACK(on_sort_deadline), NULL);
    g_signal_connect(btn_difficulty, "clicked", G_CALLBACK(on_sort_difficulty), NULL);

    gtk_box_pack_start(GTK_BOX(hbox), btn_alpha, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(hbox), btn_deadline, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(hbox), btn_difficulty, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 0);

    listbox = gtk_list_box_new();
    gtk_box_pack_start(GTK_BOX(vbox), listbox, TRUE, TRUE, 0);

    refresh_task_list();
    gtk_widget_show_all(window);
    gtk_main();
    return 0;
}
void open_new_task_dialog(GtkWidget *widget, gpointer data) {
    GtkWidget *dialog = gtk_dialog_new_with_buttons(
        "Nova Tarefa", GTK_WINDOW(data), 
        GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
        "_Cancelar", GTK_RESPONSE_CANCEL,
        "_Salvar", GTK_RESPONSE_ACCEPT,
        NULL
    );

    GtkWidget *content = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    GtkWidget *grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid), 10);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 10);
    gtk_container_set_border_width(GTK_CONTAINER(grid), 15);

    GtkWidget *entry_title = gtk_entry_new();
    GtkWidget *entry_description = gtk_entry_new();
    GtkWidget *calendar = gtk_calendar_new();
    GtkWidget *spin_difficulty = gtk_spin_button_new_with_range(1, 5, 1);

    gtk_entry_set_placeholder_text(GTK_ENTRY(entry_title), "Ex: Estudar para prova");
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry_description), "Breve descrição da tarefa");

    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("Título:"), 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), entry_title, 1, 0, 1, 1);

    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("Descrição:"), 0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), entry_description, 1, 1, 1, 1);

    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("Prazo:"), 0, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), calendar, 1, 2, 1, 1);

    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("Dificuldade (1–5):"), 0, 3, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), spin_difficulty, 1, 3, 1, 1);

    gtk_container_add(GTK_CONTAINER(content), grid);
    gtk_widget_show_all(dialog);

    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
        Task new_task;

        // Título e descrição
        strncpy(new_task.title, gtk_entry_get_text(GTK_ENTRY(entry_title)), sizeof(new_task.title));
        strncpy(new_task.description, gtk_entry_get_text(GTK_ENTRY(entry_description)), sizeof(new_task.description));

        // Obtem a data do calendário e formata como YYYY-MM-DD
        guint year, month, day;
        gtk_calendar_get_date(GTK_CALENDAR(calendar), &year, &month, &day);
        snprintf(new_task.deadline, sizeof(new_task.deadline), "%04d-%02d-%02d", year, month + 1, day); // mês começa em 0

        new_task.difficulty = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(spin_difficulty));
        new_task.completed = 0;

        add_task(new_task);
        refresh_task_list();
    }

    gtk_widget_destroy(dialog);
}
