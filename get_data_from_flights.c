#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "project_header.h"

int get_flight_data(struct FlightInfo *flights, int maxFlightCount) {
    FILE *file = fopen("flights.csv", "r");
    if (file == NULL) {
        printf("Error opening file.\n");
        return 1;
    }


    int flightCount = 0; 
    char line[256]; 
    while (fgets(line, sizeof(line), file)) {
        char *src = strtok(line, ",");
        char *dest = strtok(NULL, ",");
        char *flightType = strtok(NULL, ",");  
        char *priceStr = strtok(NULL, ",");  
        char *durationStr = strtok(NULL, ",");  
        char *distanceStr = strtok(NULL, ",");

        if (src != NULL && dest != NULL && flightType != NULL && priceStr != NULL && durationStr != NULL && distanceStr != NULL) {
            strcpy(flights[flightCount].source, src);
            strcpy(flights[flightCount].destination, dest);
            strcpy(flights[flightCount].flightType, flightType);
            flights[flightCount].price = atof(priceStr);

            int hours, minutes;
            sscanf(durationStr, "%dh %dm", &hours, &minutes);

            flights[flightCount].duration = (hours * 60) + minutes;
            flights[flightCount].distance = atoi(distanceStr);
            flightCount++;
        }

        if (flightCount >= maxFlightCount) {
            printf("Too many flights. Increase the array size.\n");
            fclose(file);
            return 1;
        }
    }

    fclose(file);

    if (flightCount == 0) {
        printf("No valid data found in file.\n");
        return -1;
    }

#if 0   
    printf("Source: %s\n", flights[5].source);
    printf("Destination: %s\n", flights[5].destination);
    printf("Flight Type: %s\n", flights[5].flightType);
    printf("Price: %.2f\n", flights[5].price);

    
    int hours = flights[5].duration / 60;
    int minutes = flights[5].duration % 60;

    printf("Duration: %d hours %d minutes\n", hours, minutes);

    printf("Distance: %d\n", flights[5].distance);
#endif
    return flightCount;
}
