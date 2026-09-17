# DebugCalc 🧮

[![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg)](LICENSE)
[![Platform](https://img.shields.io/badge/Platform-Windows%20%7C%20Linux%20%7C%20macOS-brightgreen.svg)]()
[![Backend](https://img.shields.io/badge/Backend-Pure%20C-orange.svg)]()
[![Frontend](https://img.shields.io/badge/Frontend-HTML5%20%2F%20CSS3%20%2F%20JS-blue.svg)]()

**DebugCalc** is a beginner-friendly open-source debugging workshop repository. It provides students and new open-source contributors with a realistic, lightweight sandbox to practice **Git workflows**, **reading existing code**, **software debugging**, **HTTP APIs**, and submitting their very first **Pull Requests (PRs)**!

---

## 🎯 Project Purpose

In real-world software engineering, developers rarely write code from scratch. Most time is spent:
1. Reading and understanding someone else's codebase.
2. Reproducing bug reports filed by users.
3. Formulating a hypothesis and locating the root cause in code.
4. Implementing a targeted, minimal fix without breaking existing behavior.
5. Creating feature/bugfix branches and submitting clean pull requests with descriptive documentation.

**DebugCalc** provides this exact experience:
- The **Frontend** is 100% complete, fully functional, and bug-free.
- The **Backend** is written in lightweight, cross-platform **C** and intentionally contains subtle logic bugs, edge-case flaws, and protocol discrepancies waiting for you to solve!

---

## 🏛️ Architecture & System Design

```text
┌────────────────────────────────────────────────────────┐
│                   Web Browser                          │
│  frontend/index.html + styles.css + app.js             │
│  - User inputs operands a & b                          │
│  - Selects operation (+, -, *, /)                      │
│  - Live API Inspector displays raw HTTP request & JSON │
└───────────────────────────┬────────────────────────────┘
                            │
                            │ HTTP GET (CORS Enabled)
                            │ e.g. /api/calculate?op=add&a=10&b=20
                            ▼
┌────────────────────────────────────────────────────────┐
│                   C Backend                            │
│  backend/main.c                                        │
│  - Native HTTP Socket Server listening on :8080        │
│  - Request Parser & Parameter Validator                │
│  - Mathematical Calculation Engine                     │
│  - JSON Response Formatter (200 OK / 400 Bad Request)  │
└────────────────────────────────────────────────────────┘
```

### Communication Protocol
The frontend speaks directly to the backend over HTTP GET queries on port `8080`.
All responses are formatted in UTF-8 JSON with standard CORS headers (`Access-Control-Allow-Origin: *`), meaning the frontend can be opened directly from disk or hosted on any local web server.

---

## 📁 Repository Structure

```text
debugcalc/
│
├── frontend/
│   ├── index.html       # Calculator UI & API Inspector
│   ├── styles.css       # Responsive dark theme & animations
│   └── app.js           # Client HTTP logic & live health monitor
│
├── backend/
│   ├── main.c           # Pure C HTTP server & calculation engine
│   └── Makefile         # Build configuration for Linux / macOS / Windows
│
├── README.md            # You are here!
├── INSTRUCTOR_NOTES.md  # Instructor answer key (Do not distribute to students)
├── .gitignore           # Ignores compiled binaries & build artifacts
└── LICENSE              # MIT License
```

---

## 🚀 Quickstart Guide

### 1. Prerequisites

You will need a C compiler installed on your system:
- **Windows**: [MinGW-w64 / GCC](https://www.msys2.org/) or [Git for Windows SDK](https://git-scm.com/)
- **Linux**: GCC (`sudo apt install build-essential` or `sudo dnf groupinstall "Development Tools"`)
- **macOS**: Clang / Xcode Command Line Tools (`xcode-select --install`)
- **Git**: Installed and configured with your name and email.

---

### 2. Compilation & Running the Backend

Open your terminal or command prompt and navigate to the project directory:

```bash
cd debugcalc
```

#### Windows (PowerShell / Command Prompt)
Compile with GCC linking the Windows Socket library (`-lws2_32`):
```powershell
gcc backend/main.c -o backend/debugcalc.exe -lws2_32
```
Run the executable:
```powershell
.\backend\debugcalc.exe
```

#### Linux & macOS (Bash / Zsh)
Compile using `gcc` or `make`:
```bash
gcc backend/main.c -o backend/debugcalc -lm
```
*(Or simply run `make -C backend`)*

Run the server:
```bash
./backend/debugcalc
```

You should see:
```text
====================================================
  DebugCalc C Backend v1.0
  Listening on http://localhost:8080
  Health Check: http://localhost:8080/health
  Calculate:    http://localhost:8080/api/calculate?op=add&a=10&b=20
====================================================
[INFO] Server ready to accept connections. Press Ctrl+C to terminate.
```

---

### 3. Launching the Frontend

Simply open `frontend/index.html` in your favorite web browser:
- **Windows**: Double-click `frontend/index.html` or run:
  ```powershell
  start frontend/index.html
  ```
- **macOS**:
  ```bash
  open frontend/index.html
  ```
- **Linux**:
  ```bash
  xdg-open frontend/index.html
  ```

Alternatively, you can serve it with any static local server (e.g. `npx serve frontend` or `python -m http.server 3000 -d frontend`).

When opened, check the upper-right corner:
- 🟢 **Backend Online (Connected Port 8080)**: Everything is ready!
- 🔴 **Backend Offline**: Make sure your C server is running in your terminal.

---

## 📡 API Specification

### 1. Health Check
Checks if the backend is online and accepting connections.

- **URL**: `/health`
- **Method**: `GET`
- **Success Response**: `200 OK`
  ```json
  {
    "status": "ok"
  }
  ```

---

### 2. Arithmetic Calculation
Evaluates a mathematical operation between two numeric operands `a` and `b`.

- **URL**: `/api/calculate`
- **Method**: `GET`
- **Query Parameters**:
  | Parameter | Type | Required | Description |
  |---|---|---|---|
  | `op` | String | Yes | Mathematical operation: `add`, `subtract`, `multiply`, `divide` |
  | `a` | Number | Yes | First operand (e.g. `10`, `3.14`, `-5`) |
  | `b` | Number | Yes | Second operand (e.g. `20`, `2`, `0.5`) |

#### Successful Calculation
- **Status**: `200 OK`
- **Example Request**: `GET /api/calculate?op=add&a=10&b=20`
- **Response**:
  ```json
  {
    "result": 30
  }
  ```

#### Error Response
- **Status**: `400 Bad Request`
- **Example Request**: `GET /api/calculate?op=divide&a=10&b=0`
- **Response**:
  ```json
  {
    "error": "Division by zero"
  }
  ```

---

