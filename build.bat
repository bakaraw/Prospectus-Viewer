@echo off
set ASM_FILE=main.asm
set OBJ_FILE=main.obj
set EXE_FILE=main.exe

echo Assembling %ASM_FILE%...
ml /c /coff "%ASM_FILE%"
if %ERRORLEVEL% neq 0 (
    echo Assembly failed!
    exit /b 1
)

echo Linking %OBJ_FILE%...
link /SUBSYSTEM:CONSOLE "%OBJ_FILE%"
if %ERRORLEVEL% neq 0 (
    echo Linking failed!
    exit /b 1
)

echo Running "%EXE_FILE%"...
%EXE_FILE%
