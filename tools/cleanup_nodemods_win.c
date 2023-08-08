#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <jansson.h> // For JSON handling
#include <shellapi.h> // For SHFileOperation    


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

//void searchAndDryRun();


void searchAndDryRun() {
    // Check if "occurrences.json" exists
    FILE *file = fopen("occurrences.json", "r");
    if (!file) {
        printf("'occurrences.json' not found. Please perform a search first.\n");
        return;
    }

    // Read "occurrences.json" into a JSON array
    json_error_t error;
    json_t *occurrences = json_loadf(file, 0, &error);
    fclose(file);

    if (!occurrences) {
        printf("Error reading 'occurrences.json': %s\n", error.text);
        return;
    }

    // Open "dryrun.log" for writing
    FILE *dryRunLog = fopen("dryrun.log", "w");
    if (!dryRunLog) {
        printf("Error opening 'dryrun.log' for writing.\n");
        json_decref(occurrences);
        return;
    }

    printf("Dry run deletion of 'node_modules' folders:\n");

    // Iterate through the occurrences and simulate deletion
    size_t index;
    json_t *value;
    json_array_foreach(occurrences, index, value) {
        const char *path = json_string_value(value);
        printf("Would delete: %s\n", path);
        fprintf(dryRunLog, "Would delete: %s\n", path);

        // Optionally, read and log additional information (e.g., yarn.lock)
        // ...
    }

    fclose(dryRunLog);
    json_decref(occurrences);

    printf("Dry run complete. See 'dryrun.log' for details.\n");
}



void searchAndDryRun() {
    // Check if "occurrences.json" exists
    FILE *file = fopen("occurrences.json", "r");
    if (!file) {
        printf("'occurrences.json' not found. Please perform a search first.\n");
        return;
    }

    // Read "occurrences.json" into a JSON array
    json_error_t error;
    json_t *occurrences = json_loadf(file, 0, &error);
    fclose(file);

    if (!occurrences) {
        printf("Error reading 'occurrences.json': %s\n", error.text);
        return;
    }

    // Open "dryrun.log" for writing
    FILE *dryRunLog = fopen("dryrun.log", "w");
    if (!dryRunLog) {
        printf("Error opening 'dryrun.log' for writing.\n");
        json_decref(occurrences);
        return;
    }

    printf("Dry run deletion of 'node_modules' folders:\n");

    // Iterate through the occurrences and simulate deletion
    size_t index;
    json_t *value;
    json_array_foreach(occurrences, index, value) {
        const char *path = json_string_value(value);
        printf("Would delete: %s\n", path);
        fprintf(dryRunLog, "Would delete: %s\n", path);

        // Optionally, read and log additional information (e.g., yarn.lock)
        // ...
    }

    fclose(dryRunLog);
    json_decref(occurrences);

    printf("Dry run complete. See 'dryrun.log' for details.\n");
}

//void deleteNodeModules();

void deleteNodeModules() {
    // Check if "occurrences.json" exists
    FILE *file = fopen("occurrences.json", "r");
    if (!file) {
        printf("'occurrences.json' not found. Please perform a search first.\n");
        return;
    }

    // Read "occurrences.json" into a JSON array
    json_error_t error;
    json_t *occurrences = json_loadf(file, 0, &error);
    fclose(file);

    if (!occurrences) {
        printf("Error reading 'occurrences.json': %s\n", error.text);
        return;
    }

    // Confirm deletion with the user
    char confirmation;
    printf("Are you sure you want to delete the 'node_modules' folders listed in 'occurrences.json'? (y/n): ");
    scanf(" %c", &confirmation);
    if (confirmation != 'y' && confirmation != 'Y') {
        printf("Deletion canceled.\n");
        json_decref(occurrences);
        return;
    }

    printf("Deleting 'node_modules' folders:\n");

    // Iterate through the occurrences and delete each directory
    size_t index;
    json_t *value;
    json_array_foreach(occurrences, index, value) {
        const char *path = json_string_value(value);

        // Check for permission issues
        if (!hasWritePermission(path)) {
            printf("Permission denied: %s\n", path);
            continue;
        }

        // Delete the directory
        SHFILEOPSTRUCT fileOp = { 0 };
        fileOp.wFunc = FO_DELETE;
        fileOp.pFrom = path;
        fileOp.fFlags = FOF_NOCONFIRMATION | FOF_NOERRORUI | FOF_SILENT;
        int result = SHFileOperation(&fileOp);

        if (result == 0) {
            printf("Deleted: %s\n", path);
        } else {
            printf("Error deleting: %s\n", path);
        }
    }

    json_decref(occurrences);

    printf("Deletion complete.\n");
}


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
