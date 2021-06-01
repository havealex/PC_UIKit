@echo off
setlocal enabledelayedexpansion
set errorlevel=0

SET "LIB_PATH=%cd%\output_temp\lib_x64\Release\"
SET "DLL_PATH=%cd%\output_temp\bin_x64\Release\"
SET "PDB_PATH=%cd%\output_temp\pdb_x64\Release\"

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
msbuild.exe ./opensource/icu4c/icu/source/allinone/allinone.sln /m /t:rebuild /p:Platform=x64 /p:Configuration=Release /nr:false
msbuild.exe ./opensource/icu4c/icu/source/allinone/allinone.sln /m /t:build /p:Platform=x64 /p:Configuration=Release /nr:false

pushd .\opensource\icu4c
xcopy icu\lib64\icudt.lib %LIB_PATH% /y
xcopy icu\lib64\icuin.lib %LIB_PATH% /y
xcopy icu\lib64\icutu.lib %LIB_PATH% /y
xcopy icu\lib64\icuuc.lib %LIB_PATH% /y
xcopy icu\lib64\icuio.lib %LIB_PATH% /y

xcopy icu\lib64\icuin.pdb %PDB_PATH% /y
xcopy icu\lib64\icutu.pdb %PDB_PATH% /y
xcopy icu\lib64\icuuc.pdb %PDB_PATH% /y
xcopy icu\lib64\icuio.pdb %PDB_PATH% /y

xcopy icu\bin64\icudt67.dll %DLL_PATH% /y
xcopy icu\bin64\icuin67.dll %DLL_PATH% /y
xcopy icu\bin64\icutu67.dll %DLL_PATH% /y
xcopy icu\bin64\icuuc67.dll %DLL_PATH% /y
xcopy icu\bin64\icuio67.dll %DLL_PATH% /y
xcopy icu\bin64\genrb.exe %DLL_PATH% /y
popd

:GenInclude

xcopy /yr %cd%\opensource\icu4c\icu\include\unicode\*.h %cd%\output_temp\include\opensource\icu4c\unicode\
xcopy /yr %cd%\opensource\icu4c\icu\include\unicode\*.hpp %cd%\output_temp\include\opensource\icu4c\unicode\

:ExitBuild
@echo compileicu end