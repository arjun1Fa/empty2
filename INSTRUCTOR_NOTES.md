# ⚠️ INSTRUCTOR NOTES — DO NOT DISTRIBUTE TO STUDENTS ⚠️

> **CONFIDENTIAL TEACHING REFERENCE**
>
> This document is strictly for educators, workshop facilitators, and teaching assistants. It details every intentional bug in the **DebugCalc** backend, the exact reproduction steps, expected vs. actual outcomes, recommended code fixes, and pedagogical difficulty ratings.

---

## Quick Reference Summary Table

| # | Bug Name | Category | Function / Location | Difficulty |
|---|---|---|---|---|
| **1** | Reversed Operands in Subtraction | Arithmetic Logic | `handle_calculate` | Beginner |
| **2** | Reversed Operands in Division | Arithmetic Logic | `handle_calculate` | Beginner |
| **3** | Division-by-Zero Checks Wrong Variable | Validation Logic | `handle_calculate` | Intermediate |
| **4** | Decimal Truncation in Operand `b` | Type Parsing | `handle_calculate` | Intermediate |
| **5** | Negative Numbers Rejected by Validator | Input Validation | `is_valid_number` | Intermediate |
| **6** | Multiplication by Zero Erroneously Rejected | Business Logic | `handle_calculate` | Intermediate |
| **7** | Fallthrough on Invalid Operations | API Routing / Fallback | `handle_calculate` | Beginner |
| **8** | Faulty Sign Arithmetic in Addition | Edge Case Logic | `handle_calculate` | Intermediate / Advanced |
| **9** | Incorrect HTTP Status Code (200 instead of 400) | HTTP Protocol Compliance | `handle_calculate` | Beginner / Intermediate |
| **10** | Integer Truncation on Multiplication Products | Precision / Casting | `handle_calculate` | Intermediate |

---

## Detailed Bug Breakdown

### Bug 1: Reversed Operands in Subtraction
- **Difficulty**: Beginner
- **Location**: `backend/main.c`, inside `handle_calculate()`, `strcmp(op, "subtract") == 0` block.
- **What the bug does**: Computes `b - a` instead of `a - b`.
- **How to reproduce**:
  - **Frontend**: Enter First Number `10`, Second Number `3`, select **Subtract**, click **Calculate Result**.
  - **cURL**:
    ```bash
    curl "http://localhost:8080/api/calculate?op=subtract&a=10&b=3"
    ```
- **Expected Behavior**: `{"result": 7}`
- **Actual Behavior**: `{"result": -7}`
- **Code Comparison**:
  ```c
  // BROKEN
  result = b - a;

  // FIXED
  result = a - b;
  ```
- **Pedagogical Takeaway**: Teaches students to check variable order and parameter mapping in binary operations.

---

### Bug 2: Reversed Operands in Division
- **Difficulty**: Beginner
- **Location**: `backend/main.c`, inside `handle_calculate()`, `strcmp(op, "divide") == 0` block.
- **What the bug does**: Computes `b / a` instead of `a / b`.
- **How to reproduce**:
  - **Frontend**: Enter First Number `20`, Second Number `5`, select **Divide**, click **Calculate Result**.
  - **cURL**:
    ```bash
    curl "http://localhost:8080/api/calculate?op=divide&a=20&b=5"
    ```
- **Expected Behavior**: `{"result": 4}`
- **Actual Behavior**: `{"result": 0.25}`
- **Code Comparison**:
  ```c
  // BROKEN
  result = b / a;

  // FIXED
  result = a / b;
  ```
- **Pedagogical Takeaway**: Reinforces vigilance with non-commutative mathematical operations.

---

### Bug 3: Division-by-Zero Checks Wrong Variable
- **Difficulty**: Intermediate
- **Location**: `backend/main.c`, inside `handle_calculate()`, `strcmp(op, "divide") == 0` block.
- **What the bug does**: The guard condition checks whether the numerator `a` is zero (`fabs(a) < 1e-9`) instead of checking the denominator `b`. Consequently, dividing zero by any number (`0 / 5`) triggers a "Division by zero" error, while dividing a number by zero (`5 / 0`) bypasses validation entirely!
- **How to reproduce**:
  - **Frontend Test 1**: Enter `0` in First Number and `5` in Second Number. Click **Divide**. It shows an error.
  - **Frontend Test 2**: Enter `5` in First Number and `0` in Second Number. Click **Divide**. It calculates without throwing a validation error.
  - **cURL**:
    ```bash
    curl "http://localhost:8080/api/calculate?op=divide&a=0&b=5"
    ```
- **Expected Behavior**: `0 / 5` should return `{"result": 0}`, while `5 / 0` should return `{"error": "Division by zero"}`.
- **Actual Behavior**: `0 / 5` returns `{"error": "Division by zero"}`, while `5 / 0` returns `{"result": 0}` (or Infinity depending on operands).
- **Code Comparison**:
  ```c
  // BROKEN
  if (fabs(a) < 1e-9) {
      send_http_response(client_sock, 400, "Bad Request", "{\"error\": \"Division by zero\"}");
      return;
  }

  // FIXED
  if (fabs(b) < 1e-9) {
      send_http_response(client_sock, 400, "Bad Request", "{\"error\": \"Division by zero\"}");
      return;
  }
  ```
- **Pedagogical Takeaway**: Illustrates how a copy-paste error or wrong identifier in a validation guard creates inverted logic.

---

### Bug 4: Decimal Truncation in Operand `b` (`atoi` instead of `atof`)
- **Difficulty**: Intermediate
- **Location**: `backend/main.c`, inside `handle_calculate()`, numerical conversion block.
- **What the bug does**: Operand `a` is converted with `atof(a_str)`, but operand `b` is converted using `atoi(b_str)`. As a result, fractional/floating-point values for operand `b` are silently truncated to integers (e.g., `2.7` becomes `2.0`).
- **How to reproduce**:
  - **Frontend**: Enter First Number `10.0`, Second Number `2.5`, select **Add**, click **Calculate Result**.
  - **cURL**:
    ```bash
    curl "http://localhost:8080/api/calculate?op=add&a=10.0&b=2.5"
    ```
- **Expected Behavior**: `{"result": 12.5}`
- **Actual Behavior**: `{"result": 12}`
- **Code Comparison**:
  ```c
  // BROKEN
  double a = atof(a_str);
  double b = (double)atoi(b_str);

  // FIXED
  double a = atof(a_str);
  double b = atof(b_str);
  // Or even better: strtod(b_str, NULL);
  ```
- **Pedagogical Takeaway**: Highlights the importance of consistent standard library conversion functions and spotting subtle typing discrepancies.

---

### Bug 5: Negative Numbers Rejected by Validator
- **Difficulty**: Intermediate
- **Location**: `backend/main.c`, inside `is_valid_number()`.
- **What the bug does**: The function iterates through the input string and rejects any character that is not a digit (`isdigit`) or a single decimal point. It does not account for an optional leading `+` or `-` sign, causing all negative numbers (e.g. `-5`, `-12.4`) to fail validation with `400 Bad Request`.
- **How to reproduce**:
  - **Frontend**: Enter First Number `-5`, Second Number `10`, click **Add**.
  - **cURL**:
    ```bash
    curl "http://localhost:8080/api/calculate?op=add&a=-5&b=10"
    ```
- **Expected Behavior**: Validation passes, returns `{"result": 5}`.
- **Actual Behavior**: HTTP `400 Bad Request` with `{"error": "Invalid format for operand 'a'. Must be a valid number."}`
- **Code Comparison**:
  ```c
  // BROKEN
  int is_valid_number(const char *str) {
      if (!str || *str == '\0') return 0;
      int decimal_count = 0;
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

  // FIXED
  int is_valid_number(const char *str) {
      if (!str || *str == '\0') return 0;
      int i = 0;
      if (str[0] == '-' || str[0] == '+') {
          i = 1;
          if (str[1] == '\0') return 0; // Just a sign with no digits is invalid
      }
      int decimal_count = 0;
      int digit_count = 0;
      for (; str[i] != '\0'; i++) {
          if (str[i] == '.') {
              decimal_count++;
              if (decimal_count > 1) return 0;
          } else if (isdigit((unsigned char)str[i])) {
              digit_count++;
          } else {
              return 0;
          }
      }
      return digit_count > 0;
  }
  ```
- **Pedagogical Takeaway**: Teaches students how to write robust string validation for numerical inputs including signs and boundaries.

---

### Bug 6: Multiplication by Zero Erroneously Rejected
- **Difficulty**: Intermediate
- **Location**: `backend/main.c`, inside `handle_calculate()`, prior to arithmetic execution.
- **What the bug does**: An unnecessary "safeguard" checks `if (strcmp(op, "multiply") == 0 && (a == 0.0 || b == 0.0))` and returns `400 Bad Request`. In mathematics, `0 * x = 0` is completely legitimate.
- **How to reproduce**:
  - **Frontend**: Enter First Number `0`, Second Number `5`, select **Multiply**, click **Calculate Result**.
  - **cURL**:
    ```bash
    curl "http://localhost:8080/api/calculate?op=multiply&a=0&b=5"
    ```
- **Expected Behavior**: `{"result": 0}` with status `200 OK`.
- **Actual Behavior**: HTTP `400 Bad Request` with `{"error": "Multiplication by zero is not allowed"}`.
- **Code Comparison**:
  ```c
  // BROKEN
  if (strcmp(op, "multiply") == 0 && (a == 0.0 || b == 0.0)) {
      send_http_response(client_sock, 400, "Bad Request",
                         "{\"error\": \"Multiplication by zero is not allowed\"}");
      return;
  }

  // FIXED
  // Simply remove this bogus safeguard block entirely!
  ```
- **Pedagogical Takeaway**: Teaches students to critically assess domain requirements and remove bogus guards that confuse division rules with multiplication.

---

### Bug 7: Invalid Operation Fallthrough
- **Difficulty**: Beginner
- **Location**: `backend/main.c`, inside `handle_calculate()`, the `else` branch of operation dispatching.
- **What the bug does**: If the user sends an invalid or unsupported operation (such as `op=modulo`, `op=exponent`, or `op=foobar`), the server does not reject it with `400 Bad Request`. Instead, it falls into the `else` branch and computes addition!
- **How to reproduce**:
  - **cURL**:
    ```bash
    curl "http://localhost:8080/api/calculate?op=power&a=3&b=2"
    ```
- **Expected Behavior**: HTTP `400 Bad Request` with `{"error": "Unsupported operation: power. Supported operations: add, subtract, multiply, divide"}`.
- **Actual Behavior**: HTTP `200 OK` with `{"result": 5}`.
- **Code Comparison**:
  ```c
  // BROKEN
  else {
      result = a + b;
  }

  // FIXED
  else {
      send_http_response(client_sock, 400, "Bad Request",
                         "{\"error\": \"Invalid operation. Supported: add, subtract, multiply, divide\"}");
      return;
  }
  ```
- **Pedagogical Takeaway**: Teaches defensive programming and handling invalid states explicitly instead of defaulting to arbitrary operations.

---

### Bug 8: Faulty Sign Arithmetic in Addition
- **Difficulty**: Intermediate / Advanced
- **Location**: `backend/main.c`, inside `handle_calculate()`, `strcmp(op, "add") == 0` block.
- **What the bug does**: Contains an over-complicated condition:
  ```c
  if (a < 0.0 && b > 0.0) {
      result = b - a;
  }
  ```
  Since `a` is already negative, subtracting `a` computes `b - (-|a|) = b + |a|`! E.g., `-5 + 10` calculates `10 - (-5) = 15` instead of `5`.
- **How to reproduce**:
  *(Note: Bug #5 must be fixed first to submit negative numbers via the API!)*
  - **Frontend**: Enter First Number `-5`, Second Number `10`, click **Add**.
  - **cURL**:
    ```bash
    curl "http://localhost:8080/api/calculate?op=add&a=-5&b=10"
    ```
- **Expected Behavior**: `{"result": 5}`
- **Actual Behavior**: `{"result": 15}`
- **Code Comparison**:
  ```c
  // BROKEN
  if (a < 0.0 && b > 0.0) {
      result = b - a;
  } else {
      result = a + b;
  }

  // FIXED
  result = a + b;
  ```
- **Pedagogical Takeaway**: Shows how premature or flawed sign optimizations corrupt straightforward arithmetic expressions.

---

### Bug 9: Incorrect HTTP Status Code on Division by Zero Error
- **Difficulty**: Beginner / Intermediate
- **Location**: `backend/main.c`, inside `handle_calculate()`, division error handler.
- **What the bug does**: When returning the JSON error body `{"error": "Division by zero"}`, the server transmits `HTTP/1.1 200 OK` instead of `HTTP/1.1 400 Bad Request`.
- **How to reproduce**:
  - **cURL** (inspect headers with `-i`):
    ```bash
    curl -i "http://localhost:8080/api/calculate?op=divide&a=0&b=5"
    ```
- **Expected Behavior**:
  ```http
  HTTP/1.1 400 Bad Request
  Content-Type: application/json; charset=UTF-8
  
  {"error": "Division by zero"}
  ```
- **Actual Behavior**:
  ```http
  HTTP/1.1 200 OK
  Content-Type: application/json; charset=UTF-8
  
  {"error": "Division by zero"}
  ```
- **Code Comparison**:
  ```c
  // BROKEN
  send_http_response(client_sock, 200, "OK", "{\"error\": \"Division by zero\"}");

  // FIXED
  send_http_response(client_sock, 400, "Bad Request", "{\"error\": \"Division by zero\"}");
  ```
- **Pedagogical Takeaway**: Emphasizes REST API conventions, HTTP status semantics, and ensuring status codes match response payload types.

---

### Bug 10: Integer Truncation on Multiplication Products
- **Difficulty**: Intermediate
- **Location**: `backend/main.c`, inside `handle_calculate()`, `strcmp(op, "multiply") == 0` block.
- **What the bug does**: The product is prematurely cast to a 64-bit integer (`long long`):
  ```c
  result = (double)((long long)(a * b));
  ```
  This discards all decimal fractions from the product! For example, `2.5 * 2.5` produces `6` instead of `6.25`.
- **How to reproduce**:
  - **Frontend**: Enter First Number `2.5`, Second Number `2.5`, select **Multiply**, click **Calculate Result**.
  - **cURL**:
    ```bash
    curl "http://localhost:8080/api/calculate?op=multiply&a=2.5&b=2.5"
    ```
- **Expected Behavior**: `{"result": 6.25}`
- **Actual Behavior**: `{"result": 6}`
- **Code Comparison**:
  ```c
  // BROKEN
  result = (double)((long long)(a * b));

  // FIXED
  result = a * b;
  ```
- **Pedagogical Takeaway**: Demonstrates how explicit integer casting causes precision loss in floating-point calculations.

---

## Suggested Workshop Lesson Plan

### Phase 1: Orientation & Setup (15 mins)
1. Students clone the repo and compile the C backend using `gcc`.
2. Students open `frontend/index.html` in their web browser and observe the green connection dot.

### Phase 2: Bug Hunting & Reproduction (30 mins)
1. Assign different bugs to student pairs (e.g. Pair 1 takes Subtraction, Pair 2 takes Division, Pair 3 takes Floating Points).
2. Students formulate a hypothesis, test via the web UI and cURL, and capture the exact disparity between expected and actual results.

### Phase 3: Branching & Fixing (30 mins)
1. Students create feature/bugfix branches:
   ```bash
   git checkout -b fix/subtraction-reversed-operands
   ```
2. Students edit `backend/main.c`, recompile, and verify the fix.
3. Students commit with conventional commit messages:
   ```bash
   git commit -m "fix(backend): correct operand order in subtraction"
   ```

### Phase 4: Code Review & Pull Requests (15 mins)
1. Students open a Pull Request using the project PR template.
2. Peer review: Another student tests the branch and approves the PR.
