@echo off

rmdir /s /q .\output\
rmdir /s /q .\output_temp\

setlocal enabledelayedexpansion
set errorlevel=0

call .\build\compileIcu.bat

@echo *********************************************************
@echo Set VS2017 Build Environment
@echo *********************************************************
@call :GetVSCommonToolsDir
@if "%VS150COMNTOOLS%"=="" goto SetEnvErr
@call "%VS150COMNTOOLS%VsDevCmd.bat"

@echo Environment setting Succeeded!

@goto Continue

:GetVSCommonToolsDir
@set VS150COMNTOOLS=
@call :GetVSCommonToolsDirHelper32 HKLM > nul 2>&1
@if errorlevel 1 call :GetVSCommonToolsDirHelper32 HKCU > nul 2>&1
@if errorlevel 1 call :GetVSCommonToolsDirHelper64  HKLM > nul 2>&1
@if errorlevel 1 call :GetVSCommonToolsDirHelper64  HKCU > nul 2>&1
@exit /B 0

:GetVSCommonToolsDirHelper32
@for /F "tokens=1,2*" %%i in ('reg query "%1\SOFTWARE\Microsoft\VisualStudio\SxS\VS7" /v "15.0"') DO (
    @if "%%i"=="15.0" (
        @SET VS150COMNTOOLS=%%k
    )
)
@if "%VS150COMNTOOLS%"=="" exit /B 1
@SET VS150COMNTOOLS=%VS150COMNTOOLS%Common7\Tools\
@exit /B 0

:GetVSCommonToolsDirHelper64
@for /F "tokens=1,2*" %%i in ('reg query "%1\SOFTWARE\Wow6432Node\Microsoft\VisualStudio\SxS\VS7" /v "15.0"') DO (
    @if "%%i"=="15.0" (
        @SET VS150COMNTOOLS=%%k
    )
)
@if "%VS150COMNTOOLS%"=="" exit /B 1
@SET VS150COMNTOOLS=%VS150COMNTOOLS%Common7\Tools\
@exit /B 0

:Continue

:BuildByVS
echo Compiling solution by 'msbuild.exe'
echo %1=%2
if "%1"=="" (
    @goto BilidNoPara
)
if "%2"=="" (
    @goto BilidNoPara  
)
msbuild.exe ./build/DuiLib.sln /m /t:rebuild /p:Platform=x64 /p:Configuration=Release %1=%2 /nr:false
@goto BilidEnd
  
:BilidNoPara
msbuild.exe ./build/DuiLib.sln /m /t:rebuild /p:Platform=x64 /p:Configuration=Release /nr:false
:BilidEnd

xcopy /yr %cd%\src\*.h %cd%\output_temp\include\Duilib\
xcopy /yr %cd%\src\*.hpp %cd%\output_temp\include\Duilib\
xcopy /yr %cd%\src\*.tlh %cd%\output_temp\include\Duilib\
    
for  %%I in (Control,Core,Debug,Layout,Utils) do (
    xcopy /yr %cd%\src\%%I\*.h %cd%\output_temp\include\Duilib\%%I\
    xcopy /yr %cd%\src\%%I\*.hpp %cd%\output_temp\include\Duilib\%%I\
    xcopy /yr %cd%\src\%%I\*.tlh %cd%\output_temp\include\Duilib\%%I\
)

xcopy /yr %cd%\src\*.h %cd%\output_temp\include\Duilib\
xcopy /yr %cd%\src\*.hpp %cd%\output_temp\include\Duilib\
xcopy /yr %cd%\src\*.tlh %cd%\output_temp\include\Duilib\

xcopy /s /e /q /yr %cd%\output_temp\bin_x64 %cd%\output\bin_x64\
xcopy /s /e /q /yr %cd%\output_temp\lib_x64 %cd%\output\lib_x64\
xcopy /s /e /q /yr %cd%\output_temp\pdb_x64 %cd%\output\pdb_x64\
xcopy /s /e /q /yr %cd%\output_temp\include %cd%\output\include\

:ExitBuild
@echo compile end