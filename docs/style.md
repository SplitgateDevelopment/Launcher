# Style & linting

C++ formatting and linting for this repo — the equivalent of SwiftLint / Biome:

- **[`.clang-format`](../.clang-format)** — the formatter (layout: tabs, Allman
  braces, indented namespaces). Tuned to the existing style so it's low-churn.
- **[`.clang-tidy`](../.clang-tidy)** — the linter (a curated set of
  correctness / performance / modernization checks).

Both are standard tools shipped with LLVM and with Visual Studio (the
"C++ Clang tools for Windows" component). The naming conventions themselves are
described in [`.claude/rules/code-style.md`](../.claude/rules/code-style.md).

## Formatting

```powershell
# format every C++ source in place (skips external/ and build output)
Tools/format.ps1

# verify only — non-zero exit if anything is unformatted (use in CI / pre-commit)
Tools/format.ps1 -Check
```

Or directly: `clang-format -i Internal/settings/Settings.cpp`. Editors (VS,
VS Code) pick up `.clang-format` automatically for format-on-save.

## Linting

`clang-tidy` needs to know how each file is compiled. The easy path is a
compilation database; MSBuild can emit one, or run it per file with the include
paths after `--`:

```powershell
clang-tidy Internal/settings/Settings.cpp -- -std=c++latest -I Internal
```

`HeaderFilterRegex` limits diagnostics to first-party headers (external code is
skipped). Tighten the `Checks:` list in `.clang-tidy` as the codebase adopts
more of them.

## CI

`Tools/format.ps1 -Check` is the piece to add as a CI step (or a pre-commit
hook) to keep formatting enforced; it exits non-zero on any drift.
