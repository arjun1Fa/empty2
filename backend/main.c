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

/* Decode percent-encoded characters in URL (e.g., %20 -> space, %2B -> +) */
void url_decode(char *dst, const char *src) {
    char a, b;
    while (*src) {
        if ((*src == '%') &&
            ((a = src[1]) && (b = src[2])) &&
            (isxdigit((unsigned char)a) && isxdigit((unsigned char)b))) {
            if (a >= 'a') a -= 'a' - 'A';
            if (a >= 'A') a -= ('A' - 10);
            else a -= '0';
            if (b >= 'a') b -= 'a' - 'A';
            if (b >= 'A') b -= ('A' - 10);
            else b -= '0';
            *dst++ = 16 * a + b;
            src += 3;
        } else if (*src == '+') {
            *dst++ = ' ';
            src++;
        } else {
            *dst++ = *src++;
        }
    }
    *dst = '\0';
}

/* Extract query parameter value by key from query string */
int get_query_param(const char *query, const char *key, char *out_val, size_t max_len) {
    if (!query || !key || !out_val || max_len == 0) return 0;
    
    size_t key_len = strlen(key);
    const char *p = query;

    while (*p) {
        if (strncmp(p, key, key_len) == 0 && p[key_len] == '=') {
            p += key_len + 1;
            size_t i = 0;
            while (*p && *p != '&' && i < max_len - 1) {
                out_val[i++] = *p++;
            }
            out_val[i] = '\0';
            return 1;
        }
        const char *next = strchr(p, '&');
        if (!next) break;
        p = next + 1;
    }
    return 0;
}

/* Validate if a string represents a valid numeric input */
int is_valid_number(const char *str) {
    if (!str || *str == '\0') return 0;
    
    int decimal_count = 0;
    /*
     * NOTE: Validation logic for student inspection.
     * Bug #5: Does not account for optional leading minus sign '-' or plus sign '+'.
     * This causes negative inputs like "-5" to fail validation.
     */
    for (int i = 0; str[i] != '\0'; i++) {
        if (str[i] == '.') {
            decimal_count++;
            if (decimal_count > 1) return 0;
        } else if (!isdigit((unsigned char)str[i])) {
            return 0;
        }
    }
    return 1;
}

int main(void) { return 0; }
