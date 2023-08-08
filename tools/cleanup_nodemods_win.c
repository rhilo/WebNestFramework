#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <jansson.h> // For JSON handling



// Function prototypes
void searchNodeModules();
void searchAndDryRun();
void deleteNodeModules();

int main() {
    int choice;

    do {
        printf("Please choose an option:\n");
        printf("1. Search for 'node_modules' folders\n");
        printf("2. Search and dry run deletion\n");
        printf("3. Delete 'node_modules' folders\n");
        printf("4. Exit\n");
        printf("Choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                searchNodeModules();
                break;
            case 2:
                searchAndDryRun();
                break;
            case 3:
                deleteNodeModules();
                break;
            case 4:
                printf("Exiting program.\n");
                break;
            default:
                printf("Invalid choice. Please try again.\n");
                break;
        }
    } while (choice != 4);

    return 0;
}
