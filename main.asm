.386
.MODEL flat, stdcall
OPTION casemap :none

INCLUDE \masm32\include\windows.inc
INCLUDE \masm32\include\kernel32.inc
INCLUDE \masm32\include\masm32.inc

INCLUDELIB \masm32\lib\kernel32.lib
INCLUDELIB \masm32\lib\masm32.lib

; Imports the procedures from output_subjects.inc
INCLUDE includes\output_subjects.inc
INCLUDE includes\enroll_course.inc   ; Include enroll functionality

.DATA

    dept        db 9, 9, 9, "  College of Teacher Education", 10, 0
    header      db 9, 9, 9, "Bachelor of Secondary Education", 10, 0
    header2     db 9, 9, 9, "       Major in Filipino", 10, 10, 0
    header3     db 9, 9, 9, "           PROSPECTUS", 10, 10, 0
    prompt      db "Select your year level:", 10, 0
    year1txt    db 9, 9, "[1] 1st Year", 10, 0
    year2txt    db 9, 9, "[2] 2nd Year", 10, 0
    year3txt    db 9, 9, "[3] 3rd Year", 10, 0
    year4txt    db 9, 9, "[4] 4th Year", 10, 10, 0

    prompt2     db "Select semester:", 10, 0
    sem1txt     db 9, 9, "[1] 1st Semester", 10, 0
    sem2txt     db 9, 9, "[2] 2nd Semester", 10, 0
    sem3txt     db 9, 9, "[3] Summer", 10, 10, 0
    enroll_opt  db 9, 9, "[4] Enroll", 10, 10, 0
    againMsg    db 10, "Would you like to select again? [Y/N]: ", 0

.DATA?
    inputBuffer db 4 dup(?)
    yearNum     dd ?
    semNum      dd ?

.CODE

main PROC
year_input PROC ;
               invoke ClearScreen
               invoke StdOut, ADDR dept
               invoke StdOut, ADDR header
               invoke StdOut, ADDR header2
               invoke StdOut, ADDR header3
               invoke StdOut, ADDR prompt
               invoke StdOut, ADDR year1txt
               invoke StdOut, ADDR year2txt
               invoke StdOut, ADDR year3txt
               invoke StdOut, ADDR year4txt
               invoke StdIn, ADDR inputBuffer, SIZEOF inputBuffer
               invoke atodw, ADDR inputBuffer
               mov    yearNum, eax
               invoke dwtoa, yearNum, ADDR inputBuffer

        ; If the user enters an invalid year
        .if yearNum < 1 || yearNum > 4
               jmp    year_input
        .endif

        invoke StdOut, ADDR newline

    sem_input:
              invoke StdOut, ADDR prompt2
              invoke StdOut, ADDR sem1txt
              invoke StdOut, ADDR sem2txt
              .if yearNum < 3  ; 1st & 2nd year students have summer
                  invoke StdOut, ADDR sem3txt
              .endif
               invoke StdOut, ADDR enroll_opt 
              invoke StdIn, ADDR inputBuffer, SIZEOF inputBuffer
              invoke atodw, ADDR inputBuffer
              mov    semNum, eax
              invoke dwtoa, semNum, ADDR inputBuffer
              
         .if semNum == 4   ; Handle the enroll option
              invoke EnrollCourse, yearNum     ; Call the enrollment procedure
        .endif


        .if yearNum < 3
            .if semNum < 1 || semNum > 3
                  jmp    sem_input
            .endif
        .else
            .if semNum < 1 || semNum > 2
                  jmp    sem_input
            .endif
        .endif

          ; Display subjects based on yearNum & semNum
          invoke ShowSubjects, yearNum, semNum

          invoke StdOut, ADDR againMsg
          invoke StdIn, ADDR inputBuffer, SIZEOF inputBuffer

    .if inputBuffer[0] == 'Y' || inputBuffer[0] == 'y'
          jmp    year_input
    .endif

          invoke ExitProcess, 0
year_input ENDP
main ENDP

END main
