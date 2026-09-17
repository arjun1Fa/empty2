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

