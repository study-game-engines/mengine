for %%O in (%*) do for /f "tokens=1,2 delims==" %%a in (%%O) do set "%%~a=%%~b"

echo **********************************************************************
echo SOLUTION_NAME: %SOLUTION_NAME%
echo CONFIGURATION: %CONFIGURATION%
echo GENERATOR: %GENERATOR%
echo ARCHITECTURE: %ARCHITECTURE%
echo SOURCE_DIRECTORY: %SOURCE_DIRECTORY%
echo LIBRARY_DIRECTORY: %LIBRARY_DIRECTORY%
echo **********************************************************************

set "BUILD_TEMP_DIR=..\solutions\%SOLUTION_NAME%\%CONFIGURATION%"

@mkdir %BUILD_TEMP_DIR%
@pushd %BUILD_TEMP_DIR%

CMake -G "%GENERATOR%" -A %ARCHITECTURE% -S "%SOURCE_DIRECTORY%" "-DCMAKE_CONFIGURATION_TYPES:STRING='%CONFIGURATION%'" "-DCMAKE_BUILD_TYPE:STRING='%CONFIGURATION%'" "-DMENGINE_LIB_DIR:STRING='%LIBRARY_DIRECTORY%'"

@popd

pause