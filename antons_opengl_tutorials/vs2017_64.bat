@echo off

rem VS120COMNTOOLS=C:\Program Files (x86)\Microsoft Visual Studio 12.0\Common7\Tools\
set VS140COMNTOOLS=C:\Program Files (x86)\Microsoft Visual Studio\2017\Professional\Common7\Tools\
set VSPATH=C:\Program Files (x86)\Microsoft Visual Studio\2017\Professional\VC\Auxiliary\Build
rem call "%VS140COMNTOOLS%\..\..\VC\vcvarsall.bat" x86_amd64
call "%VSPATH%\vcvars64.bat"



call "%VS140COMNTOOLS%\..\IDE\devenv" dp_opengl_tutorials.sln

