#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <jansson.h> // For JSON handling
#include <shellapi.h> // For SHFileOperation    
/**
 * Searches for all 'node_modules' folders recursively in the current directory and its subdirectories.
 * Stores the paths of the found folders in a JSON array and writes them to a file named 'occurrences.json'.
 * 
 * @param path The path of the directory to search in.
 * @param occurrences A pointer to a JSON array to store the paths of the found 'node_modules' folders.
 */



/**
 * Searches for all 'node_modules' folders recursively in the current directory and its subdirectories.
 * Stores the paths of the found folders in a JSON array and writes them to a file named 'occurrences.json'.
 * 
 * If 'occurrences.json' does not exist, prints an error message and returns.
 * If 'occurrences.json' cannot be read, prints an error message and returns.
 * If 'dryrun.log' cannot be opened for writing, prints an error message and returns.
 * 
 * Iterates through the paths of the found 'node_modules' folders and simulates their deletion by printing their paths to the console and to a file named 'dryrun.log'.
 * 
 * Optionally, reads and logs additional information (e.g., yarn.lock) for each 'node_modules' folder.
 */


/**
 * Searches for all 'node_modules' folders recursively in the current directory and its subdirectories.
 * Stores the paths of the found folders in a JSON array and writes them to a file named 'occurrences.json'.
 * 
 * Prints the paths of the found 'node_modules' folders to the console and writes them to a file named 'occurrences.json'.
 * 
 * If 'occurrences.json' cannot be written, prints an error message to the console.
 */
void searchNodeModules() {
    char path[MAX_PATH];
    GetCurrentDirectory(MAX_PATH, path);
    printf("Searching for 'node_modules' folders in: %s\n", path);
    json_t *occurrences = json_array();
    searchNodeModulesRecursively(path, occurrences);
    FILE *file = fopen("occurrences.json", "w");
    if (file) {
        json_dumpf(occurrences, file, JSON_INDENT(4));
        fclose(file);
        printf("'node_modules' occurrences have been written to 'occurrences.json'.\n");
    } else {
        printf("Error writing to 'occurrences.json'.\n");
    }
    json_decref(occurrences);
}

void searchNodeModulesRecursively(const char *path, json_t *occurrences) {
    WIN32_FIND_DATA findFileData;
    HANDLE hFind = FindFirstFile(strcat(path, "\\*"), &findFileData);
    if (hFind == INVALID_HANDLE_VALUE) return;
    do {
        if (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            if (strcmp(findFileData.cFileName, "node_modules") == 0) {
                char nodeModulesPath[MAX_PATH];
                snprintf(nodeModulesPath, sizeof(nodeModulesPath), "%s\\%s", path, findFileData.cFileName);
                printf("Found: %s\n", nodeModulesPath);
                json_array_append_new(occurrences, json_string(nodeModulesPath));
            } else if (strcmp(findFileData.cFileName, ".") != 0 && strcmp(findFileData.cFileName, "..") != 0) {
                char subPath[MAX_PATH];
                snprintf(subPath, sizeof(subPath), "%s\\%s", path, findFileData.cFileName);
                searchNodeModulesRecursively(subPath, occurrences);
            }
        }
    } while (FindNextFile(hFind, &findFileData) != 0);
    FindClose(hFind);
}
//void searchAndDryRun();
/**
 * Searches for all 'node_modules' folders recursively in the current directory and its subdirectories.
 * Stores the paths of the found folders in a JSON array and writes them to a file named 'occurrences.json'.
 * 
 * Prints the paths of the found 'node_modules' folders to the console and writes them to a file named 'occurrences.json'.
 * 
 * If 'occurrences.json' cannot be written, prints an error message to the console.
 */

/**
 * Searches for all 'node_modules' folders recursively in the current directory and its subdirectories.
 * Simulates the deletion of the found folders by printing their paths to the console and to a file named 'dryrun.log'.
 * 
 * Optionally, reads and logs additional information (e.g., yarn.lock) for each 'node_modules' folder.
 * 
 * If 'dryrun.log' cannot be opened for writing, prints an error message and returns.
 * 
 * If 'occurrences.json' cannot be read, prints an error message to the console and returns.
 * 
 * If 'occurrences.json' does not exist, prints an error message to the console and returns.
 */
void searchAndDryRun() {
    FILE *file = fopen("occurrences.json", "r");
    if (!file) {
        printf("'occurrences.json' not found. Please perform a search first.\n");
        return;
    }
    json_error_t error;
    json_t *occurrences = json_loadf(file, 0, &error);
    fclose(file);
    if (!occurrences) {
        printf("Error reading 'occurrences.json': %s\n", error.text);
        return;
    }
    FILE *dryRunLog = fopen("dryrun.log", "w");
    if (!dryRunLog) {
        printf("Error opening 'dryrun.log' for writing.\n");
        json_decref(occurrences);
        return;
    }
    printf("Dry run deletion of 'node_modules' folders:\n");
    size_t index;
    json_t *value;
    json_array_foreach(occurrences, index, value) {
        const char *path = json_string_value(value);
        printf("Would delete: %s\n", path);
        fprintf(dryRunLog, "Would delete: %s\n", path);
    }
    fclose(dryRunLog);
    json_decref(occurrences);
    printf("Dry run complete. See 'dryrun.log' for details.\n");
}
/**
 * Deletes all 'node_modules' folders listed in 'occurrences.json'.
 * Confirms deletion with the user before proceeding.
 * 
 * If 'occurrences.json' cannot be read, prints an error message to the console and returns.
 * 
 * If 'occurrences.json' does not exist, prints an error message to the console and returns.
 * 
 * If a 'node_modules' folder does not have write permission, prints an error message to the console and continues.
 * 
 * Deletes each 'node_modules' folder using the Windows API function SHFileOperation.
 * 
 * Prints the paths of the deleted 'node_modules' folders to the console.
 * 
 * If an error occurs during deletion, prints an error message to the console.
 */
void deleteNodeModules() {
    FILE *file = fopen("occurrences.json", "r");
    if (!file) {
        printf("'occurrences.json' not found. Please perform a search first.\n");
        return;
    }
    json_error_t error;
    json_t *occurrences = json_loadf(file, 0, &error);
    fclose(file);
    if (!occurrences) {
        printf("Error reading 'occurrences.json': %s\n", error.text);
        return;
    }
    char confirmation;
    printf("Are you sure you want to delete the 'node_modules' folders listed in 'occurrences.json'? (y/n): ");
    scanf(" %c", &confirmation);
    if (confirmation != 'y' && confirmation != 'Y') {
        printf("Deletion canceled.\n");
        json_decref(occurrences);
        return;
    }
    printf("Deleting 'node_modules' folders:\n");
    size_t index;
    json_t *value;
    json_array_foreach(occurrences, index, value) {
        const char *path = json_string_value(value);
        if (!hasWritePermission(path)) {
            printf("Permission denied: %s\n", path);
            continue;
        }
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
            case 1: searchNodeModules(); break;
            case 2: searchAndDryRun(); break;
            case 3: deleteNodeModules(); break;
            case 4: printf("Exiting program.\n"); break;
            default: printf("Invalid choice. Please try again.\n"); break;
        }
    } while (choice != 4);
    return 0;
}
