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
    dept        db 9, 9, 9, "  College of Teacher Education", 10, 0
    header      db 9, 9, 9, "Bachelor of Secondary Education", 10, 0
    header2     db 9, 9, 9, "       Major in Filipino", 10, 10, 0
    header3     db 9, 9, 9, "           PROSPECTUS", 10, 10, 0
    prompt      db "Enter your year in college (1-4): ", 0
    prompt2     db "Enter semester: ", 10, 0
    sem1txt     db 9, 9, "[1] 1st Semester", 10, 0
    sem2txt     db 9, 9, "[2] 2nd Semester", 10, 0
    sem3txt     db 9, 9, "[3] Summer", 10, 10, 0
    againMsg    db 10, "Prompt Again [Y/N]? ", 0


.DATA?
    inputBuffer db 4 dup(?)
    yearNum     dd ?
    semNum      dd ?
.CODE
main PROC
    year_input:
               invoke ClearScreen
               invoke StdOut, ADDR dept
               invoke StdOut, ADDR header
               invoke StdOut, ADDR header2
               invoke StdOut, ADDR header3
               invoke StdOut, ADDR prompt
               invoke StdIn, ADDR inputBuffer, SIZEOF inputBuffer
               ; converts the input string to a double word
               invoke atodw, ADDR inputBuffer
               mov    yearNum, eax
               invoke dwtoa, yearNum, ADDR inputBuffer
        ; loops when the user enters a value that is not in the range of 1-4
        .if yearNum < 1 || yearNum > 4
               jmp    year_input
        .endif
              invoke StdOut, ADDR newline
    sem_input:
              invoke StdOut, ADDR prompt2
              invoke StdOut, ADDR sem1txt
              invoke StdOut, ADDR sem2txt
              .if yearNum < 3
                  invoke StdOut, ADDR sem3txt
              .endif
              invoke StdIn, ADDR inputBuffer, SIZEOF inputBuffer
              invoke atodw, ADDR inputBuffer
              mov    semNum, eax
              invoke dwtoa, semNum, ADDR inputBuffer
        ; loops when the user enters a value that is not in the range of 1-3
        .if semNum < 1 || semNum > 3
              jmp    sem_input
        .endif

          invoke dwtoa, semNum, ADDR inputBuffer
          invoke ShowSubjects, yearNum, semNum

          invoke StdOut, ADDR againMsg
          invoke StdIn, ADDR inputBuffer, SIZEOF inputBuffer

    .if inputBuffer[0] == 'Y' || inputBuffer[0] == 'y'
          jmp    year_input
    .endif

          invoke ExitProcess, 0
main ENDP
END main 
