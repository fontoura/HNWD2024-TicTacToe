#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>

int inet_pton(int af, const char *src, void *dst) {
    //printf("inet_pton\n");
    //printf("af: %d\n", af);
    //printf("src: %s\n", src);
    //printf("dst: %p\n", dst);

    int length;
    int result;
    
    switch (af) {
        case AF_INET:
            length = sizeof(struct sockaddr_in);
            result = WSAStringToAddressA((LPSTR)src, af, NULL, (struct sockaddr *)dst, &length);
            if (result == 0) {
                // Successfully converted
                *(struct in_addr *)dst = ((struct sockaddr_in *)dst)->sin_addr;
                return 1;
            }
            break;

        case AF_INET6:
            length = sizeof(struct sockaddr_in6);
            result = WSAStringToAddressA((LPSTR)src, af, NULL, (struct sockaddr *)dst, &length);
            if (result == 0) {
                // Successfully converted
                *(struct in6_addr *)dst = ((struct sockaddr_in6 *)dst)->sin6_addr;
                return 1;
            }
            break;

        default:
            // Unsupported address family
            return 0;
    }

    // If we reached here, an error occurred
    int error = WSAGetLastError();
    //printf("WSAStringToAddressA failed with error: %d\n", error);
    return 0;
}

#else
#include <unistd.h>
#include <arpa/inet.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/socket.h>
#endif

#define BUFFER_SIZE 4096

void die(const char *msg) {
#ifdef _WIN32
    //fprintf(stderr, "%s: %d\n", msg, WSAGetLastError());
#else
    //perror(msg);
#endif
    //exit(EXIT_FAILURE);
}

// Function to parse the URL into hostname, port, and path
void parse_url(const char *url, char *hostname, char *port, char *path) {
    char *url_copy = strdup(url);
    if (!url_copy) {
        die("strdup");
    }

    // Skip the scheme (e.g., http://)
    char *host_start = strstr(url_copy, "://");
    if (host_start) {
        host_start += 3; // Skip "://"
    } else {
        host_start = url_copy; // No scheme
    }

    // Extract hostname and port
    char *port_start = strchr(host_start, ':');
    char *path_start = strchr(host_start, '/');
    if (port_start && path_start && port_start < path_start) {
        strncpy(hostname, host_start, port_start - host_start);
        hostname[port_start - host_start] = '\0';
        strncpy(port, port_start + 1, path_start - port_start - 1);
        port[path_start - port_start - 1] = '\0';
        strcpy(path, path_start);
    } else if (path_start) {
        strncpy(hostname, host_start, path_start - host_start);
        hostname[path_start - host_start] = '\0';
        strcpy(path, path_start);
        port[0] = '\0'; // Default port
    } else {
        strcpy(hostname, host_start);
        port[0] = '\0'; // Default port
        path[0] = '\0';
    }

    free(url_copy);

    // Default port to 80 if not specified
    if (strlen(port) == 0) {
        strcpy(port, "80");
    }
}

#ifdef _WIN32
static int firstTime = 1;
#endif

void upload_file(const char *file_path, const char *url) {
    int sockfd;
#ifdef _WIN32
    WSADATA wsaData;
    struct sockaddr_in server_addr;
    char request[BUFFER_SIZE];
    char file_buffer[BUFFER_SIZE];
    FILE *file;
    size_t file_size;
    ssize_t bytes_read;
    char hostname[256];
    char port[6];
    char path[1024];

    // Initialize Winsock
    if (firstTime) {
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
            die("WSAStartup failed");
            return;
        }
        firstTime = 0;
    }
#else
    struct sockaddr_in server_addr;
    char request[BUFFER_SIZE];
    char file_buffer[BUFFER_SIZE];
    FILE *file;
    size_t file_size;
    ssize_t bytes_read;
    char hostname[256];
    char port[6];
    char path[1024];
#endif

    // Parse the URL
    parse_url(url, hostname, port, path);

#ifdef _WIN32
    // Create socket
    if ((sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET) {
        die("socket");
        return;
    }
#else
    // Create socket
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        die("socket");
        return;
    }
#endif

    // Set up server address
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(atoi(port));
#ifdef _WIN32
    if (inet_pton(AF_INET, hostname, &server_addr.sin_addr) <= 0) {
        die("inet_pton");
        return;
    }
#else
    if (inet_pton(AF_INET, hostname, &server_addr.sin_addr) <= 0) {
        die("inet_pton");
        return;
    }
#endif

    // Connect to server
    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        die("connect");
        return;
    }

    // Open file
    if ((file = fopen(file_path, "rb")) == NULL) {
        die("fopen");
        return;
    }

    // Get file size
    fseek(file, 0, SEEK_END);
    file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    // Create HTTP POST request
    snprintf(request, sizeof(request),
             "POST %s HTTP/1.1\r\n"
             "Host: %s\r\n"
             "Content-Length: %lu\r\n"
             "Content-Type: application/octet-stream\r\n"
             "Connection: close\r\n\r\n",
             path, hostname, file_size);

    // Send request headers
    if (send(sockfd, request, strlen(request), 0) < 0) {
        die("send");
        return;
    }

    // Send file data
    while ((bytes_read = fread(file_buffer, 1, sizeof(file_buffer), file)) > 0) {
        if (send(sockfd, file_buffer, bytes_read, 0) < 0) {
            die("send");
            return;
        }
    }
    send(sockfd, "\r\n", 2, 0);

    // Receive response
    /*char response[BUFFER_SIZE];
    ssize_t response_length;
    while ((response_length = recv(sockfd, response, sizeof(response) - 1, 0)) > 0) {
        response[response_length] = '\0';
        //printf("%s", response);
    }
    if (response_length < 0) {
        //die("recv");
        return;
    }*/

    // Clean up
    fclose(file);
#ifdef _WIN32
    closesocket(sockfd);
#else
    close(sockfd);
#endif
}
