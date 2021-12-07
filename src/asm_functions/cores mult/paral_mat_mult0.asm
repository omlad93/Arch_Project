add,  $r2, $zero, $imm, 16       # 0: R2 = 16 constant 
add,  $r3, $zero, $imm, 8        # 1: R3 = 8 constant    MAYBE DONT NEED 
add,  $r4, $zero, $imm, 4        # 2: R4 = 4 constant 
add,  $r5, $zero, $zero, 0       # 3: R5 = 0     (l)
add,  $r6, $zero, $zero, 0       # 4: R6 = 0     (i)             START FOR LOOP L
add,  $r15, $r5, $imm, 8         # 5: R15 = R5 + 8  (R15 = l + 8)
mul,  $r9, $r6, $imm, 16         # 6: R9 = 16*R6 (16*i)          START FOR LOOP I 

add,  $r7,  $r5, $zero, 0        # 7: R7 = R5    (j = l) 
add,  $r13, $zero, $zero, 0      # 8: R13 = 0  (RESULT)          START FOR LOOP J 
add,  $r8,  $zero, $zero, 0      # 9: R8 = 0     (k) 
mul,  $r10, $r8, $imm, 16        #10: R10 = 16*R8 (16*k)         START FOR LOOP K

lw,   $r11, $r9,  $r8, 0         #11: R11 = Mem[R9 + R8] ( A[i][k] = Mem[16*i + k]) 
add,  $r16, $r7,  $imm, 256      #12: R16 = R7 + 256  (R16 = j +256 )
lw,   $r12, $r10, $r16, 0        #13: R12 = Mem[R10 + R16 ] ( B[k][j] = Mem[16*k + j + 256])
mul,  $r12, $r12, $r11, 0        #14: R12 = R12 * R11  (R12 =A[i][k] * B[k][j] )
add,  $r13, $r13, $r12, 0        #15: R13 = R13 + R12  (RESULT = RESULT +A[i][k] * B[k][j] )
   
add,  $r8, $r8, $imm, 1          #16: R8 = R8 ++     (k++ )   
blt,  $imm, $r8, $r2, 10         #17: if(R8 < R2 ) (k < 16) -> go to start of for loop k 
   
add,  $r14, $r9, $imm, 512       #18: R14 = R9 + 512  (R14 = 16*i + 512)
sw,   $r13, $r14, $r7, 0         #19: Mem[R14 + R7] = R13  ( Mem[i*16 +512 +j] = result ) -- saving C[i][j] 
   
add,  $r7, $r7, $imm, 1          #20: R7 = R7 ++     (j++ )   
blt,  $imm, $r7, $r15, 8         #21: if(R7 < R15 ) (j < l+8) -> go to start of for loop j 
add,  $zero, $zero, $zero, 0     #22: DOING NOTHING 
  
add,  $r6, $r6, $imm, 1          #23: R6 = R6 ++     (i++ )   
blt,  $imm, $r6, $r4, 6          #24: if(R6 < R4 ) (i < 4) -> go to start of for loop i 
add,  $zero, $zero, $zero, 0     #25: DOING NOTHING 
  
add,  $r5, $r5, $imm, 8          #26: R5 = R5 + 8      (l = l + 8 )   
blt,  $imm, $r5, $r2, 4          #27: if(R5 < R2 ) (l < 16) -> go to start of for loop l 
add,  $zero, $zero, $zero, 0     #28: DOING NOTHING 
halt, $zero, $zero, $zero, 0     #29: halt - end of run 
