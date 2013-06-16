@echo off
REM Use this batch file to start up VS2012

set GLUT_BIN_DIR=@GLUT_ROOT_PATH@\bin\x64
set GLEW_BIN_DIR=@GLEW_ROOT_PATH@\bin
set BOOST_BIN_DIR=@BOOST_ROOT@\bin

set PATH=%GLUT_BIN_DIR%;%GLEW_BIN_DIR%;%BOOST_BIN_DIR%;%PATH%

if "@MSVC11@" == "1" (
  call "C:\Program Files (x86)\Microsoft Visual Studio 11.0\VC\vcvarsall.bat" amd64
)

REM devenv is for VSPro
REM devenv cs354.sln

"C:\Program Files (x86)\Microsoft Visual Studio 11.0\Common7\IDE\WDExpress.exe" cs354.sln
