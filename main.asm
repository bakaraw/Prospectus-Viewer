.386
.MODEL flat, stdcall
OPTION casemap :none

INCLUDE \masm32\include\windows.inc
INCLUDE \masm32\include\kernel32.inc
INCLUDE \masm32\include\masm32.inc

INCLUDELIB \masm32\lib\kernel32.lib
INCLUDELIB \masm32\lib\masm32.lib

; imports the procedures from txt_reader.inc
INCLUDE includes\txt_reader.inc

.DATA
    prompt           db "Enter your year in college (1-4): ", 0
    prompt2          db "Enter semester: ", 10, 0
    sem1txt          db 9, 9, "[1] 1st Semester", 10, 0
    sem2txt          db 9, 9, "[2] 2nd Semester", 10, 0
    sem3txt          db 9, 9, "[3] Summer", 10, 10, 0
    yearMsg          db "You entered: ", 0

    first_year_sem1  db "prospectus\\first_year_sem1.txt", 0
    first_year_sem2  db "prospectus\\first_year_sem2.txt", 0
    second_year_sem1 db "prospectus\\second_year_sem1.txt", 0
    second_year_sem2 db "prospectus\\second_year_sem2.txt", 0
    third_year_sem1  db "prospectus\\third_year_sem1.txt", 0
    third_year_sem2  db "prospectus\\third_year_sem2.txt", 0
    fourth_year_sem1 db "prospectus\\fourth_year_sem1.txt", 0
    fourth_year_sem2 db "prospectus\\fourth_year_sem2.txt", 0

.DATA?
    yearInput        db 4 dup(?)
    semInput         db 4 dup(?)
    yearNum          dd ?
    semNum           dd ?
.CODE
main PROC
    year_input:
               invoke ClearScreen
               invoke StdOut, ADDR newline
               invoke StdOut, ADDR prompt
               invoke StdIn, ADDR yearInput, SIZEOF yearInput
               invoke atodw, ADDR yearInput
               mov    yearNum, eax
              invoke dwtoa, yearNum, ADDR yearInput
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
              invoke StdIn, ADDR semInput, SIZEOF semInput
              invoke atodw, ADDR semInput
              mov    semNum, eax
              invoke dwtoa, semNum, ADDR semInput
.IF semNum < 1 || semNum > 3
              jmp    sem_input
.ENDIF
              invoke dwtoa, semNum, ADDR semInput

    
.IF yearNum == 1 && semNum == 1
          invoke ReadFileProc, ADDR first_year_sem1
.ELSEIF yearNum == 1 && semNum == 2
          invoke ReadFileProc, ADDR first_year_sem2
.ELSEIF yearNum == 2 && semNum == 1
          invoke ReadFileProc, ADDR second_year_sem1
.ELSEIF yearNum == 2 && semNum == 2
          invoke ReadFileProc, ADDR second_year_sem2
.ELSEIF yearNum == 3 && semNum == 1
          invoke ReadFileProc, ADDR third_year_sem1
.ELSEIF yearNum == 3 && semNum == 2
          invoke ReadFileProc, ADDR third_year_sem2
.ELSEIF yearNum == 4 && semNum == 1
          invoke ReadFileProc, ADDR fourth_year_sem1
.ELSEIF yearNum == 4 && semNum == 2
          invoke ReadFileProc, ADDR fourth_year_sem2
.ENDIF

          invoke ExitProcess, 0
main ENDP
END main