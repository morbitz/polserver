@echo off

rem Attempts to find the most recent visual studio.

:VS2015
set VCVARSALL="%VS140COMNTOOLS%..\..\VC\vcvarsall.bat"
set POLSOL="pol-2015.sln"
if not exist %VCVARSALL% goto VS2013
goto begin

:VS2013
set VCVARSALL="%VS120COMNTOOLS%..\..\VC\vcvarsall.bat"
set POLSOL="pol-2013.sln"
if not exist %VCVARSALL% goto VS2012
goto begin

:VS2012
set VCVARSALL="%VS110COMNTOOLS%..\..\VC\vcvarsall.bat"
set POLSOL="pol-2012.sln"
if not exist %VCVARSALL% goto vserror
goto begin



:begin
set STARTTIME=%TIME%

echo Extracting boost libraries
pushd ..\lib
call prepare_boost.bat
if %errorlevel% neq 0 goto :error
popd


call %VCVARSALL%
if %errorlevel% neq 0 goto :error

rem echo Building Win32-release from %POLSOL%...
rem msbuild %POLSOL% /m /p:Configuration=Release /p:Platform="Win32" > dist\buildlog_32.log
rem if %errorlevel% neq 0 goto :error

echo Building x64-release from %POLSOL%...
msbuild %POLSOL% /m /p:Configuration=Release /p:Platform="x64" > dist\buildlog_64.log
if %errorlevel% neq 0 goto :error

echo Packing everything up...
pushd dist
call mkdist x64 clean
call mkdist x64 dist 
call mkdist x64 pdb
if %errorlevel% neq 0 goto :error

rem call mkdist x86 clean
rem call mkdist x86 dist
rem call mkdist x86 pdb
if %errorlevel% neq 0 goto :error


rem Removed debug because its contents are mostly in src, lib and obj.
rem call mkdist x64 debug
rem call mkdist x64 lib
rem call mkdist x64 src
if %errorlevel% neq 0 goto :error

popd

goto end

:vserror
echo Couldnt find a suitable vcvarsall.bat.
pause
exit 1

:error
echo Something went wrong. Check the logs in the 'dist' folder.
pause
exit 1

:end
echo Done. Started at %STARTTIME%, finished at %TIME%.
pause
