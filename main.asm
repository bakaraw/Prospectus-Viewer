.386
.model flat, stdcall
option casemap :none

include \masm32\include\windows.inc
include \masm32\include\kernel32.inc
include \masm32\include\masm32.inc

includelib \masm32\lib\kernel32.lib
includelib \masm32\lib\masm32.lib

.data
    prompt db "Enter your year of college (1-4): ", 0
    inputBuffer db 4 dup(0)
    yearMsg db "You entered: ", 0
    newline db 13, 10, 0

.code 
start:
    invoke StdOut, addr newline
    invoke StdOut, addr prompt
    invoke StdIn, addr inputBuffer, 4
    invoke StdOut, addr yearMsg
    invoke StdOut, addr inputBuffer
    invoke StdOut, addr newline
    invoke ExitProcess, 0
end start