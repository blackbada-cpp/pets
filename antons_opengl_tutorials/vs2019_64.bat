@echo off

set VS160COMNTOOLS=C:\Program Files (x86)\Microsoft Visual Studio\2019\Professional\Common7\Tools\
set VSPATH=C:\Program Files (x86)\Microsoft Visual Studio\2019\Professional\VC\Auxiliary\Build
call "%VSPATH%\vcvars64.bat"

call "%VS160COMNTOOLS%\..\IDE\devenv" dp_opengl_vs2019.sln

