@echo off

set "SOLUTION_NAME=solution_mingw64_publish"
set "SOURCE_DIRECTORY=%~dp0..\..\cmake\MinGW64"
set "GENERATOR=Ninja"
set "BUILD_PUBLISH=ON"

@echo Starting build %SOLUTION_NAME% configuration...

@call %~dp0../build_solution.bat %* "SOLUTION_NAME=%SOLUTION_NAME%" "SOURCE_DIRECTORY=%SOURCE_DIRECTORY%" "GENERATOR=%GENERATOR%" "BUILD_PUBLISH=%BUILD_PUBLISH%"

exit /b %errorlevel%