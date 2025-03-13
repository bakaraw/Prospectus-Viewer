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
    name_prompt db 9, 9, 9, "Student Name: ", 0
    studentId db  9, 9, 9, "ID Number: ", 0
    

  name_error  db 9, 9, 9, "Invalid name: Name must not contain numbers.", 13, 10, 0
    id_error    db 9, 9, 9, "Invalid ID: ID must contain 6 numeric digits only.", 13, 10, 0


    dept        db 9, 9, 9, "  College of Teacher Education", 10, 0
    header      db 9, 9, 9, "Bachelor of Secondary Education", 10, 0
    header2     db 9, 9, 9, "       Major in Filipino", 10, 10, 0
    header3     db 9, 9, 9, "           PROSPECTUS", 10, 10, 0
    prompt      db "Select your year level:", 10, 0
    year1txt    db 9, 9, "[1] 1st Year", 10, 0
    year2txt    db 9, 9, "[2] 2nd Year", 10, 0
    year3txt    db 9, 9, "[3] 3rd Year", 10, 0
    year4txt    db 9, 9, "[4] 4th Year", 10, 10, 0
    choice_Promt      db "Enter choice: ", 0


    prompt2     db "Select semester:", 10, 0
    sem1txt     db 9, 9, "[1] 1st Semester", 10, 0
    sem2txt     db 9, 9, "[2] 2nd Semester", 10, 0
    sem3txt     db 9, 9, "[3] Summer", 10, 10, 0
    enroll_opt  db 9, 9, "[4] Enroll", 10, 10, 0
        viewOptions db 10, "[1] Back to main menu [2] Back to Semester Menu [3] Exit: ", 0

   ; newline     db 13, 10, 0
       back_opt    db 9, 9, "[5] Back to Year Selection", 10, 10, 0


.DATA?
   inputBuffer db 4 dup(?)
    yearNum     dd ?
    semNum      dd ?
    studentName db 50 dup(?)  ; Buffer for student name (up to 50 chars)
    studentIdNum db 7 dup(?)

.CODE

main PROC
        invoke ClearScreen

name_input:
    ; Get student name
    invoke StdOut, ADDR name_prompt
    invoke StdIn, ADDR studentName, SIZEOF studentName
    
    ; Validate name (letters only)
    mov ecx, LENGTHOF studentName - 1  ; Maximum length
    xor esi, esi                      ; String index
    
    check_name_char:
        cmp BYTE PTR [studentName + esi], 0    ; Check for null terminator
        je name_valid                          ; If null, end of string
        
        ; Check if character is a letter (A-Z or a-z) or space
        mov al, BYTE PTR [studentName + esi]
        
        cmp al, ' '                   ; Check for space
        je valid_name_char
        
        cmp al, 'A'                   ; Check if below 'A'
        jb invalid_name
        cmp al, 'Z'                   ; Check if between 'A' and 'Z'
        jbe valid_name_char
        cmp al, 'a'                   ; Check if below 'a'
        jb invalid_name
        cmp al, 'z'                   ; Check if between 'a' and 'z'
        jbe valid_name_char
        
    invalid_name:
        invoke StdOut, ADDR name_error
        jmp name_input
        
    valid_name_char:
        inc esi
        loop check_name_char
        
name_valid:
    ; Name is valid, now get student ID
    
id_input:
    invoke StdOut, ADDR studentId
    invoke StdIn, ADDR studentIdNum, SIZEOF studentIdNum
    
    ; Validate student ID (6 digits only)
    xor esi, esi                      ; String index
    xor ecx, ecx                      ; Character counter
    
    check_id_char:
        cmp BYTE PTR [studentIdNum + esi], 0    ; Check for null terminator
        je check_id_length
        
        ; Check if character is a digit (0-9)
        mov al, BYTE PTR [studentIdNum + esi]
        
        cmp al, '0'                   ; Check if below '0'
        jb invalid_id
        cmp al, '9'                   ; Check if above '9'
        ja invalid_id
        
        inc esi
        inc ecx
        cmp ecx, 6                    ; Max 6 digits
        jbe check_id_char
        
    invalid_id:
        invoke StdOut, ADDR id_error
        jmp id_input
        
    check_id_length:
        cmp ecx, 6                    ; Must be exactly 6 digits
        jne invalid_id
        
    ; Student ID is valid, continue with program
    


year_input PROC ;
               invoke ClearScreen
            
               invoke StdOut, ADDR dept
               invoke StdOut, ADDR header
               invoke StdOut, ADDR header2
               invoke StdOut, ADDR header3
                
                 ; Display the fixed student name
               invoke StdOut, ADDR name_prompt
               invoke StdOut, ADDR studentName
               invoke StdOut, ADDR newline
               
               ; Display the fixed student ID
               invoke StdOut, ADDR studentId
               invoke StdOut, ADDR studentIdNum
               invoke StdOut, ADDR newline
               
               invoke StdOut, ADDR prompt
               invoke StdOut, ADDR year1txt
               invoke StdOut, ADDR year2txt
               invoke StdOut, ADDR year3txt
               invoke StdOut, ADDR year4txt
               
                invoke StdOut, ADDR choice_Promt             
            
               invoke StdIn, ADDR inputBuffer, SIZEOF inputBuffer
               invoke atodw, ADDR inputBuffer
               mov    yearNum, eax
               invoke dwtoa, yearNum, ADDR inputBuffer

     .if yearNum < 1 || yearNum > 4
        jmp year_input
    .ENDIF
   
    
    sem_input:
            invoke ClearScreen

      invoke StdOut, ADDR dept
               invoke StdOut, ADDR header
               invoke StdOut, ADDR header2
               invoke StdOut, ADDR header3
              invoke StdOut, ADDR prompt2
              invoke StdOut, ADDR sem1txt
              invoke StdOut, ADDR sem2txt

              .if yearNum < 3  ; 1st & 2nd year students have summer
                  invoke StdOut, ADDR sem3txt
              .endif
               invoke StdOut, ADDR enroll_opt 
                   invoke StdOut, ADDR back_opt      ; Add the back option
                              invoke StdOut, ADDR choice_Promt             

              invoke StdIn, ADDR inputBuffer, SIZEOF inputBuffer
              invoke atodw, ADDR inputBuffer
              mov    semNum, eax
              invoke dwtoa, semNum, ADDR inputBuffer
              

               .if semNum == 5                   ; Check for the back option
        jmp year_input                ; Go back to year input
        invoke ClearScreen
    .endif

         .if semNum == 4   ; Handle the enroll option
         invoke ClearScreen
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

          invoke StdOut, ADDR viewOptions
          invoke StdIn, ADDR inputBuffer, SIZEOF inputBuffer

        mov al, byte ptr [inputBuffer]
        .IF al == '1'
            ; Option 1: Select again (go back to year selection)
            jmp year_input
        .ELSEIF al == '2'
            ; Option 2: Back to semester selection for current year
            jmp sem_input
        .ELSE
            ; Option 3 or anything else: Exit
            invoke ExitProcess, 0
        .ENDIF
year_input ENDP
main ENDP

END main
