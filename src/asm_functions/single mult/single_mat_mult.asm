# based on the code from cache opt lecture 5 (blocking 2) 

add, $r2, $zero, $imm, 16       # 0: R2 = 16 constant 
add, $r3, $zero, $zero, 0        # 1: R3 = 0 (jj)

add, $r4, $zero, $zero, 0        # 2: R4 = 0 (kk) -- THIS IS J 

add, $r5, $zero, $zero, 0        # 3: R5 = 0 (i)  -- THIS is K
mul, $imm2, $r5, $imm, 16       # 4: R12 = 16*R5 (16*i)  THIS IS U 

add, $r9, $r3, $imm, 8         # 5: R9 = R3 + 8 ( jj + 8 ) --- Min condition j for loop
blt, $imm, $r9, $r2, 9         # 6: if(R9<R2) (jj+B < N)          CHANGE W 
add, $r6, $r3, $zero, 0        # 7: R6 = R3 (j = jj) - heppend any way 
add, $r9, $r2, $zero, 0         # 8: R9 = R2 (16) When the min is 16
add, $imm3, $zero, $zero, 0        # 9: R13 = 0 (r = 0)   --- THIS IS W & E 

add, $imm0, $r4, $imm, 8        # 10: R10 = R4 + 8 (kk + B(8) ) --- Min condition k for loop
blt, $imm,  $imm0, $r2, 14        # 11: if(R10<R2) (kk+B < N)          CHANGE L 
add, $r7,  $r4, $zero, 0        # 12: R7 = R4 (k = kk) - heppend any way
add, $imm0, $r2, $zero, 0        # 13: R10 = R2 (16)   WHEN THE MIN IS N (16)

lw,  $r8, $imm2, $r7, 0        # 14: R8 = Mem[R12 + R7] ( Y[i][k] = Mem[16*i + k]) ---- THIS IS L and Q 
mul, $imm1, $r7, $imm, 16       # 15: R11 = R7 * 16 (16 * k )
add, $imm1, $imm1, $imm, 256     # 16: R11 = R11 + 256 ( R11 = 16 * k + 256 )
lw,  $imm4, $imm1, $r6, 0       # 17: R14 = Mem[R6 + R11] (Z[k][i] = Mem[256 + 16*k +j] )
mul, $imm4, $imm4, $r8, 0       # 18: R14 = R14 * R8 (Z[k][i] * Y[i][k] )
add, $imm3, $imm3, $imm4, 0      # 19: R13 = R13 + R14 ( r = r + Z[k][i] * Y[i][k] )

add, $r7,  $r7, $imm, 1         # 20: R7 = R7 + 1 ( k++)
blt, $imm,  $r7, $imm0, 14        # 21: if(R7 < R10 ) (k < min(kk+b,N)) -> go to start of for loop k 
mul, $imm1, $r5, $imm, 16       # 22: R11 = R5 * 16 ( i*16)  prepare to sw
add, $imm1, $imm1, $imm, 512     # 23: R11 = R11 + 512 ( i*16 +512) 
sw,  $imm3, $imm1, $r6, 0        # 24: Mem[R11 + R6] = R13  ( Mem[i*16 +512 +j] = r ) -- saving X[i][j] 

add, $r6, $r6, $imm, 1         # 25: R6 = R6 + 1 ( j++)
blt, $imm, $r6, $r9, 9         # 26: if(R6 < R9 ) (j < min(jj+B,N)) -> got to start of for j 
add, $zero, $zero, $zero, 0         # 27: DOING NOTHING 

add, $r5, $r5, $imm, 1         # 28: R5 = R5 + 1 ( i++)
blt, $imm, $r5, $r2, 4         # 29: if(R5 < R2 ) (i < 16)) -> got to start of for i U
add, $zero, $zero, $zero, 0         # 30: DOING NOTHING 

add, $r4, $r4, $imm, 8         # 31: R4 = R4 + 8 ( kk = kk + 8 )
blt, $imm, $r4, $r2, 3         # 32: if(R4 < R2 ) (kk < 16)) -> got to start of for kk 
add, $zero, $zero, $zero, 0         # 33: DOING NOTHING 

add, $r3, $r3, $imm, 8         # 34: R3 = R3 + 8 ( jj = jj + 8 )
blt, $imm, $r3, $r2, 2         # 35: if(R3 < R2 ) (jj < 16)) -> got to start of for jj 
add, $zero, $zero, $zero, 0         # 36: DOING NOTHING 
halt, $zero, $zero, $zero, 0        # 37: halt - end of run 
