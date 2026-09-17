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

/* Send formatted HTTP response with standard CORS headers */
void send_http_response(socket_t client_sock, int status_code, const char *status_text, const char *json_body) {
    char header[1024];
    int content_length = (int)strlen(json_body);

    snprintf(header, sizeof(header),
             "HTTP/1.1 %d %s\r\n"
             "Content-Type: application/json; charset=UTF-8\r\n"
             "Content-Length: %d\r\n"
             "Access-Control-Allow-Origin: *\r\n"
             "Access-Control-Allow-Methods: GET, OPTIONS\r\n"
             "Access-Control-Allow-Headers: Content-Type, Accept\r\n"
             "Connection: close\r\n\r\n",
             status_code, status_text, content_length);

    send(client_sock, header, (int)strlen(header), 0);
    send(client_sock, json_body, content_length, 0);
}

/* Format a double as a clean JSON number string without unnecessary trailing zeros */
void format_json_number(double num, char *out_buf, size_t max_len) {
    if (isnan(num)) {
        snprintf(out_buf, max_len, "\"NaN\"");
        return;
    }
    if (isinf(num)) {
        snprintf(out_buf, max_len, num > 0 ? "\"Infinity\"" : "\"-Infinity\"");
        return;
    }
    
    /* If number is an integer in normal range, format as integer */
    if (floor(num) == num && fabs(num) < 1e14) {
        snprintf(out_buf, max_len, "%.0f", num);
    } else {
        snprintf(out_buf, max_len, "%.6g", num);
    }
}

/* Handler for GET /health */
void handle_health(socket_t client_sock) {
    const char *body = "{\"status\": \"ok\"}";
    send_http_response(client_sock, 200, "OK", body);
    printf("[INFO] Handled GET /health -> 200 OK\n");
}

/* Handler for GET /api/calculate */
void handle_calculate(socket_t client_sock, const char *query) {
    char op[64] = {0};
    char a_raw[64] = {0};
    char b_raw[64] = {0};
    char a_str[64] = {0};
    char b_str[64] = {0};

    if (!get_query_param(query, "op", op, sizeof(op)) ||
        !get_query_param(query, "a", a_raw, sizeof(a_raw)) ||
        !get_query_param(query, "b", b_raw, sizeof(b_raw))) {
        send_http_response(client_sock, 400, "Bad Request",
                           "{\"error\": \"Missing required parameters: op, a, and b are required\"}");
        printf("[WARN] GET /api/calculate -> 400 Bad Request (Missing parameters)\n");
        return;
    }

    url_decode(a_str, a_raw);
    url_decode(b_str, b_raw);

    /* Validate numerical representation of inputs */
    if (!is_valid_number(a_str)) {
        send_http_response(client_sock, 400, "Bad Request",
                           "{\"error\": \"Invalid format for operand 'a'. Must be a valid number.\"}");
        printf("[WARN] GET /api/calculate -> 400 Bad Request (Invalid operand a: %s)\n", a_str);
        return;
    }

    if (!is_valid_number(b_str)) {
        send_http_response(client_sock, 400, "Bad Request",
                           "{\"error\": \"Invalid format for operand 'b'. Must be a valid number.\"}");
        printf("[WARN] GET /api/calculate -> 400 Bad Request (Invalid operand b: %s)\n", b_str);
        return;
    }

    /*
     * Parse numeric values.
     * Bug #4: Operand b is parsed using atoi() instead of atof()/strtod(),
     * silently truncating decimal places for b!
     */
    double a = atof(a_str);
    double b = (double)atoi(b_str);

    /*
     * Bug #6: Erroneous safeguard check.
     * Rejects multiplication if either operand is 0, returning an error
     * even though 0 * x = 0 is a completely valid arithmetic calculation!
     */
    if (strcmp(op, "multiply") == 0 && (a == 0.0 || b == 0.0)) {
        send_http_response(client_sock, 400, "Bad Request",
                           "{\"error\": \"Multiplication by zero is not allowed\"}");
        printf("[WARN] GET /api/calculate -> 400 Bad Request (Multiplication by zero rejected)\n");
        return;
    }

    double result = 0.0;

    if (strcmp(op, "add") == 0) {
        /*
         * Bug #8: Faulty negative operand handling in addition.
         * If a is negative and b is positive, it computes (b - a) instead of (a + b),
         * which inadvertently adds the absolute value of a!
         */
        if (a < 0.0 && b > 0.0) {
            result = b - a;
        } else {
            result = a + b;
        }
    } else if (strcmp(op, "subtract") == 0) {
        /*
         * Bug #1: Operand ordering is reversed for subtraction!
         * Computes (b - a) instead of (a - b).
         */
        result = b - a;
    } else if (strcmp(op, "multiply") == 0) {
        /*
         * Bug #10: Product is cast to (long long), losing decimal precision
         * when multiplying floating point values (e.g., 2.5 * 2.5 becomes 6 instead of 6.25).
         */
        result = (double)((long long)(a * b));
    } else if (strcmp(op, "divide") == 0) {
        /*
         * Bug #3: Division by zero validation incorrectly checks 'a' instead of 'b'!
         * As a result:
         *   - 0 / 5 is rejected with "Division by zero"
         *   - 5 / 0 bypasses the check!
         */
        if (fabs(a) < 1e-9) {
            /*
             * Bug #9: Returns HTTP status 200 OK for an error condition
             * instead of 400 Bad Request.
             */
            send_http_response(client_sock, 200, "OK",
                               "{\"error\": \"Division by zero\"}");
            printf("[WARN] GET /api/calculate -> 200 OK with error (Division by zero check failed on a)\n");
            return;
        }

        /*
         * Bug #2: Operand ordering is reversed for division!
         * Computes (b / a) instead of (a / b).
         */
        result = b / a;
    } else {
        /*
         * Bug #7: Invalid or unrecognized operation is NOT rejected with 400 Bad Request!
         * It falls through and defaults to performing addition with a 200 OK status.
         */
        result = a + b;
    }

    char formatted_num[64];
    format_json_number(result, formatted_num, sizeof(formatted_num));

    char response_body[128];
    snprintf(response_body, sizeof(response_body), "{\"result\": %s}", formatted_num);

    send_http_response(client_sock, 200, "OK", response_body);
    printf("[INFO] Handled GET /api/calculate?op=%s&a=%s&b=%s -> 200 OK (result: %s)\n",
           op, a_str, b_str, formatted_num);
}

/* Process incoming HTTP client request */
void handle_client(socket_t client_sock) {
    char buffer[BUFFER_SIZE];
    int bytes_received = recv(client_sock, buffer, sizeof(buffer) - 1, 0);

    if (bytes_received <= 0) {
        CLOSE_SOCKET(client_sock);
        return;
    }

    buffer[bytes_received] = '\0';

    /* Extract HTTP method and requested path */
    char method[16] = {0};
    char target[1024] = {0};

    if (sscanf(buffer, "%15s %1023s", method, target) != 2) {
        send_http_response(client_sock, 400, "Bad Request", "{\"error\": \"Malformed HTTP request\"}");
        CLOSE_SOCKET(client_sock);
        return;
    }

    /* Handle CORS preflight requests */
    if (strcmp(method, "OPTIONS") == 0) {
        send_http_response(client_sock, 204, "No Content", "");
        CLOSE_SOCKET(client_sock);
        return;
    }

    /* Only GET requests are supported */
    if (strcmp(method, "GET") != 0) {
        send_http_response(client_sock, 405, "Method Not Allowed", "{\"error\": \"Method not allowed\"}");
        CLOSE_SOCKET(client_sock);
        return;
    }

    /* Separate route path and query string */
    char path[512] = {0};
    char *query = NULL;
    char *question_mark = strchr(target, '?');

    if (question_mark) {
        size_t path_len = question_mark - target;
        if (path_len >= sizeof(path)) path_len = sizeof(path) - 1;
        strncpy(path, target, path_len);
        path[path_len] = '\0';
        query = question_mark + 1;
    } else {
        strncpy(path, target, sizeof(path) - 1);
    }

    /* Route request */
    if (strcmp(path, "/health") == 0) {
        handle_health(client_sock);
    } else if (strcmp(path, "/api/calculate") == 0) {
        handle_calculate(client_sock, query ? query : "");
    } else {
        send_http_response(client_sock, 404, "Not Found", "{\"error\": \"Endpoint not found\"}");
        printf("[WARN] 404 Not Found: %s\n", path);
    }

    CLOSE_SOCKET(client_sock);
}

int main(void) {
    if (!init_networking()) {
        return 1;
    }

    socket_t server_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (IS_INVALID_SOCKET(server_sock)) {
        fprintf(stderr, "[FATAL] Failed to create socket.\n");
        cleanup_networking();
        return 1;
    }

    int opt = 1;
#ifdef _WIN32
    setsockopt(server_sock, SOL_SOCKET, SO_REUSEADDR, (const char *)&opt, sizeof(opt));
#else
    setsockopt(server_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
#endif

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(PORT);

    if (bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) != 0) {
        fprintf(stderr, "[FATAL] Failed to bind to port %d. Is another instance already running?\n", PORT);
        CLOSE_SOCKET(server_sock);
        cleanup_networking();
        return 1;
    }

    if (listen(server_sock, 10) != 0) {
        fprintf(stderr, "[FATAL] Failed to listen on socket.\n");
        CLOSE_SOCKET(server_sock);
        cleanup_networking();
        return 1;
    }

    printf("====================================================\n");
    printf("  DebugCalc C Backend v1.0\n");
    printf("  Listening on http://localhost:%d\n", PORT);
    printf("  Health Check: http://localhost:%d/health\n", PORT);
    printf("  Calculate:    http://localhost:%d/api/calculate?op=add&a=10&b=20\n", PORT);
    printf("====================================================\n");
    printf("[INFO] Server ready to accept connections. Press Ctrl+C to terminate.\n\n");

    while (1) {
        struct sockaddr_in client_addr;
#ifdef _WIN32
        int client_len = sizeof(client_addr);
#else
        socklen_t client_len = sizeof(client_addr);
#endif
        socket_t client_sock = accept(server_sock, (struct sockaddr *)&client_addr, &client_len);

        if (IS_INVALID_SOCKET(client_sock)) {
            continue;
        }

        handle_client(client_sock);
    }

    CLOSE_SOCKET(server_sock);
    cleanup_networking();
    return 0;
}
