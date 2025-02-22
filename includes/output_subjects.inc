INCLUDE txt_reader.inc

.DATA
    first_year db 9, 9, 9, "First Year ", 0
    second_year db 9, 9, 9, "Second Year ", 0
    third_year db 9, 9, 9, "Third Year ", 0
    fourth_year db 9, 9, 9,"Fourth Year ", 0
    sem1 db "Semester 1 ", 0
    sem2 db "Semester 2 ", 0
    summer db "Summer ", 0
    subjects db "Subjects: ", 10, 0

    first_year_sem1  db "prospectus\\first_year_sem1.txt", 0
    first_year_sem2  db "prospectus\\first_year_sem2.txt", 0
    second_year_sem1 db "prospectus\\second_year_sem1.txt", 0
    second_year_sem2 db "prospectus\\second_year_sem2.txt", 0
    third_year_sem1  db "prospectus\\third_year_sem1.txt", 0
    third_year_sem2  db "prospectus\\third_year_sem2.txt", 0
    fourth_year_sem1 db "prospectus\\fourth_year_sem1.txt", 0
    fourth_year_sem2 db "prospectus\\fourth_year_sem2.txt", 0

.CODE
ShowSubjects PROC year:DWORD, semester:DWORD
    ; Prints the year
    .if year == 1
        invoke StdOut, ADDR first_year
    .elseif year == 2
        invoke StdOut, ADDR second_year
    .elseif year == 3
        invoke StdOut, ADDR third_year
    .elseif year == 4
        invoke StdOut, ADDR fourth_year
    .endif

    ; Prints the semester
    .if semester == 1
        invoke StdOut, ADDR sem1
    .elseif semester == 2
        invoke StdOut, ADDR sem2
    .elseif semester == 3
        invoke StdOut, ADDR summer
    .endif

    invoke StdOut, ADDR subjects
    ; Prints the subjects
    ; ReadFileProc "subjects.txt"
    .IF year == 1 && semester == 1
              invoke ReadFileProc, ADDR first_year_sem1
    .ELSEIF year == 1 && semester == 2
              invoke ReadFileProc, ADDR first_year_sem2
    .ELSEIF year == 2 && semester == 1
              invoke ReadFileProc, ADDR second_year_sem1
    .ELSEIF year == 2 && semester == 2
              invoke ReadFileProc, ADDR second_year_sem2
    .ELSEIF year == 3 && semester == 1
              invoke ReadFileProc, ADDR third_year_sem1
    .ELSEIF year == 3 && semester == 2
              invoke ReadFileProc, ADDR third_year_sem2
    .ELSEIF year == 4 && semester == 1
              invoke ReadFileProc, ADDR fourth_year_sem1
    .ELSEIF year == 4 && semester == 2
              invoke ReadFileProc, ADDR fourth_year_sem2
    .ENDIF
    ret
ShowSubjects ENDP