# Testing

Unit tests live in the **`Tests`** project ([`Tests/`](../Tests/)) — a small
x64 executable built with [GoogleTest](https://github.com/google/googletest)
(pulled via vcpkg). It compiles the self-contained modules directly and
exercises them off the game; anything that needs the live game (hooks, the UE
SDK, DirectX, the game `Logger`) is out of scope.

## What is covered

| File                       | Covers                                                        |
| -------------------------- | ------------------------------------------------------------- |
| `Tests/SettingsTests.cpp`  | `settings/Settings.cpp`: JSON round-trip, save→load, missing/corrupt files, defaults, persisted-vs-runtime fields. |
| `Tests/FeaturesTests.cpp`  | `features/FeatureRunner.h`: init-once, check gating, run-when-enabled, destroy-once on the disable edge, OneTime run/re-arm, legacy `Check()` contract. |

Feature tests use **fake features** (subclasses of `Feature` that count calls),
since the concrete features require the game. Settings tests use a fixture that
moves any real settings file aside and restores it afterwards, so running tests
never clobbers your own configuration.

## Build & run

The project auto-installs its vcpkg dependencies on first build.

```bash
# from the repo root
msbuild Splitgate.sln -t:Tests -p:Configuration=Release -p:Platform=x64
x64/Release/Tests.exe
```

In Visual Studio, tests also appear in **Test Explorer** (via the Google Test
adapter) and can be run/debugged from there.

Useful gtest flags:

```bash
x64/Release/Tests.exe --gtest_filter=FeaturesTest.*      # one suite
x64/Release/Tests.exe --gtest_list_tests                 # list all
```

## How the Tests project is wired

- Compiles the code under test in place (e.g.
  `..\Internal\settings\Settings.cpp`) plus the test `.cpp` files.
- Include paths point at `..\Internal\features` and `..\Internal\settings`.
- `vcpkg.json` declares `gtest` + `nlohmann-json`; `gtest_main` supplies
  `main()`, so there is no hand-written entry point.
- Registered in `Splitgate.sln` for `Release|x64`.

## Adding tests

Add a `*.cpp` to `Tests/`, list it in `Tests.vcxproj`, and include the header of
the module under test. Keep tests to modules that don't depend on the game; for
anything game-facing, isolate the pure logic into a testable header first (as
was done for the feature runner).
