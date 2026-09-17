#!/usr/bin/env python3
"""
============================================================================
DebugCalc - Lightweight Python Backend HTTP Server (Python 3.11 compatible)
============================================================================

NOTE FOR STUDENTS:
Welcome to the DebugCalc backend! This server handles arithmetic operations
requested by the frontend via HTTP GET requests.

It listens on port 8080 and provides:
  - GET /health
  - GET /api/calculate?op=<operation>&a=<number>&b=<number>

This Python version replicates the exact architecture and all 10 intentional bugs
from the original C implementation (main.c) for educational debugging exercises.
Every intentional bug and potential issue is clearly documented with comments.
============================================================================
"""

import math
import urllib.parse
from http.server import HTTPServer, BaseHTTPRequestHandler

PORT = 8080


# ============================================================================
# BUG #5: Negative Numbers Rejected by Validator
#
# Issue/Error:
#   The validator checks that every character is a digit or decimal point.
#   It does NOT account for an optional leading '+' or '-' sign.
#
# Consequence:
#   Negative numbers such as "-5" or "-12.4" fail validation and return
#   HTTP 400 Bad Request with an invalid format error.
#
# How to Fix:
#   Allow an optional '+' or '-' at index 0 before checking digits.
# ============================================================================
def is_valid_number(s: str) -> bool:
    """Validate if a string represents a valid numeric input."""
    if not s:
        return False

    decimal_count = 0
    for ch in s:
        if ch == '.':
            decimal_count += 1
            if decimal_count > 1:
                return False
        elif not ch.isdigit():
            return False

    return True


def format_json_number(num: float) -> str:
    """Format a float as a clean JSON number string without trailing zeros."""
    if math.isnan(num):
        return '"NaN"'
    if math.isinf(num):
        return '"Infinity"' if num > 0 else '"-Infinity"'

    # If number is an integer in normal range, format as integer
    if num.is_integer() and abs(num) < 1e14:
        return f"{int(num)}"

    # Format matching C's %.6g
    return f"{num:.6g}"


class CalculatorHandler(BaseHTTPRequestHandler):
    """HTTP Request Handler for DebugCalc API."""

    protocol_version = "HTTP/1.1"

    def log_message(self, format, *args):
        """
        Suppress noisy default console request logging to keep backend output clean.
        """
        pass

    def send_http_response(self, status_code: int, status_text: str, json_body: str):
        """Send formatted HTTP response with standard CORS headers."""
        body_bytes = json_body.encode('utf-8')
        self.send_response(status_code, status_text)
        self.send_header('Content-Type', 'application/json; charset=UTF-8')
        self.send_header('Content-Length', str(len(body_bytes)))
        self.send_header('Access-Control-Allow-Origin', '*')
        self.send_header('Access-Control-Allow-Methods', 'GET, OPTIONS')
        self.send_header('Access-Control-Allow-Headers', 'Content-Type, Accept')
        self.send_header('Connection', 'close')
        self.end_headers()
        self.wfile.write(body_bytes)

    def do_OPTIONS(self):
        """Handle CORS preflight requests."""
        self.send_response(204, 'No Content')
        self.send_header('Content-Length', '0')
        self.send_header('Access-Control-Allow-Origin', '*')
        self.send_header('Access-Control-Allow-Methods', 'GET, OPTIONS')
        self.send_header('Access-Control-Allow-Headers', 'Content-Type, Accept')
        self.send_header('Connection', 'close')
        self.end_headers()

    def do_GET(self):
        """Process incoming GET requests."""
        parsed_url = urllib.parse.urlparse(self.path)
        path = parsed_url.path
        query = parsed_url.query

        if path == "/health":
            self.handle_health()
        elif path == "/api/calculate":
            self.handle_calculate(query)
        else:
            # ================================================================
            # ERROR HANDLING: Endpoint Not Found (404)
            # Issue/Error: Requested route does not match /health or /api/calculate.
            # ================================================================
            self.send_http_response(404, "Not Found", '{"error": "Endpoint not found"}')

    def do_POST(self):
        """Handle unsupported methods."""
        self.handle_method_not_allowed()

    def do_PUT(self):
        """Handle unsupported methods."""
        self.handle_method_not_allowed()

    def do_DELETE(self):
        """Handle unsupported methods."""
        self.handle_method_not_allowed()

    def handle_method_not_allowed(self):
        """
        ======================================================================
        ERROR HANDLING: Method Not Allowed (405)
        Issue/Error: Only GET and OPTIONS requests are supported by this API.
        ======================================================================
        """
        self.send_http_response(405, "Method Not Allowed", '{"error": "Method not allowed"}')

    def handle_health(self):
        """Handler for GET /health endpoint."""
        body = '{"status": "ok"}'
        self.send_http_response(200, "OK", body)

    def handle_calculate(self, query_string: str):
        """Handler for GET /api/calculate endpoint."""
        # Parse query parameters
        params = urllib.parse.parse_qs(query_string, keep_blank_values=True)
        op_list = params.get('op', [])
        a_list = params.get('a', [])
        b_list = params.get('b', [])

        # ====================================================================
        # ERROR HANDLING: Missing Required Parameters
        # Issue/Error:
        #   Client did not supply all three required query parameters:
        #   'op', 'a', and 'b'.
        # Response:
        #   HTTP 400 Bad Request with descriptive JSON error.
        # ====================================================================
        if not op_list or not a_list or not b_list:
            self.send_http_response(
                400,
                "Bad Request",
                '{"error": "Missing required parameters: op, a, and b are required"}'
            )
            return

        op = op_list[0]
        a_str = a_list[0]
        b_str = b_list[0]

        # ====================================================================
        # ERROR HANDLING: Operand Validation
        # Issue/Error:
        #   Input string contains characters that are not valid numbers.
        #   (Note: Due to Bug #5, negative signs are also rejected here!)
        # Response:
        #   HTTP 400 Bad Request
        # ====================================================================
        if not is_valid_number(a_str):
            self.send_http_response(
                400,
                "Bad Request",
                '{"error": "Invalid format for operand \'a\'. Must be a valid number."}'
            )
            return

        if not is_valid_number(b_str):
            self.send_http_response(
                400,
                "Bad Request",
                '{"error": "Invalid format for operand \'b\'. Must be a valid number."}'
            )
            return

        # ====================================================================
        # BUG #4: Decimal Truncation in Operand 'b'
        #
        # Issue/Error:
        #   In C main.c, operand b is parsed using atoi() instead of atof(),
        #   which truncates all decimal places (e.g., 2.5 becomes 2.0).
        #
        # Consequence:
        #   Calculations like 10 + 2.5 evaluate to 10 + 2 = 12.
        #
        # How to Fix:
        #   Parse b as float(b_str) instead of casting through int.
        # ====================================================================
        a = float(a_str)
        b = float(int(float(b_str)))  # Exact replication of (double)atoi(b_str)

        # ====================================================================
        # BUG #6: Multiplication by Zero Erroneously Rejected
        #
        # Issue/Error:
        #   An incorrect safeguard rejects multiplication if either operand is 0.
        #   However, 0 * x = 0 is a completely valid mathematical operation!
        #
        # Consequence:
        #   Multiplications like 0 * 5 return HTTP 400 Bad Request error.
        #
        # How to Fix:
        #   Remove this check completely.
        # ====================================================================
        if op == "multiply" and (a == 0.0 or b == 0.0):
            self.send_http_response(
                400,
                "Bad Request",
                '{"error": "Multiplication by zero is not allowed"}'
            )
            return

        result = 0.0

        if op == "add":
            # ================================================================
            # BUG #8: Faulty Sign Arithmetic in Addition
            #
            # Issue/Error:
            #   If 'a' is negative and 'b' is positive, it computes (b - a).
            #   Because 'a' is negative, b - (-|a|) = b + |a|, inadvertently
            #   adding the absolute value of a!
            #
            # Consequence:
            #   -5 + 10 calculates 10 - (-5) = 15 instead of 5.
            #
            # How to Fix:
            #   Simply compute 'result = a + b' unconditionally.
            # ================================================================
            if a < 0.0 and b > 0.0:
                result = b - a
            else:
                result = a + b

        elif op == "subtract":
            # ================================================================
            # BUG #1: Reversed Operands in Subtraction
            #
            # Issue/Error:
            #   Operand order is reversed: computes (b - a) instead of (a - b).
            #
            # Consequence:
            #   10 - 3 returns -7 instead of 7.
            #
            # How to Fix:
            #   Change 'result = b - a' to 'result = a - b'.
            # ================================================================
            result = b - a

        elif op == "multiply":
            # ================================================================
            # BUG #10: Integer Truncation on Multiplication Products
            #
            # Issue/Error:
            #   In C main.c, the product is cast to (long long), stripping
            #   decimal precision.
            #
            # Consequence:
            #   2.5 * 2.5 produces 6.0 instead of 6.25.
            #
            # How to Fix:
            #   Compute 'result = a * b' without integer casting.
            # ================================================================
            result = float(int(a * b))

        elif op == "divide":
            # ================================================================
            # BUG #3: Division by Zero Validation Checks Wrong Variable
            #
            # Issue/Error:
            #   The guard condition checks numerator 'a' instead of denominator 'b'!
            #
            # Consequence:
            #   - 0 / 5 triggers "Division by zero" error!
            #   - 5 / 0 bypasses the validation!
            #
            # How to Fix:
            #   Change 'abs(a) < 1e-9' to 'abs(b) < 1e-9'.
            # ================================================================
            if abs(a) < 1e-9:
                # ============================================================
                # BUG #9: Incorrect HTTP Status Code on Division by Zero
                #
                # Issue/Error:
                #   Returns HTTP 200 OK for an error condition instead of 400.
                #
                # Consequence:
                #   The response contains an error JSON, but HTTP status is 200.
                #
                # How to Fix:
                #   Return status code 400 Bad Request instead of 200 OK.
                # ============================================================
                self.send_http_response(200, "OK", '{"error": "Division by zero"}')
                return

            # ================================================================
            # BUG #2: Reversed Operands in Division
            #
            # Issue/Error:
            #   Computes (b / a) instead of (a / b).
            #
            # Consequence:
            #   20 / 5 evaluates to 5 / 20 = 0.25 instead of 4.
            #
            # How to Fix:
            #   Change 'result = b / a' to 'result = a / b'.
            # ================================================================
            result = b / a

        else:
            # ================================================================
            # BUG #7: Fallthrough on Invalid Operations
            #
            # Issue/Error:
            #   Invalid/unsupported operation names are not rejected with 400.
            #   Instead, execution falls into 'else' and defaults to addition!
            #
            # Consequence:
            #   Requesting op=power returns 200 OK with the sum of a and b.
            #
            # How to Fix:
            #   Return HTTP 400 Bad Request with an "Unsupported operation" error.
            # ================================================================
            result = a + b

        formatted_num = format_json_number(result)
        response_body = f'{{"result": {formatted_num}}}'
        self.send_http_response(200, "OK", response_body)


class ReusableHTTPServer(HTTPServer):
    """HTTPServer with address reuse enabled to prevent bind errors on restart."""
    allow_reuse_address = True


def main():
    server_address = ('', PORT)
    httpd = ReusableHTTPServer(server_address, CalculatorHandler)

    print("====================================================")
    print("  DebugCalc Python Backend (Python 3.11)")
    print(f"  Listening on http://localhost:{PORT}")
    print(f"  Health Check: http://localhost:{PORT}/health")
    print(f"  Calculate:    http://localhost:{PORT}/api/calculate?op=add&a=10&b=20")
    print("====================================================")
    print("Server ready to accept connections. Press Ctrl+C to terminate.\n")

    try:
        httpd.serve_forever()
    except KeyboardInterrupt:
        print("\nShutting down server...")
        httpd.server_close()


if __name__ == '__main__':
    main()
