# Contributing to DebugCalc

Thanks for being here. DebugCalc exists specifically to be contributed to by people who have never contributed to anything before, so if this is your first pull request anywhere — you are in exactly the right repository.

This document covers how to set up the project, how to pick something to work on, and what a good pull request looks like here.

---

## Table of Contents

- [Ground rules](#ground-rules)
- [Setting up locally](#setting-up-locally)
- [Choosing what to work on](#choosing-what-to-work-on)
- [The workflow](#the-workflow)
- [Writing a good bug report](#writing-a-good-bug-report)
- [Commit messages](#commit-messages)
- [Pull request expectations](#pull-request-expectations)
- [C style guide](#c-style-guide)
- [Review process](#review-process)
- [Getting help](#getting-help)

---

## Ground rules

1. **The frontend is not broken.** `frontend/` is complete and correct. If the calculator gives a wrong answer, the cause is in `backend/main.c`. Please don't submit "fixes" to the frontend.
2. **The specification wins.** [`docs/API.md`](docs/API.md) is the source of truth. A bug exists whenever the backend disagrees with it.
3. **Fix the cause, not the symptom.** Special-casing an input to produce the right answer is not a fix.
4. **One pull request, one idea.** Don't bundle a bug fix with a refactor and a formatting pass.
5. **Never commit directly to `main`.**

---

## Setting up locally

### Prerequisites

| Platform | Compiler |
|---|---|
| Windows | [MinGW-w64 / GCC](https://www.msys2.org/) or Git for Windows SDK |
| Linux | GCC — `sudo apt install build-essential` |
| macOS | Clang — `xcode-select --install` |

Plus **Git**, configured with your name and email.

### Fork, clone, build

```bash
git clone https://github.com/<your-username>/debugcalc.git
cd debugcalc
git remote add upstream https://github.com/your-org/debugcalc.git
```

Build the backend:

```bash
make -C backend
```

If `make` isn't available, compile directly from inside `backend/`:

```bash
gcc main.c -o main -lws2_32   # Windows
gcc main.c -o main -lm        # Linux / macOS
```

Run it, then open `frontend/index.html`. Full setup notes and troubleshooting live in [`docs/SETUP.md`](docs/SETUP.md).

### Optional: the Python test suite

```bash
pip install -r requirements.txt
pytest -q
```

**These tests are supposed to fail right now.** They encode the API specification, and the backend currently violates parts of it. Every test you turn green is a bug you fixed. Don't edit the tests to make them pass.

---

## Choosing what to work on

### If this is your first pull request ever

Start with **the Typo Hunt** in the [README](README.md#-first-contribution-the-typo-hunt). It's a documentation-only exercise that walks you through the entire fork → branch → commit → PR loop with no C involved.

### If you want to fix a bug

1. Play with the calculator and try to break it. Negative numbers, decimals, zero, very large values, missing parameters, an operation that doesn't exist.
2. Check the [open issues](../../issues) first — someone may have already claimed it.
3. If it's unreported, open an issue using the bug report template before you start writing code.
4. If it's reported and unclaimed, comment saying you'd like to take it, and wait for a maintainer to assign it to you.

Claiming an issue costs you a day of waiting and saves two people from writing the same patch.

### If you want to add something

Open an issue describing the idea before writing code. DebugCalc is deliberately minimal — features that make the codebase harder for beginners to read are usually declined, even when they're good ideas.

---

## The workflow

### 1. Create a branch

```bash
git checkout main
git pull upstream main
git checkout -b fix/subtraction-reversed-operands
```

Branch naming:

| Prefix | Use for |
|---|---|
| `fix/` | Bug fixes |
| `feat/` | New functionality |
| `docs/` | Documentation only |
| `test/` | Test additions |
| `chore/` | Build config, tooling, cleanup |

### 2. Reproduce before you edit

Do not open the editor yet. Prove the bug exists first:

```bash
curl "http://localhost:8080/api/calculate?op=subtract&a=10&b=3"
```

Record three things: what you expected, what you got, and the exact command. A bug you can't reproduce on demand is a bug you can't prove you fixed.

### 3. Find the root cause

Open `backend/main.c` and read the relevant section slowly, comments included. The comments are not always accurate — treat them as testimony, not evidence. [`docs/DEBUGGING.md`](docs/DEBUGGING.md) has techniques if you get stuck.

### 4. Make the minimal fix

Change as few lines as possible. If your diff is larger than about ten lines for a single-operation bug, you are probably fixing the wrong thing.

### 5. Rebuild and verify

```bash
make -C backend
```

Then:

1. Re-run your reproduction command.
2. Test the edge cases around your fix.
3. Confirm the other three operations still behave.
4. Run `pytest -q` if you're using the test suite, and check you didn't turn a passing test red.

### 6. Commit and push

```bash
git add backend/main.c
git commit -m "fix(backend): correct operand order in subtraction"
git push -u origin fix/subtraction-reversed-operands
```

### 7. Open the pull request

Open it against `main` and fill in the template. Link the issue with `Fixes #42` so it closes automatically when merged.

---

## Writing a good bug report

A useful report answers four questions:

- **What did you do?** The exact request, copy-pasteable.
- **What did you expect?** Quote the line in `docs/API.md` that says so.
- **What happened instead?** The literal response body and status code.
- **What's your environment?** OS, compiler, and version.

Compare:

> ❌ "division is broken"

> ✅ "`GET /api/calculate?op=divide&a=7&b=2` returns `{"result": 3}`. The spec says operands are numbers and doesn't restrict results to integers, so I expected `3.5`. Linux, GCC 13.2."

The second one can be fixed by someone who has never spoken to you.

---

## Commit messages

We use [Conventional Commits](https://www.conventionalcommits.org/):

```text
<type>(<scope>): <short description in the imperative>
```

| Type | Meaning |
|---|---|
| `fix` | A bug fix |
| `feat` | New functionality |
| `docs` | Documentation only |
| `test` | Adding or correcting tests |
| `refactor` | Code change that neither fixes a bug nor adds a feature |
| `chore` | Tooling, build, dependencies |

Examples:

```text
fix(backend): correct operand order in subtraction
fix(backend): return 400 when operand parameters are missing
docs(readme): correct spelling errors in the mission briefing
test(api): add coverage for decimal division
```

Write the subject line in the imperative — "correct operand order", not "corrected" or "corrects". If you need to explain *why*, add a blank line and a body paragraph. The diff already shows what changed; only you know why.

---

## Pull request expectations

Your PR should:

- Target `main` from a descriptively named branch
- Change one thing
- Include the reproduction command in the description
- Show verification — `curl` output, a screenshot, or a now-passing test
- Touch only the files it needs to

Your PR should **not**:

- Reformat files you didn't otherwise change
- Bump dependencies unrelated to the fix
- Include compiled binaries (`main`, `main.exe`, `*.o`) — these are gitignored for a reason
- Edit `tests/` to make a failing test pass
- Touch `INSTRUCTOR_NOTES.md`

Open it as a **draft** if it isn't finished. Draft PRs invite early feedback; broken "ready" PRs invite silence.

---

## C style guide

Match the surrounding code. Where the existing code is inconsistent, prefer:

- **Indentation:** 4 spaces, no tabs
- **Braces:** opening brace on the same line
- **Naming:** `snake_case` for functions and variables, `UPPER_SNAKE_CASE` for macros
- **Line length:** keep it under about 100 characters
- **Comments:** explain *why*, not *what*. `// increment i` helps nobody
- **No new dependencies.** The backend uses only the C standard library and platform sockets, on purpose
- **Portability:** guard platform-specific code with `#ifdef _WIN32` / `#else`, the way the existing socket setup does
- **Buffers:** prefer `snprintf` over `sprintf`, and always bound your string copies

---

## Review process

1. A maintainer reads your PR, usually within a week. Most contributors here are volunteers and students, so silence is not rejection.
2. You'll likely get change requests. This is normal and happens to everyone, including maintainers.
3. Respond with commits rather than arguments. If you disagree, say so once with your reasoning, then defer — it's their call.
4. "I don't understand this feedback" is a perfectly good comment. Asking is faster than guessing wrong twice.
5. Once approved, a maintainer merges. Don't force-push after review has started unless asked; it makes the review history unreadable.

Review comments are about the code, not about you. This is the hardest thing to internalise and the most important.

---

## Getting help

- **Stuck on the C?** Open a draft PR with what you have and ask in the description.
- **Stuck on Git?** Open an issue with the `question` label. Nobody here was born knowing `git rebase`.
- **Ask in public.** Post questions in the issue thread rather than DMing a maintainer — the next person searching for the same problem will find your answer.

And if a maintainer reviewed your code for free, it costs nothing to say thanks.