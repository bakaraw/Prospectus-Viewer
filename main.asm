.386
.MODEL flat, stdcall
OPTION casemap :none

INCLUDE \masm32\include\windows.inc
INCLUDE \masm32\include\kernel32.inc
INCLUDE \masm32\include\masm32.inc

INCLUDELIB \masm32\lib\kernel32.lib
INCLUDELIB \masm32\lib\masm32.lib

; imports the procedures from output_subjects.inc
INCLUDE includes\output_subjects.inc

.DATA
    prompt      db "Enter your year in college (1-4): ", 0
    prompt2     db "Enter semester: ", 10, 0
    sem1txt     db 9, 9, "[1] 1st Semester", 10, 0
    sem2txt     db 9, 9, "[2] 2nd Semester", 10, 0
    sem3txt     db 9, 9, "[3] Summer", 10, 10, 0
    yearMsg     db "You entered: ", 0


.DATA?
    inputBuffer db 4 dup(?)
    yearNum     dd ?
    semNum      dd ?
.CODE
main PROC
    year_input:
               invoke ClearScreen
               invoke StdOut, ADDR prompt
               invoke StdIn, ADDR inputBuffer, SIZEOF inputBuffer
    ; converts the input string to a double word
               invoke atodw, ADDR inputBuffer
               mov    yearNum, eax
               invoke dwtoa, yearNum, ADDR inputBuffer
    ; loops when the user enters a value that is not in the range of 1-4
.IF yearNum < 1 || yearNum > 4
               jmp    year_input
.ENDIF
              invoke StdOut, ADDR newline

    ; calls the ReadFileProc procedure from txt_reader.inc
    ; to read the contents of year.txt
    sem_input:
              invoke StdOut, ADDR prompt2
              invoke StdOut, ADDR sem1txt
              invoke StdOut, ADDR sem2txt
              invoke StdOut, ADDR sem3txt
              invoke StdIn, ADDR inputBuffer, SIZEOF inputBuffer
              invoke atodw, ADDR inputBuffer
              mov    semNum, eax
              invoke dwtoa, semNum, ADDR inputBuffer
    ; loops when the user enters a value that is not in the range of 1-3
.IF semNum < 1 || semNum > 3
              jmp    sem_input
.ENDIF
          invoke dwtoa, semNum, ADDR inputBuffer
          invoke ShowSubjects, yearNum, semNum
          invoke ExitProcess, 0
main ENDP
END main  ; Specify the entry point here