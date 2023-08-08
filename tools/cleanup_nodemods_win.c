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
    WIN32_FIND_DATA findFileData;
    HANDLE hFind = FindFirstFile(strcat(path, "\\*"), &findFileData);

    if (hFind == INVALID_HANDLE_VALUE) {
        return;
    } else {
        do {
            if (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                // Check if the directory is named "node_modules"
                if (strcmp(findFileData.cFileName, "node_modules") == 0) {
                    // Construct the full path to the "node_modules" directory
                    char nodeModulesPath[MAX_PATH];
                    snprintf(nodeModulesPath, sizeof(nodeModulesPath), "%s\\%s", path, findFileData.cFileName);

                    // Log to console
                    printf("Found: %s\n", nodeModulesPath);

                    // Add to JSON array
                    json_array_append_new(occurrences, json_string(nodeModulesPath));
                } else if (strcmp(findFileData.cFileName, ".") != 0 && strcmp(findFileData.cFileName, "..") != 0) {
                    // Recursively search in subdirectories
                    char subPath[MAX_PATH];
                    snprintf(subPath, sizeof(subPath), "%s\\%s", path, findFileData.cFileName);
                    searchNodeModulesRecursively(subPath, occurrences);
                }
            }
        } while (FindNextFile(hFind, &findFileData) != 0);
        FindClose(hFind);
    }
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
