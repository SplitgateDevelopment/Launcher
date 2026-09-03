# Launcher
🚀 Custom Splitgate launcher and ingame-dll coded in cpp

## Usage
  • Build the project or download the latest precompiled binaries from [here](https://nightly.link/SplitgateDevelopment/Launcher/workflows/msbuild/master/Release.zip)

  • Make sure Launcher.exe and Internal.dll in the same folder
  
  • Start the game
  
  • Open Launcher.exe
  
  • Use the `Ins` button to show/hide the gui

## Compile
To compile the project make sure you have [Visual Studio 22](https://visualstudio.microsoft.com/it/downloads/), [vcpkg](https://vcpkg.io/en/getting-started) integrated with `MSBuild` and any [Python](https://www.python.org/downloads/) 3.x installed

> [!IMPORTANT]
> `Internal` embeds Python (via pybind11 + `<Python.h>`), so a Python 3.x installation is required to compile.

`Internal/python.props` auto-detects the newest Python installed in a standard location (`C:\Python3XY`, `%LOCALAPPDATA%\Programs\Python\Python3XY`, or `%ProgramFiles%\Python3XY`), falling back to the install path recorded in the registry — no configuration needed for a default install.

If Python is installed somewhere custom that isn't picked up, set the `SPLITGATE_PYTHON_DIR` environment variable to your Python root (the folder containing `include\` and `libs\`), or pass `/p:PythonRoot=<dir>` to MSBuild. Whichever version is selected must match the `python3XY.dll` present at runtime.

- Install `Internal` dependencies:
  ```sh
  cd Internal
  vcpkg install
  ```
- Compile using Visual Studio
