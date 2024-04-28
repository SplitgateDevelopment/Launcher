set BUILD_CONFIGURATION=Release

set AdditionalLibPaths="%pythonLocation%\libs"
set IncludePath="$(ProjectDir)\external\pybind11\include;%pythonLocation%\include;$(IncludePath)"

msbuild /m /p:Configuration=%BUILD_CONFIGURATION% /p:AdditionalLibPaths=%AdditionalLibPaths% /p:IncludePath=%IncludePath% %SOLUTION_FILE_PATH%