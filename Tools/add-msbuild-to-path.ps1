# Adds MSBuild to the User PATH so `msbuild` (and Tools\build.bat) work from any shell.
# Locates it with vswhere (covers VS 2022/2026 and standalone Build Tools), then refreshes the
# current session's PATH so it takes effect immediately, without opening a new shell.

$msbuild = & "${env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer\vswhere.exe" -latest -prerelease -products * -requires Microsoft.Component.MSBuild -find MSBuild\**\Bin\MSBuild.exe

if (-not $msbuild) {
    Write-Host "MSBuild not found. Ensure Visual Studio 2026 or Build Tools are installed."
    return
}

# -find can return more than one match (e.g. a versioned dir and Current); take the first.
$msbuild = @($msbuild)[0]

# MSBuild.exe lives in ...\MSBuild\Current\Bin — add that Bin directory itself to PATH.
$binDir = Split-Path -Parent $msbuild

$userPath = [System.Environment]::GetEnvironmentVariable('PATH', 'User')
if (($userPath -split ';') -contains $binDir) {
    Write-Host "MSBuild path already in User PATH: $binDir"
} else {
    [System.Environment]::SetEnvironmentVariable('PATH', "$userPath;$binDir", 'User')
    Write-Host "MSBuild path added to User PATH: $binDir"
}

# Refresh this session's PATH (Machine + User) so msbuild is usable right away.
$env:PATH = [System.Environment]::GetEnvironmentVariable('PATH', 'Machine') + ';' + [System.Environment]::GetEnvironmentVariable('PATH', 'User')
