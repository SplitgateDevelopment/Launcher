set BUILD_CONFIGURATION=Release
set BUILD_PLATFORM=x64

REM Python include/lib paths are resolved by Internal\python.props (from SPLITGATE_PYTHON_DIR
REM in CI, or a local install), and the pybind11 include dir is added by Internal.vcxproj, so
REM no AdditionalLibPaths/IncludePath overrides are needed here.

msbuild /m /p:Configuration=%BUILD_CONFIGURATION% /p:Platform=%BUILD_PLATFORM% %SOLUTION_FILE_PATH%
