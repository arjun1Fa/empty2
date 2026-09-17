<!--
Thanks for contributing to DebugCalc!

Fill in the sections below. If this is your first pull request anywhere,
don't worry about getting it perfect — a maintainer will help.

Delete any section that genuinely doesn't apply.
-->

## Type of change

<!-- Tick one. -->

- [ ] 🐛 Bug fix (backend behaviour now matches `docs/API.md`)
- [ ] 📝 Documentation / typo fix
- [ ] ✅ Test addition
- [ ] ✨ New feature (please confirm an issue was opened and agreed first)
- [ ] 🔧 Chore / tooling

---

## Problem

<!-- What was broken, in one or two sentences. -->

Fixes #

---

## Reproduction

<!-- The exact command a reviewer can paste to see the old behaviour. -->

```bash
curl "http://localhost:8080/api/calculate?op=&a=&b="
```

| | |
|---|---|
| **Expected** | |
| **Actual (before fix)** | |
| **Spec reference** | `docs/API.md` — section |

---

## Cause

<!--
What was the code in backend/main.c actually doing, and why did that
produce the wrong result? The diff shows what changed — this section
explains why it was wrong in the first place.
-->

---

## Solution

<!-- What you changed, and why this is the minimal correct fix. -->

---

## Verification

<!-- Proof. Paste curl output, a screenshot, or passing test results. -->

```text

```

---

## Checklist

- [ ] I created a branch (not committing to `main`)
- [ ] I reproduced the bug before changing any code
- [ ] My change fixes the root cause, not just the symptom
- [ ] I recompiled and re-ran my reproduction command
- [ ] I tested edge cases (negatives, decimals, zero, missing parameters)
- [ ] I confirmed the other operations still work
- [ ] My commit message follows [Conventional Commits](https://www.conventionalcommits.org/)
- [ ] I did not reformat files I didn't otherwise need to touch
- [ ] I did not commit compiled binaries (`main`, `main.exe`, `*.o`)
- [ ] I did not edit `tests/` to make a failing test pass
- [ ] I did not include content from `INSTRUCTOR_NOTES.md`

---

## Environment

| | |
|---|---|
| **OS** | <!-- e.g. Windows 11, Ubuntu 24.04, macOS 14 --> |
| **Compiler** | <!-- e.g. GCC 13.2, Clang 15 --> |

---

## Anything else?

<!--
Stuck on part of it? Unsure whether this is the right approach? Say so here.
Open questions in a PR are welcome — they're how review is supposed to work.
-->