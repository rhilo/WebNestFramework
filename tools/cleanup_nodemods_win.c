#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <jansson.h> // For JSON handling



// Function prototypes
//void searchNodeModules();
void searchNodeModules() {
    char path[MAX_PATH];
    GetCurrentDirectory(MAX_PATH, path);
    printf("Searching for 'node_modules' folders in: %s\n", path);

    json_t *occurrences = json_array(); // JSON array to store occurrences

    // Recursive function to search for 'node_modules'
    searchNodeModulesRecursively(path, occurrences);

    // Write occurrences to JSON file
    FILE *file = fopen("occurrences.json", "w");
    if (file) {
        json_dumpf(occurrences, file, JSON_INDENT(4));
        fclose(file);
        printf("'node_modules' occurrences have been written to 'occurrences.json'.\n");
    } else {
        printf("Error writing to 'occurrences.json'.\n");
    }

    json_decref(occurrences); // Free JSON object
}

void searchNodeModulesRecursively(const char *path, json_t *occurrences) {
    // Implementation for recursive search
    // ...
}




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
