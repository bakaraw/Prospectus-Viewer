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
    ;newline        db 13, 10, 0

    ; Buffer for storing selected subject codes
    MAX_SELECTIONS  EQU 9
    MAX_CODE_LEN    EQU 10
    selected_codes  db MAX_SELECTIONS * MAX_CODE_LEN dup(0)  ; Initialize with zeros
    code_count      dd 0

    done_msg       db "Done selecting subjects? (y/n): ", 0
    final_list     db "Your final subject list:", 0ah, 0
    
    subject_header  db "SUBJECT CODE    SUBJECT TITLE                                               UNITS   PREREQUISITES", 0ah, 0
    line_separator  db "================================================================================", 0ah, 0
    subject_error   db "Subject code not found!", 0ah, 0

    ; File reading variables
    bytes_read     dd 0                ; Renamed to avoid conflict
.DATA?
    enroll_buffer  db 4 dup(?)
    subject_buffer db 4096 dup(?)      ; Increased buffer size for subjects
    input_buffer   db 20 dup(?)        ; Buffer for subject code input
    temp_buffer    db 100 dup(?)       ; Temporary buffer for output formatting

.CODE

; Completely revised FindSubjectDetails procedure
FindSubjectDetails PROC uses esi edi ebx subject_code:PTR BYTE
    LOCAL found:DWORD
    
    mov found, 0
    mov esi, OFFSET subject_buffer
    
    ; Search line by line
@search_line:
    mov edi, esi            ; Save line start
    
    ; Check if end of buffer
    cmp byte ptr [esi], 0
    je @not_found
    
    ; Skip leading spaces/numbers
@skip_prefix:
    mov al, [esi]
    cmp al, 0ah            ; New line?
    je @next_line
    cmp al, 0              ; End of buffer?
    je @not_found
    
    ; Check if this might be a subject code
    cmp al, 'A'
    jl @not_code_start
    cmp al, 'Z'
    jle @compare_code      ; Uppercase letter - possible code
    cmp al, 'a'
    jl @not_code_start
    cmp al, 'z'
    jle @compare_code      ; Lowercase letter - possible code
    
@not_code_start:
    inc esi
    jmp @skip_prefix
    
@compare_code:
    ; Compare with subject code
    mov edi, subject_code
    push esi               ; Save start position
    
@compare_loop:
    mov al, [edi]
    test al, al            ; End of input?
    jz @possible_match
    
    mov bl, [esi]
    cmp al, bl
    jne @no_match
    
    inc esi
    inc edi
    jmp @compare_loop
    
@no_match:
    pop esi                ; Restore position
    
@next_line:
    ; Move to next line
    @find_eol:
    mov al, [esi]
    inc esi
    test al, al            ; End of buffer?
    jz @not_found
    cmp al, 0ah            ; New line?
    jne @find_eol
    jmp @search_line
    
@possible_match:
    ; Check if followed by space
    mov al, [esi]
    cmp al, ' '
    je @match_found
    
    pop esi                ; Not a match, restore position
    jmp @next_line
    
@match_found:
    mov found, 1
    pop esi                ; Get start of code
    mov edi, esi            ; Save for printing
    
    ; Go to start of line
    @find_line_start:
    dec edi
    cmp edi, OFFSET subject_buffer
    jb @print_line         ; If at start of buffer, print line
    mov al, [edi]
    cmp al, 0ah            ; Previous line?
    jne @find_line_start
    inc edi                ; Move past newline
    
@print_line:
    ; Print the entire line
    push edi
    invoke StdOut, edi
    invoke StdOut, ADDR newline
    pop edi
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

    ; Reset selection counter
    mov code_count, 0

@input_loop:
    ; Check if maximum subjects reached
    mov eax, code_count
    cmp eax, MAX_SELECTIONS
    jge @show_final
    
    ; Get subject code
    invoke StdOut, ADDR select_prompt
    invoke StdIn, ADDR input_buffer, SIZEOF input_buffer
    
    ; Check for exit (if user entered "0")
    cmp BYTE PTR [input_buffer], '0'
    je @show_final
    
    ; Try to find subject (just check it exists, don't display it)
    invoke FindSubjectDetails, ADDR input_buffer
    cmp eax, 1                  ; Was subject found?
    jne @input_loop            ; If not, try again
    
    ; Store the subject code
    mov esi, OFFSET input_buffer
    mov edi, OFFSET selected_codes
    mov eax, code_count
    imul eax, MAX_CODE_LEN
    add edi, eax
    
    ; Copy code (up to MAX_CODE_LEN-1 chars)
    mov ecx, 0
@copy_loop:
    cmp ecx, MAX_CODE_LEN-1
    jge @copy_done
    
    mov al, [esi]
    test al, al
    jz @copy_done
    
    mov [edi], al
    inc esi
    inc edi
    inc ecx
    jmp @copy_loop
    
@copy_done:
    mov BYTE PTR [edi], 0      ; Null terminate
    inc code_count
    
    ; Confirm subject added
    invoke StdOut, ADDR newline
    invoke StdOut, ADDR selected_header
    
    ; Show current selections count
    push eax
    mov eax, code_count
    invoke dwtoa, eax, ADDR temp_buffer
    invoke StdOut, ADDR temp_buffer
    invoke StdOut, ADDR code_separator
    pop eax
    
    jmp @input_loop
    
@show_final:
    ; Display final list header
    invoke ClearScreen
    invoke StdOut, ADDR final_list
    invoke StdOut, ADDR subject_header
    invoke StdOut, ADDR line_separator
    
    ; Show all selected subjects
    xor ecx, ecx
    
@display_loop:
    cmp ecx, code_count
    jge @done
    
    ; Get pointer to current code
    mov eax, ecx
    imul eax, MAX_CODE_LEN
    add eax, OFFSET selected_codes
    
    ; Display subject details
    push ecx
    invoke FindSubjectDetails, eax
    pop ecx
    
    inc ecx
    jmp @display_loop
    
@done:
    invoke StdOut, ADDR newline
    pop edi
    pop esi
    pop ebx
    ret
SelectSubjects ENDP

; Modified LoadSubjectFile to append to the buffer
LoadSubjectFile PROC filePathAddr:DWORD
    LOCAL fileHandle:DWORD
    LOCAL currentPosition:DWORD
    
    ; Find the end of the current buffer content
    mov edi, OFFSET subject_buffer
    mov currentPosition, 0
    
    ; Find null terminator
    .WHILE BYTE PTR [edi] != 0
        inc edi
        inc currentPosition
    .ENDW
    
    ; Open the file
    invoke CreateFile, filePathAddr, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL
    cmp eax, INVALID_HANDLE_VALUE
    je @file_error
    
    mov fileHandle, eax
    
    ; Calculate remaining space in buffer
    mov ecx, SIZEOF subject_buffer
    sub ecx, currentPosition
    dec ecx  ; Leave space for null terminator
    
    ; Read file content into buffer at the current position
    invoke ReadFile, fileHandle, edi, ecx, ADDR bytes_read, NULL
    
    ; Null-terminate the buffer
    mov eax, bytes_read
    mov BYTE PTR [edi + eax], 0
    
    ; Close file
    invoke CloseHandle, fileHandle
    mov eax, 1  ; Success
    ret
    
@file_error:
    xor eax, eax  ; Failure
    ret
LoadSubjectFile ENDP

; Main enrollment procedure (fixed parameter list)
EnrollCourse PROC yearNum:DWORD
    ; Initialize code count
    mov code_count, 0
    
    ; Clear previous selections
    push edi
    mov edi, OFFSET selected_codes
    mov ecx, MAX_SELECTIONS * MAX_CODE_LEN
    mov al, 0
    rep stosb
    pop edi
    
    invoke ClearScreen
    invoke StdOut, ADDR enroll_header
    
    ; Display year
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
    
    ; Load the subject file for the selected year
    .IF yearNum == 1
        invoke LoadSubjectFile, ADDR first_year_sem1
        invoke LoadSubjectFile, ADDR first_year_sem2
        invoke LoadSubjectFile, ADDR first_year_summer
        invoke ClearScreen

    .ELSEIF yearNum == 2
        invoke LoadSubjectFile, ADDR second_year_sem1
        invoke LoadSubjectFile, ADDR second_year_sem2
        invoke LoadSubjectFile, ADDR second_year_summer
                invoke ClearScreen

    .ELSEIF yearNum == 3
        invoke LoadSubjectFile, ADDR third_year_sem1
        invoke LoadSubjectFile, ADDR third_year_sem2
                invoke ClearScreen

    .ELSEIF yearNum == 4
        invoke LoadSubjectFile, ADDR fourth_year_sem1
        invoke LoadSubjectFile, ADDR fourth_year_sem2
                invoke ClearScreen

    .ENDIF
    
    ; Display available subjects
    invoke StdOut, ADDR newline
    invoke StdOut, ADDR subject_header
    invoke StdOut, ADDR line_separator
    invoke StdOut, ADDR subject_buffer
    invoke StdOut, ADDR newline
    
    ; Handle subject selection
    invoke SelectSubjects
    
    ; Wait for user input
    invoke StdOut, ADDR press_key
    invoke StdIn, ADDR enroll_buffer, SIZEOF enroll_buffer
    
    ret
EnrollCourse ENDP