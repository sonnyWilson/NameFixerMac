#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <limits.h>

#define MAX_MATCHES 1024

typedef struct {
    char dir_path[PATH_MAX];
    char old_name[NAME_MAX];
} FileItem;

FileItem matches[MAX_MATCHES];
int match_count = 0;

// Helper to replace character occurrences in a string
void replace_char(const char *src, char *dest, char target, char replacement) {
    int i = 0;
    while (src[i] != '\0') {
        if (src[i] == target) {
            dest[i] = replacement;
        } else {
            dest[i] = src[i];
        }
        i++;
    }
    dest[i] = '\0';
}

// Recursively traverse directory tree
void search_directory(const char *base_path, char target_char) {
    DIR *dir = opendir(base_path);
    if (!dir) return;

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        // Skip current (.) and parent (..) directories
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        char full_path[PATH_MAX];
        snprintf(full_path, sizeof(full_path), "%s/%s", base_path, entry->d_name);

        struct stat path_stat;
        if (stat(full_path, &path_stat) != 0) continue;

        if (S_ISREG(path_stat.st_mode)) { // Regular file
            if (strchr(entry->d_name, target_char) != NULL) {
                if (match_count < MAX_MATCHES) {
                    strncpy(matches[match_count].dir_path, base_path, PATH_MAX - 1);
                    strncpy(matches[match_count].old_name, entry->d_name, NAME_MAX - 1);
                    match_count++;
                }
            }
        } else if (S_ISDIR(path_stat.st_mode)) { // Subdirectory
            search_directory(full_path, target_char);
        }
    }
    closedir(dir);
}

int main(void) {
    char dir_input[PATH_MAX];
    char target_char, replacement_char;

    printf("Enter directory path (press Enter for current folder '.'): ");
    if (fgets(dir_input, sizeof(dir_input), stdin)) {
        dir_input[strcspn(dir_input, "\n")] = 0; // Strip newline
    }

    if (strlen(dir_input) == 0) {
        strcpy(dir_input, ".");
    }

    printf("Enter target character to replace: ");
    scanf(" %c", &target_char);

    printf("Enter replacement character: ");
    scanf(" %c", &replacement_char);

    search_directory(dir_input, target_char);

    if (match_count == 0) {
        printf("\nNo files found containing '%c'.\n", target_char);
        return 0;
    }

    printf("\nFound %d matching file(s):\n", match_count);
    for (int i = 0; i < match_count; i++) {
        printf(" - %s/%s\n", matches[i].dir_path, matches[i].old_name);
    }

    // Dry Run Preview
    printf("\n--- Preview of Changes ---\n");
    for (int i = 0; i < match_count; i++) {
        char new_name[NAME_MAX];
        replace_char(matches[i].old_name, new_name, target_char, replacement_char);
        printf("[PREVIEW] %s/%s  -->  %s\n", matches[i].dir_path, matches[i].old_name, new_name);
    }

    // Confirmation
    char confirm;
    printf("\nProceed with renaming? (y/n): ");
    scanf(" %c", &confirm);

    if (confirm == 'y' || confirm == 'Y') {
        int success_count = 0;
        for (int i = 0; i < match_count; i++) {
            char new_name[NAME_MAX];
            replace_char(matches[i].old_name, new_name, target_char, replacement_char);

            char old_full_path[PATH_MAX];
            char new_full_path[PATH_MAX];
            snprintf(old_full_path, sizeof(old_full_path), "%s/%s", matches[i].dir_path, matches[i].old_name);
            snprintf(new_full_path, sizeof(new_full_path), "%s/%s", matches[i].dir_path, new_name);

            if (rename(old_full_path, new_full_path) == 0) {
                printf("Renamed: %s --> %s\n", matches[i].old_name, new_name);
                success_count++;
            } else {
                perror("Error renaming file");
            }
        }
        printf("\nFinished! Renamed %d of %d file(s).\n", success_count, match_count);
    } else {
        printf("\nOperation cancelled. No files were changed.\n");
    }

    return 0;
}