#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

// Define a semaphore for parking spots
sem_t parking_spots;
int total_spots = 10; // Total number of parking spots

// Function to simulate parking a car
void* park(void* arg) {
    int car_id = *((int*)arg);
    printf("Car %d trying to park...\n", car_id);
    sem_wait(&parking_spots); // Decrement the semaphore (wait for a spot)
    printf("Car %d parked. Spots left: %d\n", car_id, --total_spots);
    sleep(2); // Simulate parking time
    printf("Car %d leaving...\n", car_id);
    printf("Spots available: %d\n", ++total_spots);
    sem_post(&parking_spots); // Increment the semaphore (leave the spot)
    return NULL;
}

int main() {
    pthread_t cars[15]; // Thread identifiers for cars
    int car_ids[15]; // Car IDs

    sem_init(&parking_spots, 0, 10); // Initialize the semaphore with 10 spots

    // Create threads for cars trying to park
    for (int i = 0; i < 15; i++) {
        car_ids[i] = i + 1;
        pthread_create(&cars[i], NULL, park, &car_ids[i]);
    }

    // Wait for all car threads to finish
    for (int i = 0; i < 15; i++) {
        pthread_join(cars[i], NULL);
    }

    sem_destroy(&parking_spots); // Destroy the semaphore
    return 0;
}
//ALL CODE DONE BY JAKIN ROGEL