; Enrollment course functionality

.DATA
    enroll_header  db 9, 9, 9, "Enrollment for ", 0
    sem_options    db "Select a semester to enroll in:", 10, 0
    
    option_sem1    db 9, 9, "[1] 1st Semester", 10, 0
    option_sem2    db 9, 9, "[2] 2nd Semester", 10, 0
    option_summer  db 9, 9, "[3] Summer", 10, 0
    
    back_option    db 9, 9, "[0] Back to Main Menu", 10, 10, 0
    sem_prompt     db "Enter your choice: ", 0
    
    enroll_sem1    db "1st Semester", 0
    enroll_sem2    db "2nd Semester", 0
    enroll_summer  db "Summer", 0
    
    year_str1      db "1st Year", 0
    year_str2      db "2nd Year", 0
    year_str3      db "3rd Year", 0
    year_str4      db "4th Year", 0
    space       db " - ", 0
press_key   db "Press any key to continue...", 0
    
    invalid_input  db "Invalid input. Please try again.", 10, 0

.DATA?
    enroll_buffer db 4 dup(?)
    subject_buffer db 1024 dup(?)  ; Buffer to store subjects temporarily
subject_count dd 0             ; Counter for the number of subjects read

.CODE
EnrollCourse PROC yearNum:DWORD
LOCAL semChoice:DWORD ;
    
    invoke ClearScreen
    invoke StdOut, ADDR enroll_header
    
    ; Display the year string
    .if yearNum == 1
        invoke StdOut, ADDR year_str1
    .elseif yearNum == 2
        invoke StdOut, ADDR year_str2
    .elseif yearNum == 3
        invoke StdOut, ADDR year_str3
    .elseif yearNum == 4
        invoke StdOut, ADDR year_str4
    .endif
    
    invoke StdOut, ADDR newline
    invoke StdOut, ADDR newline
    
sem_selection:
    invoke StdOut, ADDR sem_options
    invoke StdOut, ADDR option_sem1
    invoke StdOut, ADDR option_sem2
    
    .if yearNum < 3  ; 1st & 2nd year students have summer
        invoke StdOut, ADDR option_summer
    .endif
    
    invoke StdOut, ADDR back_option
    invoke StdOut, ADDR sem_prompt
    invoke StdIn, ADDR enroll_buffer, SIZEOF enroll_buffer
    invoke atodw, ADDR enroll_buffer
    mov semChoice, eax
    
    ; Check if user wants to go back
    .if semChoice == 0
        ret
    .endif
    
    ; Validate input
    .if yearNum < 3
        .if semChoice < 1 || semChoice > 3
            invoke StdOut, ADDR invalid_input
            jmp sem_selection
        .endif
    .else
        .if semChoice < 1 || semChoice > 2
            invoke StdOut, ADDR invalid_input
            jmp sem_selection
        .endif
    .endif
    
    invoke ClearScreen
    invoke StdOut, ADDR enroll_header
    
    ; Display the year string
    .if yearNum == 1
        invoke StdOut, ADDR year_str1
        invoke StdOut, ADDR space
    .elseif yearNum == 2
        invoke StdOut, ADDR year_str2
        invoke StdOut, ADDR space
    .elseif yearNum == 3
        invoke StdOut, ADDR year_str3
        invoke StdOut, ADDR space
    .elseif yearNum == 4
        invoke StdOut, ADDR year_str4
        invoke StdOut, ADDR space
    .endif
    
    ; Display semester
    .if semChoice == 1
        invoke StdOut, ADDR enroll_sem1
    .elseif semChoice == 2
        invoke StdOut, ADDR enroll_sem2
    .elseif semChoice == 3
        invoke StdOut, ADDR enroll_summer
    .endif
    
    invoke StdOut, ADDR newline
    invoke StdOut, ADDR newline
    
    ; Display the subjects for the selected year and semester
    .IF yearNum == 1 && semChoice == 1
        invoke ReadFileProc, ADDR first_year_sem1
    .ELSEIF yearNum == 1 && semChoice == 2
        invoke ReadFileProc, ADDR first_year_sem2
    .ELSEIF yearNum == 1 && semChoice == 3
        invoke ReadFileProc, ADDR first_year_summer
    .ELSEIF yearNum == 2 && semChoice == 1
        invoke ReadFileProc, ADDR second_year_sem1
    .ELSEIF yearNum == 2 && semChoice == 2
        invoke ReadFileProc, ADDR second_year_sem2
    .ELSEIF yearNum == 2 && semChoice == 3
        invoke ReadFileProc, ADDR second_year_summer
    .ELSEIF yearNum == 3 && semChoice == 1
        invoke ReadFileProc, ADDR third_year_sem1
    .ELSEIF yearNum == 3 && semChoice == 2
        invoke ReadFileProc, ADDR third_year_sem2
    .ELSEIF yearNum == 4 && semChoice == 1
        invoke ReadFileProc, ADDR fourth_year_sem1
    .ELSEIF yearNum == 4 && semChoice == 2
        invoke ReadFileProc, ADDR fourth_year_sem2
    .ENDIF
    
    ; Wait for user to press any key
    invoke StdOut, ADDR newline
    invoke StdOut, ADDR press_key

    
    invoke StdIn, ADDR enroll_buffer, SIZEOF enroll_buffer
    
    ret
EnrollCourse ENDP