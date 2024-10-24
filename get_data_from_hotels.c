// get_data_from_hotels.c

#include "project_header.h"
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <float.h>

struct HotelBucket destinationHash[DESTINATION_HASH_SIZE];

int hashDestination(const char* destination) {
    int hash = 0;
    while (*destination) {
        hash = (hash * 31) + *destination;
        destination++;
    }
    return abs(hash % DESTINATION_HASH_SIZE);
}

void insertHotel(const struct HotelInfo* hotel) {
    int hashIndex = hashDestination(hotel->place);

    struct HotelNode* newNode = (struct HotelNode*)malloc(sizeof(struct HotelNode));
    newNode->hotel = *hotel;
    newNode->next = destinationHash[hashIndex].head;

    destinationHash[hashIndex].head = newNode;
}

struct HotelInfo* findCheapestHotel(const char* destination) {
    int hashIndex = hashDestination(destination);

    struct HotelNode* current = destinationHash[hashIndex].head;
    struct HotelInfo* cheapestHotel = NULL;
    float minPrice = FLT_MAX;

   /* printf("Searching for hotels in %s:\n", destination);*/

    while (current != NULL) {
        printf("Checking hotel in %s\n", current->hotel.place);

        if (strcasecmp(current->hotel.place, destination) == 0 && current->hotel.price < minPrice) {
            cheapestHotel = &current->hotel;
            minPrice = current->hotel.price;
        }
        current = current->next;
    }

    return cheapestHotel;
}

int readHotelsData(struct HotelInfo hotels[], const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error opening file: %s\n", filename);
        return -1;
    }

    int count = 0;
    char line[256];
    while (fgets(line, sizeof(line), file)) {
        char *name = strtok(line, ",");
        char *place = strtok(NULL, ",");
        char *priceStr = strtok(NULL, ",");

        // Check for NULL pointers
        if (name != NULL && place != NULL && priceStr != NULL) {
            // Remove leading whitespace from name
            while (*name == ' ' || *name == '\t' || *name == '\n') {
                name++;
            }

            // Remove leading whitespace from place
            while (*place == ' ' || *place == '\t' || *place == '\n') {
                place++;
            }

            // Convert price to float
            hotels[count].price = atof(priceStr);

            // Copy values to the structure
            strncpy(hotels[count].name, name, sizeof(hotels[count].name) - 1);
            hotels[count].name[sizeof(hotels[count].name) - 1] = '\0';

            strncpy(hotels[count].place, place, sizeof(hotels[count].place) - 1);
            hotels[count].place[sizeof(hotels[count].place) - 1] = '\0';

            // Insert hotel into the hash table
            insertHotel(&hotels[count]);

            count++;

            // Check for MAX_HOTELS limit
            if (count >= MAX_HOTELS) {
                printf("Warning: Maximum hotel limit reached. Increase MAX_HOTELS if needed.\n");
                break;
            }
        }
    }

    fclose(file);

    // You can decide whether exceeding MAX_HOTELS is an error or a warning.
    if (count >= MAX_HOTELS) {
        return -1;  // Indicate an error if MAX_HOTELS is exceeded.
    }

    if (count == 0) {
        printf("No valid hotel data found in file.\n");
        return -1;
    }

    return count;
}
