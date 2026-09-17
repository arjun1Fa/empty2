/*
 * ============================================================================
 * DebugCalc - Lightweight C Backend HTTP Server
 * ============================================================================
 *
 * NOTE FOR STUDENTS:
 * Welcome to the DebugCalc backend! This server handles arithmetic operations
 * requested by the frontend via HTTP GET requests.
 *
 * It listens on port 8080 and provides:
 *   - GET /health
 *   - GET /api/calculate?op=<operation>&a=<number>&b=<number>
 *
 * Please read the documentation in README.md for instructions on how to build,
 * run, reproduce issues, and submit pull requests.
 * ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#ifdef _WIN32
    #ifndef _WIN32_WINNT
        #define _WIN32_WINNT 0x0600
    #endif
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #ifdef _MSC_VER
        #pragma comment(lib, "ws2_32.lib")
    #endif
    typedef SOCKET socket_t;
    #define CLOSE_SOCKET(s) closesocket(s)
    #define IS_INVALID_SOCKET(s) ((s) == INVALID_SOCKET)
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <unistd.h>
    #include <netdb.h>
    typedef int socket_t;
    #define CLOSE_SOCKET(s) close(s)
    #define IS_INVALID_SOCKET(s) ((s) < 0)
#endif

#define PORT 8080
#define BUFFER_SIZE 4096

/* Initialize socket subsystem (required on Windows) */
int init_networking(void) {
#ifdef _WIN32
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        fprintf(stderr, "[ERROR] WSAStartup failed.\n");
        return 0;
    }
#endif
    return 1;
}

/* Cleanup socket subsystem */
void cleanup_networking(void) {
#ifdef _WIN32
    WSACleanup();
#endif
}

int main(void) { return 0; }
