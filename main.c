#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "project_header.h"
#include <gtk/gtk.h>

#define MAX_INPUT_LEN 100

typedef struct {
    GtkWidget *window;
    GtkWidget *places_combobox;
    GtkWidget *start_entry;
    GtkWidget *end_entry;
    GtkWidget *paths_listbox;
    GtkWidget *hotels_listbox;
    GtkWidget *cheapest_hotel_label;
} ApplicationWindow;

void display_places_gui(char places[MAX_ROWS][MAX_LEN], int unique_places_rows, ApplicationWindow *win) {
    GtkListStore *store;
    GtkTreeIter iter;

    store = gtk_list_store_new(1, G_TYPE_STRING);

    for (int i = 0; i < unique_places_rows; i++) {
        gtk_list_store_append(store, &iter);
        gtk_list_store_set(store, &iter, 0, places[i], -1);
    }

    gtk_combo_box_set_model(GTK_COMBO_BOX(win->places_combobox), GTK_TREE_MODEL(store));
    g_object_unref(store);
}

void find_paths_button_clicked(GtkWidget *button, gpointer user_data) {
    ApplicationWindow *win = user_data;

    char start[MAX_LEN];
    char end[MAX_LEN];

    strcpy(start, gtk_entry_get_text(GTK_ENTRY(win->start_entry)));
    strcpy(end, gtk_entry_get_text(GTK_ENTRY(win->end_entry)));

    // Call the necessary functions to find paths, sort them, and display the results
    int no_of_input_rows;
    struct FlightInfo data[MAX_ROWS];
    int unique_places_rows = 0;
    char places[MAX_ROWS][MAX_LEN];
    struct adjacency_matrix_data adj[MAX_ROWS][MAX_ROWS];
    struct allpaths *all_paths_src_dst = NULL;

    no_of_input_rows = get_flight_data(data, MAX_ROWS);
    if (no_of_input_rows == -1) {
        printf("Error reading flight data.\n");
        return;
    }

    unique_places_rows = find_all_unique_places(data, places, no_of_input_rows);
    display_places_gui(places, unique_places_rows, win);

    create_adjacency_matrix(data, adj, no_of_input_rows, places, unique_places_rows);

    int allpaths_row = find_path(start, end, unique_places_rows, &all_paths_src_dst, adj, places);
    if (allpaths_row == -1) {
        printf("Error finding paths.\n");
        return;
    }

    find_total_cost_duration(all_paths_src_dst, allpaths_row, adj);

    gtk_list_box_clear(GTK_LIST_BOX(win->paths_listbox));
    gtk_list_box_clear(GTK_LIST_BOX(win->hotels_listbox));

    for (int i = 0; i < allpaths_row; i++) {
        struct allpaths *path = &all_paths_src_dst[i];

        GtkWidget *list_item = gtk_list_box_row_new();
        gtk_list_box_insert(GTK_LIST_BOX(win->paths_listbox), list_item, -1);

        GtkBox *hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
        gtk_container_add(GTK_CONTAINER(list_item), hbox);

        GtkLabel *duration_label = gtk_label_new("Duration:");
        gtk_box_pack_start(GTK_BOX(hbox), duration_label, FALSE, FALSE, 0);

        int hours = path->duration / 60;
        int minutes = path->duration % 60;
        char duration_text[100];
        snprintf(duration_text, sizeof(duration_text), "%d:%02d", hours, minutes);
        GtkLabel *duration_value_label = gtk_label_new(duration_text);
        gtk_box_pack_start(GTK_BOX(hbox), duration_value_label, FALSE, FALSE, 0);

        GtkLabel *cost_label = gtk_label_new("Cost:");
        gtk_box_pack_start(GTK_BOX(hbox), cost_label, FALSE, FALSE, 0);

        char cost_text[100];
        snprintf(cost_text, sizeof(cost_text), "%.2f", path->cost);
        GtkLabel *cost_value_label = gtk_label_new(cost_text);
        gtk_box_pack_start(GTK_BOX(hbox), cost_value_label, FALSE, FALSE, 0);

        GtkLabel *path_label = gtk_label_new("Path:");
        gtk_box_pack_start(GTK_BOX(hbox), path_label, FALSE, FALSE, 0);

        for (int j = 0; j < path->path_len; j++) {
            char path_info[100];
            snprintf(path_info, sizeof(path_info), "%s -> ", places[path->path[j]]);
            GtkLabel *path_element_label = gtk_label_new(path_info);
            gtk_box_pack_start(GTK_BOX(hbox), path_element_label, FALSE, FALSE, 0);
        }

        // Add hotel information if needed
        struct HotelInfo *hotel = findCheapestHotel(end);
        if (hotel != NULL) {
            GtkWidget *hotel_list_item = gtk_list_box_row_new();
            gtk_list_box_insert(GTK_LIST_BOX(win->hotels_listbox), hotel_list_item, -1);

            GtkLabel *hotel_name_label = gtk_label_new(hotel->name);
            gtk_container_add(GTK_CONTAINER(hotel_list_item), hotel_name_label);
        } else {
            GtkWidget *no_hotel_label = gtk_label_new("No hotels found in the destination.");
            gtk_container_add(GTK_CONTAINER(win->hotels_listbox), no_hotel_label);
        }
    }

    // Clean up dynamically allocated memory if any (e.g., all_paths_src_dst)

    // Sort paths based on duration or cost
    int sort_type;
    printf("Enter the sort type (1 for duration, 2 for cost): ");
    if (scanf("%d", &sort_type) == 1) {
        switch (sort_type) {
            case 1:
                heapSortByDuration(all_paths_src_dst, allpaths_row, places);
                break;
            case 2:
                heapSortByCost(all_paths_src_dst, allpaths_row, places);
                break;
            default:
                printf("Invalid sort type.\n");
                return;
        }
    } else {
        printf("Invalid input for sort type.\n");
        return;
    }

    // Rebuild the path list box after sorting
    GList *children = gtk_container_get_children(GTK_CONTAINER(win->paths_listbox));
    g_list_free_full(children, gtk_widget_destroy);

    for (int i = 0; i < allpaths_row; i++) {
        struct allpaths *path = &all_paths_src_dst[i];

        GtkWidget *list_item = gtk_list_box_row_new();
        gtk_list_box_insert(GTK_LIST_BOX(win->paths_listbox), list_item, -1);

        GtkBox *hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
        gtk_container_add(GTK_CONTAINER(list_item), hbox);

        GtkLabel *duration_label = gtk_label_new("Duration:");
        gtk_box_pack_start(GTK_BOX(hbox), duration_label, FALSE, FALSE, 0);

        int hours = path->duration / 60;
        int minutes = path->duration % 60;
        char duration_text[100];
        snprintf(duration_text, sizeof(duration_text), "%d:%02d", hours, minutes);
        GtkLabel *duration_value_label = gtk_label_new(duration_text);
        gtk_box_pack_start(GTK_BOX(hbox), duration_value_label, FALSE, FALSE, 0);

        GtkLabel *cost_label = gtk_label_new("Cost:");
        gtk_box_pack_start(GTK_BOX(hbox), cost_label, FALSE, FALSE, 0);

        char cost_text[100];
        snprintf(cost_text, sizeof(cost_text), "%.2f", path->cost);
        GtkLabel *cost_value_label = gtk_label_new(cost_text);
        gtk_box_pack_start(GTK_BOX(hbox), cost_value_label, FALSE, FALSE, 0);

        GtkLabel *path_label = gtk_label_new("Path:");
        gtk_box_pack_start(GTK_BOX(hbox), path_label, FALSE, FALSE, 0);

        for (int j = 0; j < path->path_len; j++) {
            char path_info[100];
            snprintf(path_info, sizeof(path_info), "%s -> ", places[path->path[j]]);
            GtkLabel *path_element_label = gtk_label_new(path_info);
            gtk_box_pack_start(GTK_BOX(hbox), path_element_label, FALSE, FALSE, 0);
        }

        // Add hotel information if needed
        struct HotelInfo *hotel = findCheapestHotel(end);
        if (hotel != NULL) {
            GtkWidget *hotel_list_item = gtk_list_box_row_new();
            gtk_list_box_insert(GTK_LIST_BOX(win->hotels_listbox), hotel_list_item, -1);

            GtkLabel *hotel_name_label = gtk_label_new(hotel->name);
            gtk_container_add(GTK_CONTAINER(hotel_list_item), hotel_name_label);
        } else {
            GtkWidget *no_hotel_label = gtk_label_new("No hotels found in the destination.");
            gtk_container_add(GTK_CONTAINER(win->hotels_listbox), no_hotel_label);
        }
    }
}

int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);

    ApplicationWindow win;
    memset(&win, 0, sizeof(ApplicationWindow));

    win.window = gtk_application_window_new(NULL);
    gtk_window_set_title(GTK_WINDOW(win.window), "Flight and Hotel Booking System");
    gtk_window_set_default_size(GTK_WINDOW(win.window), 800, 600);

    GtkBox *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER(win.window), vbox);

    GtkGrid *grid = gtk_grid_new();
    gtk_box_pack_start(GTK_BOX(vbox), grid, FALSE, FALSE, 0);

    GtkLabel *places_label = gtk_label_new("Places:");
    gtk_grid_attach(GTK_GRID(grid), places_label, 0, 0, 1, 1);

    win.places_combobox = gtk_combo_box_text_new();
    gtk_grid_attach(GTK_GRID(grid), win.places_combobox, 1, 0, 1, 1);

    GtkLabel *start_label = gtk_label_new("Start:");
    gtk_grid_attach(GTK_GRID(grid), start_label, 0, 1, 1, 1);

    win.start_entry = gtk_entry_new();
    gtk_grid_attach(GTK_GRID(grid), win.start_entry, 1, 1, 1, 1);

    GtkLabel *end_label = gtk_label_new("End:");
    gtk_grid_attach(GTK_GRID(grid), end_label, 0, 2, 1, 1);

    win.end_entry = gtk_entry_new();
    gtk_grid_attach(GTK_GRID(grid), win.end_entry, 1, 2, 1, 1);

    GtkButton *find_paths_button = gtk_button_new_with_label("Find Paths");
    g_signal_connect(find_paths_button, "clicked", G_CALLBACK(find_paths_button_clicked), &win);
    gtk_box_pack_start(GTK_BOX(vbox), find_paths_button, FALSE, FALSE, 0);

    GtkScrolledWindow *paths_scrolled_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_box_pack_start(GTK_BOX(vbox), paths_scrolled_window, TRUE, TRUE, 0);

    win.paths_listbox = gtk_list_box_new();
    gtk_container_add(GTK_CONTAINER(paths_scrolled_window), win.paths_listbox);

    GtkScrolledWindow *hotels_scrolled_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_box_pack_start(GTK_BOX(vbox), hotels_scrolled_window, TRUE, TRUE, 0);

    win.hotels_listbox = gtk_list_box_new();
    gtk_container_add(GTK_CONTAINER(hotels_scrolled_window), win.hotels_listbox);

    GtkLabel *cheapest_hotel_label = gtk_label_new("Cheapest Hotel:");
    gtk_box_pack_start(GTK_BOX(vbox), cheapest_hotel_label, FALSE, FALSE, 0);

    gtk_widget_show_all(win.window);

    g_signal_connect(win.window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    gtk_main();

    return 0;
}
