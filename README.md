# Launcher
🚀 Custom Splitgate launcher and ingame-dll coded in cpp

## Usage
  • Build the project or download the latest precompiled binaries from [here](https://nightly.link/SplitgateDevelopment/Launcher/workflows/msbuild/master/Release.zip)

  • Make sure Launcher.exe and Internal.dll in the same folder
  
  • Start the game
  
  • Open Launcher.exe
  
  • Use the `Ins` button to show/hide the gui

## Compile
To compile the project make you you have [Visual Studio 22](https://visualstudio.microsoft.com/it/downloads/), [vcpkg](https://vcpkg.io/en/getting-started) integrated with `MSBuild` and Python 3.13 installed

> [!IMPORTANT]
> Internal.vcxproj expects python path to be `C:\Python313`, either match it or update header import configuration

- Install `Internal` dependencies:
  ```sh
  cd Internal
  vcpkg install
  ```
- Compile using Visual Studio
