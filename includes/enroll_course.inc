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
    ; Remove this commented line entirely, as it's causing confusion
    ; newline        db 13, 10, 0
    return_or_exit db "Press 1 to return to main menu, 2 to exit: ", 0
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
    
    ; Keep these definitions
    total_units_line db "                                                            TOTAL UNITS:    ", 0
    ;newline          db 13, 10, 0  ; Define newline
    ; File reading variables
    bytes_read     dd 0                ; Renamed to avoid conflict

enhanced_options db "Choose an option:", 13, 10
                 db "1. Return to Semester List", 13, 10
                 db "2. Enroll more subjects (same year)", 13, 10
                 db "3. Back to Year Selection", 13, 10
                 db "4. Exit program", 13, 10
                 db "Enter choice: ", 0

.DATA?
    enroll_buffer  db 4 dup(?)
    subject_buffer db 4096 dup(?)      ; Increased buffer size for subjects
    input_buffer   db 20 dup(?)        ; Buffer for subject code input
    temp_buffer    db 100 dup(?)       ; Temporary buffer for output formatting
     ;   newline          db 13, 10, 0  ; Define newline


.CODE

EXTERNDEF year_input:PROC  ; 

; Completely revised FindSubjectDetails procedure
FindSubjectDetails PROC uses esi edi ebx subject_code:PTR BYTE
    LOCAL found:DWORD
    LOCAL line_start:DWORD
    LOCAL units_value:DWORD
    
    mov found, 0
    mov esi, OFFSET subject_buffer
    mov units_value, 0  ; Initialize units to 0
    
@search_line:
    ; Save current position as potential line start
    mov line_start, esi
    
    ; Check if end of buffer
    cmp byte ptr [esi], 0
    je @not_found
    
    ; Skip line numbers and spaces
@skip_spaces:
    mov al, [esi]
    cmp al, 0Ah            ; New line?
    je @next_line
    cmp al, 0              ; End of buffer?
    je @not_found
    
    ; Look for subject code start
    cmp al, 'A'
    jb @not_code_char
    cmp al, 'Z'
    jbe @potential_code    ; Found uppercase letter
    cmp al, 'a'
    jb @not_code_char
    cmp al, 'z'
    jbe @potential_code    ; Found lowercase letter
    
@not_code_char:
    inc esi
    jmp @skip_spaces
    
@potential_code:
    ; Compare with input subject code
    mov edi, subject_code
    push esi               ; Save start of code
    
@code_compare_loop:
    mov al, [edi]
    mov bl, [esi]
    
    test al, al            ; End of input code?
    jz @end_of_input_code
    
    cmp al, bl
    jne @code_mismatch
    
    inc esi
    inc edi
    jmp @code_compare_loop
    
@end_of_input_code:
    ; Check if end of code in subject list (space should follow)
    cmp bl, ' '
    je @found_match
    
@code_mismatch:
    pop esi                ; Restore position
    
@next_line:
    ; Find end of current line
@find_eol:
    mov al, [esi]
    inc esi
    test al, al            ; End of buffer?
    jz @not_found
    cmp al, 0Ah            ; New line?
    jne @find_eol
    jmp @search_line
    
@found_match:
    mov found, 1
    pop esi                ; Get start of subject code
    
    ; Extract units from the line (around column 70-74)
    mov edi, line_start
    add edi, 70            ; Approximate position of units column
    
    ; Find the units value
@find_units:
    mov al, [edi]
    cmp al, 0              ; End of buffer?
    je @copy_line
    cmp al, 0Ah            ; End of line?
    je @copy_line
    
    ; Check if this is a digit
    cmp al, '0'
    jb @next_units_char
    cmp al, '9'
    ja @next_units_char
    
    ; Found a digit - extract the units value
    sub al, '0'            ; Convert ASCII to number
    movzx eax, al
    mov units_value, eax
    jmp @copy_line
    
@next_units_char:
    inc edi
    jmp @find_units
    
@copy_line:
    ; Better approach: copy the entire line to temp_buffer and print at once
    lea edi, [temp_buffer]  ; Use temp buffer for storing the line
    mov esi, line_start    ; Start from beginning of line
    
@copy_line_loop:
    mov al, [esi]
    test al, al            ; End of buffer?
    jz @copy_done
    cmp al, 0Ah            ; End of line?
    je @copy_done
    
    ; Copy character to temp_buffer
    mov [edi], al
    inc esi
    inc edi
    jmp @copy_line_loop
    
@copy_done:
    mov byte ptr [edi], 0  ; Null-terminate
    
    ; Print the entire line
    invoke StdOut, ADDR temp_buffer
    invoke StdOut, ADDR newline
    
    ; Return the units value in eax
    mov eax, found
    jmp @exit
    
@not_found:
    ; Don't show error when printing final list
    mov eax, 0
    
@exit:
    ; Return units value in edx
    mov edx, units_value
    ret
FindSubjectDetails ENDP

; New procedure that finds a subject but doesn't display it
FindSubjectDetailsQuiet PROC uses esi edi ebx subject_code:PTR BYTE
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
    jmp @exit
    
@not_found:
    ; Only display error message if needed
    invoke StdOut, ADDR subject_error
    
@exit:
    mov eax, found
    ret
FindSubjectDetailsQuiet ENDP

; Modified SelectSubjects procedure to show only selected subjects
; Modified SelectSubjects procedure with improved formatting
SelectSubjects PROC
    push ebx
    push esi
    push edi

    ; We're not resetting code_count here anymore
    ; This allows adding more subjects to existing selections

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
    
    ; Check if subject already in list
    mov ecx, 0
@check_duplicate:
    cmp ecx, code_count
    jge @not_duplicate
    
    ; Compare with existing codes
    push esi
    push edi
    
    mov eax, ecx
    imul eax, MAX_CODE_LEN
    lea esi, [selected_codes + eax]
    lea edi, [input_buffer]
    
@compare_existing:
    mov al, [esi]
    mov bl, [edi]
    
    test al, al
    jz @codes_match         ; End of first code
    test bl, bl
    jz @next_code          ; End of input code
    
    cmp al, bl
    jne @next_code
    
    inc esi
    inc edi
    jmp @compare_existing
    
@codes_match:
    ; Duplicate found, just continue
    pop edi
    pop esi
    jmp @input_loop
    
@next_code:
    pop edi
    pop esi
    inc ecx
    jmp @check_duplicate
    
@not_duplicate:
    ; Try to find subject without displaying it
    invoke FindSubjectDetailsQuiet, ADDR input_buffer
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
    
    ; Show current selection count with double newline for clarity
    invoke StdOut, ADDR newline    ; Add extra newline for clarity
    invoke StdOut, ADDR selected_header
    
    ; Show current selections count
    push eax
    mov eax, code_count
    invoke dwtoa, eax, ADDR temp_buffer
    invoke StdOut, ADDR temp_buffer
    invoke StdOut, ADDR code_separator
    pop eax
    
    invoke StdOut, ADDR newline    ; Add extra newline for clarity
    jmp @input_loop
    
@show_final:
    ; Display final list header with extra spacing
    invoke ClearScreen
    invoke StdOut, ADDR final_list
    invoke StdOut, ADDR newline    ; Add extra newline for clarity
    invoke StdOut, ADDR subject_header
    invoke StdOut, ADDR line_separator
    
    ; Show only selected subjects
    xor ecx, ecx
    
    ; Initialize total units counter
    mov ebx, 0  ; Use ebx to track total units
    
@display_loop:
    cmp ecx, code_count
    jge @done
    
    ; Get pointer to current code
    mov eax, ecx
    imul eax, MAX_CODE_LEN
    add eax, OFFSET selected_codes
    
    ; Display subject details
    push ecx
    push ebx        ; Save current units count
    invoke FindSubjectDetails, eax
    ; Units value is now in edx
    pop ebx         ; Restore units count
    add ebx, edx    ; Add units from this subject
    pop ecx
    
    inc ecx
    jmp @display_loop
    
@done:
    ; Add a separator line and units calculation
    invoke StdOut, ADDR newline
    invoke StdOut, ADDR total_units_line
    
    ; Convert total units to string and display
    push eax
    mov eax, ebx    ; Total units count
    invoke dwtoa, eax, ADDR temp_buffer
    invoke StdOut, ADDR temp_buffer
    pop eax
    
    ; Add extra space before prompt
    invoke StdOut, ADDR newline
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
    LOCAL preserve_selections:DWORD
    
    ; Initialize preserve flag
    mov preserve_selections, 0
    
@enroll_start:    ; Add this label for the option to restart enrollment
    ; Only clear selections if not preserving them
    .IF preserve_selections == 0
        ; Initialize code count
        mov code_count, 0
        
        ; Clear previous selections
        push edi
        mov edi, OFFSET selected_codes
        mov ecx, MAX_SELECTIONS * MAX_CODE_LEN
        mov al, 0
        rep stosb
        pop edi
    .ENDIF
    
    ; Reset preserve flag
    mov preserve_selections, 0
    
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
        invoke StdOut, ADDR newline
        invoke LoadSubjectFile, ADDR first_year_sem2
        invoke StdOut, ADDR newline
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
    invoke StdOut, ADDR subject_buffer
    invoke StdOut, ADDR newline
    
    ; Handle subject selection
    invoke SelectSubjects
    
    ; Wait for user input with options
    invoke StdOut, ADDR newline
    invoke StdOut, ADDR enhanced_options
    invoke StdIn, ADDR enroll_buffer, SIZEOF enroll_buffer
    
    ; Check user's choice
    mov al, [enroll_buffer]
    
    .IF al == '1'
        ; Return to Semester List - just return from procedure
        invoke ClearScreen
        ret
        
    .ELSEIF al == '2'
        ; Enroll more subjects (same year) - preserve selections
        mov preserve_selections, 1
        invoke ClearScreen
        jmp @enroll_start
        
    .ELSEIF al == '3'
        ; Back to Year Selection
        invoke ClearScreen
        ; Use an indirect approach to avoid the EXTERN issue
        push yearNum   ; Save yearNum just in case
        push 0         ; Return address will be popped by year_input
        jmp year_input ; Jump to year_input (no EXTERN needed)
        
    .ELSEIF al == '4'
        ; Exit program
        invoke ClearScreen
        invoke ExitProcess, 0
        
    .ELSE
        ; Invalid choice, return to semester list
        invoke ClearScreen
        ret
    .ENDIF
    ret
    
EnrollCourse ENDP