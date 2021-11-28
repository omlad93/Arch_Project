add, 3, 0, 1, 3      # 0: R3 = 3 (core number)
lw,  2, 0, 0, 0      # 1: load R2 = mem[0] 
bne, 1, 2, 3, 1      # 2: if(R2 != R3) go to line 1
add, 4, 0, 1, 127    # 3: R4 = 127  (loops counter backwards - i )
add, 2, 2, 1, 1      # 4: R2 = R2 + 1  (core num + 1)
sw,  2, 0, 0, 0      # 5: Mem[0] = R2 

add, 2, 2, 1, 3      # 6: R2 = R2 + 3
lw,  5, 0, 0, 0      # 7: load R5 = mem[0] 
bne, 1, 5, 2, 7      # 8: if(R5 != R2 ) -> back to line 7 (mem[0] wasn't increased by all cores yet )
add, 5, 5, 1, 1      # 9: R5 = R5 + 1 
sw,  5, 0, 0, 0      # 10: Mem[0] = R5 
sub, 4, 4, 1, 1      # 11: R4 = R4 - 1 (i--)
bne, 1, 0, 4, 6      # 12: if(R4 != 0) -> line 6
add, 2, 5, 0, 0      # 13: R2 = R5 

lw, 9, 0, 1, 256     # 14: R9 = Mem[256] ( dumy op just to make miss conflict )
halt, 0, 0, 0, 0     # 15: halt run - NEED TO CHECK VALUE WAS WRITTEN TO MAIN MEM 