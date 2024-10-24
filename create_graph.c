#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "project_header.h"

int search_place(char places[MAX_ROWS][MAX_LEN], char place[MAX_LEN], int rows)
{
    int i;

    for(i=0; i<rows; i++) {
        if(strcmp(place, places[i]) == 0) {
            return i;
        }
    }

    return -1;
}

int find_all_unique_places(struct FlightInfo *data, char places[MAX_ROWS][MAX_LEN], int no_of_rows)
{
    int i, j, rows = 0, exists = 0;

    for(i=1; i<no_of_rows; i++) {
        for(j=0; j<rows; j++) {
            if(strcmp(data[i].source, places[j]) == 0) {
                exists = 1;
                break;
            }
        }
        if(exists == 0) {
            strcpy(places[j], data[i].source);
            rows++;
        }

        exists = 0;
    }

    return rows;
}

void display_places(char places[MAX_ROWS][MAX_LEN], int rows)
{
    int i, j;

    for(i=0; i<rows; i++) {
        printf("[%d] %s\n", i, places[i]);
    }
}
void display_adj(struct adjacency_matrix_data adj[MAX_ROWS][MAX_ROWS], int rows)
{
    int i, j;

    for(i=0; i<rows; i++) {
        for(j=0; j<rows; j++) {
            printf("[%.2f,%d] ", adj[i][j].price, adj[i][j].duration);
        }
        printf("\n");
    }
}

void display_paths_total_cost_duration(struct allpaths *all_paths_src_dst, int allpaths_rows)
{
    int i, j;
    for(int i=0; i<allpaths_rows; i++) {
        for(j=0; j<all_paths_src_dst[i].path_len; j++){
            printf("%d\t", all_paths_src_dst[i].path[j]);
        }
        printf("\n");
        printf("total price = %.2f, total duration = %d\n", all_paths_src_dst[i].cost, all_paths_src_dst[i].duration);
    }
    printf("\n");
}

void create_adjacency_matrix(struct FlightInfo *data, struct adjacency_matrix_data adj[MAX_ROWS][MAX_ROWS], int no_of_input_rows,
                             char places[MAX_ROWS][MAX_LEN], int unique_places_rows)
{   
    int i, j, pos_src, pos_dst;
    char src[MAX_LEN], dst[MAX_LEN];

    for(i=1; i<no_of_input_rows; i++) {
        strcpy(src, data[i].source);
        strcpy(dst, data[i].destination);

        pos_src = search_place(places, src, unique_places_rows);

        pos_dst = search_place(places, dst, unique_places_rows);
        assert(pos_src != pos_dst);
        adj[pos_src][pos_dst].price = data[i].price;
        adj[pos_src][pos_dst].duration = data[i].duration;
    }
}

int  find_all_paths(struct adjacency_matrix_data adj[MAX_ROWS][MAX_ROWS],
                    int pos_start,
                    int pos_end,
                    int unique_places_rows,
                    int path[],
                    struct allpaths **all_paths_src_dst,
                    int path_index,
                    int *allpaths_row,
                    int visited[])
{
    int i;

    visited[pos_start] = 1;
    path[path_index] = pos_start;

    if(pos_start == pos_end) {
        if((*allpaths_row) % 100 == 0) {
            struct allpaths *temp = realloc(*all_paths_src_dst, sizeof(struct allpaths) * ((*allpaths_row)+100));
            if (temp == NULL) {
                return -1;
            }
            *all_paths_src_dst = temp;
        }
        
        memcpy((*all_paths_src_dst)[*allpaths_row].path, path, (path_index+1)*(sizeof(int)));
        (*all_paths_src_dst)[*allpaths_row].path_len = path_index+1;
        //printf("pos_start = %d, pos_end = %d path_index = %d allpaths_row = %d\n", pos_start, pos_end, path_index, *allpaths_row);
        /*for(int i=0; i<path_index+1; i++) {
            printf("%d\t", path[i]);
        }
        printf("\n");*/

        (*allpaths_row)++;
        visited[pos_start] = 0;
        return 0;
    }

    for(i=0; i<unique_places_rows; i++) {
        if(adj[pos_start][i].price != 0) {
            if (visited[i] != 1) {
                if (find_all_paths(adj, i, pos_end, unique_places_rows,
                                   path, all_paths_src_dst, path_index+1,
                                   allpaths_row, visited) == -1) {
                    return -1;
                }
            }
        }
    }

    visited[pos_start] = 0;
    return 0;
}

int find_path(char start[MAX_LEN], char end[MAX_LEN], int unique_places_rows,
             struct allpaths **all_paths_src_dst, struct adjacency_matrix_data adj[MAX_ROWS][MAX_ROWS], char places[MAX_ROWS][MAX_LEN])
{
    int pos_start, pos_end, path_index = 0, allpaths_row = 0;
    int path[MAX_ROWS];
    int visited[unique_places_rows];

    memset(visited, 0, sizeof(visited));
    pos_start = search_place(places, start, unique_places_rows);
    pos_end = search_place(places, end, unique_places_rows);

    //printf("pos_start = %d, pos_end = %d\n", pos_start, pos_end);

    if(pos_start == -1 || pos_end == -1) {
        return -1;
    }

    if (find_all_paths(adj, pos_start, pos_end,
                       unique_places_rows, path,
                       all_paths_src_dst, path_index,
                       &allpaths_row, visited) == -1) {
        return -1;
    }

    printf("end of find_path func.\n");

    return allpaths_row;
}

void find_total_cost_duration(struct allpaths *all_paths_src_dst, int allpaths_rows, struct adjacency_matrix_data adj[MAX_ROWS][MAX_ROWS])
{
    int i, j, index1, index2;

    for(i=0; i<allpaths_rows; i++) {
        int total_duration=0;
        float total_price=0;

        //printf("before j loop.\n");

        for(j=0; j<all_paths_src_dst[i].path_len - 1; j++) {
            index1 = all_paths_src_dst[i].path[j];
            index2 = all_paths_src_dst[i].path[j+1];

            total_price = total_price + adj[index1][index2].price;
            total_duration = total_duration + adj[index1][index2].duration;
            //printf("j = %d\n", j);
        }

        (all_paths_src_dst)[i].cost = total_price;
        (all_paths_src_dst)[i].duration = total_duration;

        //printf("(*all_paths_src_dst)[%d]->cost = %f, (*all_paths_src_dst)[%d]->duration = %d\n", i, (all_paths_src_dst)[i].cost, i, (all_paths_src_dst)[i].duration);
    }

    display_paths_total_cost_duration(all_paths_src_dst, allpaths_rows);
}
