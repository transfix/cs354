@echo off
REM Use this batch file to run project2

set GLUT_BIN_DIR=@GLUT_ROOT_PATH@\bin\x64
set GLEW_BIN_DIR=@GLEW_ROOT_PATH@\bin
set BOOST_BIN_DIR=@BOOST_ROOT@\bin

set PATH=%GLUT_BIN_DIR%;%GLEW_BIN_DIR%;%BOOST_BIN_DIR%;%PATH%

@PROJECT_BINARY_DIR@\bin\Debug\project2.exe
