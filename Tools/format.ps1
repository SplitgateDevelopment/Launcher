# Format (or check) the project's C++ sources with clang-format.
#
#   Tools/format.ps1          # format in place (clang-format -i)
#   Tools/format.ps1 -Check   # verify only, non-zero exit if anything differs
#
# Skips third-party code (Internal/external, vcpkg_installed) and build output.
param([switch]$Check)

$ErrorActionPreference = 'Stop'
$root = Split-Path -Parent $PSScriptRoot

$files = Get-ChildItem -Path (Join-Path $root 'Internal'), (Join-Path $root 'Launcher'), (Join-Path $root 'shared'), (Join-Path $root 'Tests') `
    -Recurse -Include *.h, *.cpp |
    Where-Object { $_.FullName -notmatch '\\external\\|\\vcpkg_installed\\|\\x64\\' } |
    Select-Object -ExpandProperty FullName

if (-not $files) { Write-Output 'No source files found.'; exit 0 }

if ($Check) {
    & clang-format --dry-run --Werror @files
} else {
    & clang-format -i @files
    Write-Output "Formatted $($files.Count) files."
}
exit $LASTEXITCODE
