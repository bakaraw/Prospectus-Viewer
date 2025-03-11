; Enrollment course functionality
.DATA
    enroll_header  db 9, 9, 9, "Enrollment for ", 0
    
    enroll_sem1    db "1st Semester", 0
    enroll_sem2    db "2nd Semester", 0
    enroll_summer  db "Summer", 0
    
    year_str1      db "1st Year", 0
    year_str2      db "2nd Year", 0
    year_str3      db "3rd Year", 0
    year_str4      db "4th Year", 0
    space          db " - ", 0

    select_prompt   db "Enter subject code to enroll (0 to finish): ", 0
    selected_header db "Selected Subjects:", 10, 0
    code_separator  db ", ", 0
    press_key      db "Press any key to continue...", 0
    invalid_input  db "Invalid input. Please try again.", 10, 0

    ; Buffer for storing selected subject codes
    MAX_SELECTIONS  EQU 9
    MAX_CODE_LEN    EQU 10
    selected_codes  db MAX_SELECTIONS * MAX_CODE_LEN dup(?)
    code_count      dd 0

      done_msg       db "Done selecting subjects? (y/n): ", 0
    final_list     db "Your final subject list:", 0ah, 0
    ;subject_count  db "Total subjects selected: ", 0

     subject_header  db "SUBJECT CODE    SUBJECT TITLE                                               UNITS   PREREQUISITES", 0ah, 0
    line_separator  db "================================================================================", 0ah, 0
    subject_error   db "Subject code not found!", 0ah, 0


.DATA?
    enroll_buffer  db 4 dup(?)
    subject_buffer db 1024 dup(?)  ; Buffer to store subjects temporarily
    ;subject_count  dd 0            ; Counter for the number of subjects read
    input_buffer   db 10 dup(?)    ; Buffer for subject code input

.CODE
FindSubjectDetails PROC uses esi edi ebx subject_code:PTR BYTE
    LOCAL found:DWORD
    LOCAL lineStart:DWORD
    
    mov found, 0
    mov esi, OFFSET subject_buffer     ; Start of text file content
    
    ; Skip header line (SUBJECT CODE...)
    mov ecx, 1                         ; Changed to skip just 1 line
@skip_header:
    mov al, [esi]
    inc esi
    cmp al, 0ah                        ; Look for newline
    jne @skip_header
    
@find_loop:
    ; Store start of current line
    mov lineStart, esi
    
    ; Skip leading spaces
    @skip_spaces:
    mov al, [esi]
    cmp al, ' '
    jne @start_compare
    inc esi
    jmp @skip_spaces
    
@start_compare:
    ; Compare subject code
    mov edi, subject_code
@compare_code:
    mov al, [esi]
    mov bl, [edi]
    
    ; Check end of input code
    cmp bl, 0dh                        ; Carriage return
    je @check_space
    cmp bl, 0                          ; Null terminator
    je @check_space
    
    ; Compare characters
    cmp al, bl
    jne @next_line
    
    inc esi
    inc edi
    jmp @compare_code
    
@check_space:
    ; Verify we found complete code (followed by spaces)
    cmp al, ' '
    je @found_match
    
@next_line:
    mov esi, lineStart                 ; Return to line start
@find_next_line:
    mov al, [esi]
    inc esi
    test al, al                        ; Check end of buffer
    jz @not_found
    cmp al, 0ah                        ; Look for newline
    jne @find_next_line
    jmp @find_loop
    
@found_match:
    mov found, 1
    mov esi, lineStart                 ; Return to line start
    
@print_line:
    mov al, [esi]
    cmp al, 0ah                        ; Stop at newline
    je @print_done
    invoke StdOut, esi
    inc esi
    jmp @print_line
    
@print_done:
    invoke StdOut, ADDR newline
    jmp @exit
    
@not_found:
    invoke StdOut, ADDR subject_error
    
@exit:
    mov eax, found
    ret
FindSubjectDetails ENDP



SelectSubjects PROC
    push ebx
    push esi
    push edi

    ; Display header for selections
    invoke StdOut, ADDR subject_header
    invoke StdOut, ADDR line_separator

@input_loop:
    ; Check max subjects
    mov eax, code_count
    cmp eax, MAX_SELECTIONS
    jge @display_list
    
    ; Get subject code
    invoke StdOut, ADDR select_prompt
    invoke StdIn, ADDR input_buffer, SIZEOF input_buffer
    
    ; Check for exit
    cmp BYTE PTR [input_buffer], '0'
    je @display_list
    
    ; Find and display subject details
    invoke FindSubjectDetails, ADDR input_buffer
    cmp eax, 1                         ; Was subject found?
    jne @input_loop                    ; If not, ask again
    
    ; Store valid subject code
    mov esi, OFFSET input_buffer
    mov edi, OFFSET selected_codes
    mov eax, code_count
    imul eax, MAX_CODE_LEN
    add edi, eax
    
    mov ecx, MAX_CODE_LEN
    rep movsb
    
    inc code_count
    jmp @input_loop

@display_list:
    ; Show final list
    invoke ClearScreen
    invoke StdOut, ADDR final_list
    invoke StdOut, ADDR subject_header
    invoke StdOut, ADDR line_separator
    
    ; Display all selected subjects
    xor ecx, ecx
    mov edi, OFFSET selected_codes
    
@display_loop:
    cmp ecx, code_count
    jge @done
    
    push ecx
    push edi
    invoke FindSubjectDetails, edi
    pop edi
    pop ecx
    
    add edi, MAX_CODE_LEN
    inc ecx
    jmp @display_loop

@done:
    invoke StdOut, ADDR newline
    pop edi
    pop esi
    pop ebx
    ret
SelectSubjects ENDP


EnrollCourse PROC yearNum:DWORD
    LOCAL tempBuffer[10]:BYTE
    
    ; Initialize code count
    mov code_count, 0
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
    
    ; Display all subjects for the selected year
    .IF yearNum == 1
        invoke StdOut, ADDR enroll_sem1
        invoke StdOut, ADDR newline
        invoke ReadFileProc, ADDR first_year_sem1
        invoke StdOut, ADDR enroll_sem2
        invoke StdOut, ADDR newline
        invoke ReadFileProc, ADDR first_year_sem2
        invoke StdOut, ADDR enroll_summer
        invoke StdOut, ADDR newline
        invoke ReadFileProc, ADDR first_year_summer
    .ELSEIF yearNum == 2
        invoke StdOut, ADDR enroll_sem1
        invoke StdOut, ADDR newline
        invoke ReadFileProc, ADDR second_year_sem1
        invoke StdOut, ADDR enroll_sem2
        invoke StdOut, ADDR newline
        invoke ReadFileProc, ADDR second_year_sem2
        invoke StdOut, ADDR enroll_summer
        invoke StdOut, ADDR newline
        invoke ReadFileProc, ADDR second_year_summer
    .ELSEIF yearNum == 3
        invoke StdOut, ADDR enroll_sem1
        invoke StdOut, ADDR newline
        invoke ReadFileProc, ADDR third_year_sem1
        invoke StdOut, ADDR enroll_sem2
        invoke StdOut, ADDR newline
        invoke ReadFileProc, ADDR third_year_sem2
    .ELSEIF yearNum == 4
        invoke StdOut, ADDR enroll_sem1
        invoke StdOut, ADDR newline
        invoke ReadFileProc, ADDR fourth_year_sem1
        invoke StdOut, ADDR enroll_sem2
        invoke StdOut, ADDR newline
        invoke ReadFileProc, ADDR fourth_year_sem2
    .ENDIF

    ; Call SelectSubjects to handle subject selection
    invoke SelectSubjects
    
    ; Wait for user to press any key
    invoke StdOut, ADDR newline
    invoke StdOut, ADDR press_key
    invoke StdIn, ADDR enroll_buffer, SIZEOF enroll_buffer
    
    ret
EnrollCourse ENDP