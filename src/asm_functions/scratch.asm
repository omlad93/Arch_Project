add,  2, 0, 1, 16       # 0: R2 = 16 constant 
add,  3, 0, 1, 8        # 1: R3 = 8 constant    MAYBE DONT NEED 
add,  4, 0, 1, 4        # 2: R4 = 4 constant 
add,  5, 0, 0, 0        # 3: R5 = 0     (l)
add,  6, 0, 0, 0        # 4: R6 = 0     (i)             START FOR LOOP L
add, 15, 5, 1, 8        # 5: R15 = R5 + 8  (R15 = l + 8)
mul, 9, 6, 1, 16        # 6: R9 = 16*R6 (16*i)          START FOR LOOP I 

add,  7, 5, 0, 0        # 7: R7 = R5    (j = l) 
add, 13, 0, 0, 0        # 8: R13 = 0  (RESULT)          START FOR LOOP J 
add,  8, 0, 0, 0        # 9: R8 = 0     (k) 
mul, 10, 8, 1, 16       #10: R10 = 16*R8 (16*k)         START FOR LOOP K

lw,  11, 9,  8, 0       #11: R11 = Mem[R9 + R8] ( A[i][k] = Mem[16*i + k]) 
add, 16, 7,  1, 256     #12: R16 = R7 +256  (R16 = j +256 )
lw,  12, 10, R16, 0     #13: R12 = Mem[R10 + R16 ] ( B[k][j] = Mem[16*k + j + 256])
mul, 12, 12, 11, 0      #14: R12 = R12 * R11  (R12 =A[i][k] * B[k][j] )
add, 13, 13, 12, 0      #15: R13 = R13 + R12  (RESULT = RESULT +A[i][k] * B[k][j] )

add,  8, 8, 1, 1        #16: R8 = R8 ++     (k++ )   
blt, 1, 8, 2, 10         #17: if(R8 < R2 ) (k < 16) -> go to start of for loop k 

add, 14, 9, 1, 512      #18: R14 = R9 + 512  (R14 = 16*i + 512)
sw, 13, 14, 7, 0        #19: Mem[R14 + R7] = R13  ( Mem[i*16 +512 +j] = result ) -- saving C[i][j] 

add,  7, 7, 1, 1        #20: R7 = R7 ++     (j++ )   
blt, 1, 7, 15, 8        #21: if(R7 < R15 ) (j < l+8) -> go to start of for loop j 
add, 0, 0, 0, 0         #22: DOING NOTHING 

add,  6, 6, 1, 1       #23: R6 = R6 ++     (i++ )   
blt,  1, 6, 4, 6       #24: if(R6 < R4 ) (i < 4) -> go to start of for loop i 
add, 0, 0, 0, 0        #25: DOING NOTHING 

add,  5, 5, 1, 8       #26: R5 = R5 + 8      (l = l + 8 )   
blt,  1, 5, 2, 4       #27: if(R5 < R2 ) (l < 16) -> go to start of for loop l 
add,  0, 0, 0, 0       #28: DOING NOTHING 
halt, 0, 0, 0, 0       #29: halt - end of run 

# THIS IS ORIGINAL MULT CORE 0 



