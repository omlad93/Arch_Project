# based on the code from cache opt lecture 5 (blocking 2) 

add,  2, 0, 1, 16       # 0: R2 = 16 constant 
add,  3, 0, 0, 0        # 1: R3 = 0 (jj)

add,  4, 0, 0, 0        # 2: R4 = 0 (kk) -- THIS IS J 

add,  5, 0, 0, 0        # 3: R5 = 0 (i)  -- THIS is K
mul, 12, 5, 1, 16       # 4: R12 = 16*R5 (16*i)  THIS IS U 

add, 9, 3, 1, 8         # 5: R9 = R3 + 8 ( jj + 8 ) --- Min condition j for loop
blt, 1, 9, 2, 9         # 6: if(R9<R2) (jj+B < N)          CHANGE W 
add,  6, 3, 0, 0        # 7: R6 = R3 (j = jj) - heppend any way 
add, 9, 2, 0, 0         # 8: R9 = R2 (16) When the min is 16
add, 13, 0, 0, 0        # 9: R13 = 0 (r = 0)   --- THIS IS W & E 

add, 10, 4, 1, 8        # 10: R10 = R4 + 8 (kk + B(8) ) --- Min condition k for loop
blt, 1, 10, 2, 14        # 11: if(R10<R2) (kk+B < N)          CHANGE L 
add,  7, 4, 0, 0        # 12: R7 = R4 (k = kk) - heppend any way
add, 10, 2, 0, 0        # 13: R10 = R2 (16)   WHEN THE MIN IS N (16)

lw,  8, 12, 7, 0        # 14: R8 = Mem[R12 + R7] ( Y[i][k] = Mem[16*i + k]) ---- THIS IS L and Q 
mul, 11, 7, 1, 16       # 15: R11 = R7 * 16 (16 * k )
add, 11, 11, 1, 256     # 16: R11 = R11 + 256 ( R11 = 16 * k + 256 )
lw,  14, 11, 6, 0       # 17: R14 = Mem[R6 + R11] (Z[k][i] = Mem[256 + 16*k +j] )
mul, 14, 14, 8, 0       # 18: R14 = R14 * R8 (Z[k][i] * Y[i][k] )
add, 13, 13, 14, 0      # 19: R13 = R13 + R14 ( r = r + Z[k][i] * Y[i][k] )

add, 7, 7, 1, 1         # 20: R7 = R7 + 1 ( k++)
blt, 1, 7, 10, Q        # 21: if(R7 < R10 ) (k < min(kk+b,N)) -> go to start of for loop k 
mul, 11, 5, 1, 16       # 22: R11 = R5 * 16 ( i*16)  prepare to sw
add, 11, 11, 1, 512     # 23: R11 = R11 + 512 ( i*16 +512) 
sw, 13, 11, 6, 0        # 24: Mem[R11 + R6] = R13  ( Mem[i*16 +512 +j] = r ) -- saving X[i][j] 

add, 6, 6, 1, 1         # 25: R6 = R6 + 1 ( j++)
blt, 1, 6, 9, 9         # 26: if(R6 < R9 ) (j < min(jj+B,N)) -> got to start of for j 
add, 0, 0, 0, 0         # 27: DOING NOTHING 

add, 5, 5, 1, 1         # 28: R5 = R5 + 1 ( i++)
blt, 1, 5, 2, 4         # 29: if(R5 < R2 ) (i < 16)) -> got to start of for i U
add, 0, 0, 0, 0         # 30: DOING NOTHING 

add, 4, 4, 1, 8         # 31: R4 = R4 + 8 ( kk = kk + 8 )
blt, 1, 4, 2, 3         # 32: if(R4 < R2 ) (kk < 16)) -> got to start of for kk 
add, 0, 0, 0, 0         # 33: DOING NOTHING 

add, 3, 3, 1, 8         # 34: R3 = R3 + 8 ( jj = jj + 8 )
blt, 1, 3, 2, 2         # 35: if(R3 < R2 ) (jj < 16)) -> got to start of for jj 
add, 0, 0, 0, 0         # 36: DOING NOTHING 
halt, 0, 0, 0, 0        # 37: halt - end of run 


