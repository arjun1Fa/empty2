# DebugCalc 🧮

[![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg)](LICENSE)
[![Platform](https://img.shields.io/badge/Platform-Windows%20%7C%20Linux%20%7C%20macOS-brightgreen.svg)]()
[![Backend](https://img.shields.io/badge/Backend-Pure%20C-orange.svg)]()
[![Frontend](https://img.shields.io/badge/Frontend-HTML5%20%2F%20CSS3%20%2F%20JS-blue.svg)]()

**DebugCalc** is a beginner-friendly open-source debugging workshop repository. It gives students and new open-source contributors a realistic, lightweight sandbox to practice **Git workflows**, **reading existing code**, **software debugging**, **HTTP APIs**, and submitting their first **Pull Requests**.

---

## 🎯 Priject Purpose

In real-world software engineering, developers rarely write code from scratch. Most time is spent:

1. Reading and understanding someone else's codebase.
2. Reproducing bug reports filed by users.
3. Forming a hypothesis and locating the root cause in code.
4. Implementing a targeted, minimal fix without breaking existing behavior.
5. Creating branches and submitting clean pull requests with clear documentation.

DebugCalc provides exactly that experience:

- The **frontend** is 100% complete, fully functional, and bug-free.
- The **backend** is lightweight, cross-platform **C**, and intentionally contains subtle logic bugs, edge-case flaws, and protocol discrepancies waiting for you to find.

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
│  - Native HTTP socket server listening on :8080        │
│  - Request parser & parameter validator                │
│  - Mathematical calculation engine                     │
│  - JSON response formatter (200 OK / 400 Bad Request)  │
└────────────────────────────────────────────────────────┘
```

### Communication Protocol

The frontend talks to the backend over HTTP `GET` requests on port `8080`. All responses are UTF-8 JSON with standard CORS headers (`Access-Control-Allow-Origin: *`), so the frontend works whether you open it directly from disk or host it on a local web server.

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
├── README.md            # You are here
├── INSTRUCTOR_NOTES.md  # Instructor answer key (do not distribute to students)
├── .gitignore           # Ignores compiled binaries & build artifacts
└── LICENSE              # MIT License
```

---

## 🚀 Quickstart Guide

### 1. Prerequisites

You need a C compiler installed:

| Platform | Compiler |
|---|---|
| **Windows** | [MinGW-w64 / GCC](https://www.msys2.org/) or [Git for Windows SDK](https://git-scm.com/) |
| **Linux** | GCC — `sudo apt install build-essential` or `sudo dnf groupinstall "Development Tools"` |
| **macOS** | Clang / Xcode Command Line Tools — `xcode-select --install` |

You also need **Git**, configured with your name and email.

---

### 2. Compiling & Running the Backend

Navigate to the project directory:

```bash
cd debugcalc
```

#### Option A — Using Make (recommended)

```bash
make -C backend
```

#### Option B — If `make` doesn't work, compile manually with GCC

**Windows** — link the Winsock library with `-lws2_32`:

```powershell
gcc main.c -o main -lws2_32
```

**Linux & macOS** — link the math library with `-lm`:

```bash
gcc main.c -o main -lm
```

> Run these from inside the `backend/` folder. If you'd rather stay in the project root, use the full path instead:
> ```bash
> gcc backend/main.c -o backend/main -lws2_32   # Windows
> gcc backend/main.c -o backend/main -lm        # Linux / macOS
> ```

#### Run the server

```powershell
.\backend\main.exe     # Windows
```
```bash
./backend/main         # Linux / macOS
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

> **Port 8080 already in use?** Find what's using it with `netstat -ano | findstr :8080` (Windows) or `lsof -i :8080` (Linux/macOS), then stop that process or change the port in `main.c`.

---

### 3. Launching the Frontend

Open `frontend/index.html` in your browser:

```powershell
start frontend/index.html     # Windows
```
```bash
open frontend/index.html      # macOS
xdg-open frontend/index.html  # Linux
```

Or serve it with any static local server:

```bash
npx serve frontend
python -m http.server 3000 -d frontend
```

Check the status indicator in the upper-right corner:

- 🟢 **Backend Online (Connected Port 8080)** — everything is ready.
- 🔴 **Backend Offline** — your C server isn't running. Go back to step 2.

---

## 📡 API Specification

### 1. Health Check

Checks whether the backend is online and accepting connections.

- **URL**: `/health`
- **Method**: `GET`
- **Success Response**: `200 OK`

```json
{ "status": "ok" }
```

---

### 2. Arithmetic Calculation

Evaluates an operation between two numeric operands `a` and `b`.

- **URL**: `/api/calculate`
- **Method**: `GET`

| Parameter | Type | Required | Description |
|---|---|---|---|
| `op` | String | Yes | Operation: `add`, `subtract`, `multiply`, `divide` |
| `a` | Number | Yes | First operand (e.g. `10`, `3.14`, `-5`) |
| `b` | Number | Yes | Second operand (e.g. `20`, `2`, `0.5`) |

**Successful calculation — `200 OK`**

```bash
curl "http://localhost:8080/api/calculate?op=add&a=10&b=20"
```
```json
{ "result": 30 }
```

**Error response — `400 Bad Request`**

```bash
curl "http://localhost:8080/api/calculate?op=divide&a=10&b=0"
```
```json
{ "error": "Division by zero" }
```

> This specification is the source of truth. A bug exists whenever the backend's behavior disagrees with this section.

---

## 🛠️ Contribution Workflow for Students

### Step 1 — Clone the repository

```bash
git clone https://github.com/your-org/debugcalc.git
cd debugcalc
```

### Step 2 — Create a dedicated branch

Never commit directly to `main`. Name your branch after the bug you're tackling:

```bash
git checkout -b fix/subtraction-reversed-operands
```

Naming convention: `fix/<issue-name>` or `feature/<feature-name>`.

### Step 3 — Reproduce and confirm the bug

Before changing any code:

1. Start the backend.
2. Open the frontend, or use `curl`:
   ```bash
   curl "http://localhost:8080/api/calculate?op=subtract&a=10&b=3"
   ```
3. Note what you expected (`7`) versus what you got (`-7`).
4. Write down the exact reproduction steps.

### Step 4 — Locate and fix the issue

Open `backend/main.c`:

- Search for the relevant function or operation (`strcmp(op, "subtract")`).
- Read the surrounding logic and comments carefully.
- Implement the minimal, correct fix — fix the cause, not the symptom.

### Step 5 — Recompile and verify

```powershell
gcc main.c -o main -lws2_32    # Windows
```
```bash
gcc main.c -o main -lm         # Linux / macOS
```

Then:

1. Re-run your reproduction command to confirm the bug is gone.
2. Test edge cases — negatives, decimals, zero, missing parameters, unknown operations.
3. Confirm the other operations still work.

### Step 6 — Commit your changes

Use a [Conventional Commit](https://www.conventionalcommits.org/) message:

```bash
git add backend/main.c
git commit -m "fix(backend): correct operand order in subtraction"
```

### Step 7 — Push and open a Pull Request

```bash
git push -u origin fix/subtraction-reversed-operands
```

Then open a Pull Request against `main`.

**PR description checklist:**

- **Problem** — what was broken?
- **Reproduction** — how can reviewers reproduce the old behavior?
- **Cause** — why was the bug happening in `backend/main.c`?
- **Solution** — how do your changes fix it?
- **Verification** — proof that it works now (screenshots or `curl` output).

---

## 🕵️ First Contribution: The Typo Hunt

Not ready to take on the C backend yet? Start here. Documentation fixes are real contributions, and typo hunting is how a lot of open-source contributors make their first PR.

**The paragraph below contains exactly 11 deliberate spelling mistakes.** Find them, fix them, and open a PR on a branch named `fix/readme-typos`.

> **📼 Mission Briefing** *(practice zone — the typos below are intentional)*
>
> Wellcome to the DebugCalc firmware team. Our calcualtor has been deployed to over three thousand labratories world-wide, and the maintainence crew has gone dark. Your mision, should you choose to acept it, is to restore the arithmatic core before the next release windo closes. Remeber: every fix begins with a reproducable bug report, and every pull request tells a story. Good luck, contributer.

**Rules of the hunt:**

- Fix only the typos. Don't rewrite the prose or reflow the paragraph.
- Change nothing else in this file. A PR that fixes eleven words should show a diff of eleven words.
- List every correction in your PR description as `misspelling → correction`.
- If you spot a twelfth mistake elsewhere in the README, that one is genuine — file a separate issue for it.

> ⚠️ **Maintainers:** this block is intentional. Reject PRs that silently "correct" it outside the exercise, and restore it if a linter flattens it.

---

## 💡 Debugging Tips for Beginners

**1. Watch the terminal logs.** Every request prints a line with the path, query parameters, status code, and result:

```text
[INFO] Handled GET /api/calculate?op=add&a=10&b=20 -> 200 OK (result: 30)
```

If the log disagrees with the browser, that tells you which side the bug is on.

**2. Use the in-page API Inspector.** The panel at the bottom of the page shows the outgoing URL and the raw JSON your C code returned.

**3. Inspect HTTP status codes.** Add `-i` to curl to see the raw headers:

```bash
curl -i "http://localhost:8080/api/calculate?op=divide&a=0&b=5"
```

A wrong status code with the right answer is still a bug.

**4. Change one thing at a time.** If you edit four lines and behavior changes, you won't know which line did it.

**5. `printf` is a debugger.** Printing `a`, `b`, and `op` right after parsing will solve most bugs here. `gdb` and `lldb` are there when you need more.

**6. Suspect the boundaries.** Most bugs live where values cross a line: string → number, request → parser, calculation → JSON.

---

## 🌍 Tips for Contributing to Open Source

These apply well beyond this repository.

### Before you write code

- **Read `CONTRIBUTING.md` first.** Most projects have one, and maintainers can tell instantly who skipped it.
- **Search existing issues.** Your bug has probably been reported. Adding context to an existing thread beats opening a duplicate.
- **Claim the issue.** Comment that you'd like to work on it and wait for confirmation, so two people don't write the same patch.
- **Start small on purpose.** Typos, documentation gaps, a missing test — these teach you the workflow while the stakes are low.

### While you work

- **Match the project's style.** Their brace placement, naming, and indentation — not your personal preferences.
- **One PR, one idea.** A fix bundled with a refactor and a formatting pass will sit unreviewed.
- **Don't reformat files you didn't need to touch.** A two-line fix buried in a 400-line whitespace diff is unreviewable.
- **Keep your branch current.** Pull `main` before pushing; resolving conflicts is your job, not the reviewer's.

### When you open the PR

- **Write for someone with no context.** Problem, cause, fix, proof.
- **Link the issue** with `Fixes #42` so it closes automatically.
- **Explain the *why*, not the *what*.** The diff already shows what changed.
- **Mark it as a draft** if it isn't finished. Draft PRs invite early feedback.

### When review comes back

- **Review comments are about the code, not about you.** This is the hardest lesson and the most important one.
- **Respond with commits, not arguments.** If you disagree, say so once with reasoning, then defer.
- **"I don't understand this feedback" is a fine comment.** Asking is faster than guessing wrong twice.
- **Be patient.** Most maintainers are volunteers. A week of silence is normal; two weeks earns one polite follow-up.

### Things nobody tells you

- **Documentation counts.** So does issue triage, reproducing other people's bugs, and answering questions.
- **A rejected PR isn't a failure.** Scope and timing decisions often have nothing to do with quality. Ask what would have worked.
- **Ask in public.** Post questions in the issue thread, not in DMs — the next person searching will find your answer.

---

## 🤝 Code of Conduct

Be decent and assume good faith. Beginner questions are welcome in the issue tracker; this repository exists precisely because everyone starts somewhere.

---

## 📜 License

This project is licensed under the MIT License — see [LICENSE](LICENSE) for details.
