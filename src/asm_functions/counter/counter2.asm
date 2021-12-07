
add, $r3, $zero, $imm, 2      # 0: R3 = 2 (core number)
lw,  $r2, $zero, $zero, 0     # 1: load R2 = mem[0] 
bne, $imm, $r2, $r3, 1        # 2: if(R2 != R3) go to line 1
add, $r4, $zero, $imm, 126    # 3: R4 = 126  (loops counter backwards - i )
add, $r2, $r2, $imm, 1        # 4: R2 = R2 + 1  (core num + 1)
sw,  $r2, $zero, $zero, 0     # 5: Mem[0] = R2 

add, $r2, $r2, $imm, 3        # 6: R2 = R2 + 3
lw,  $r5, $zero, $zero, 0     # 7: load R5 = mem[0] 
bne, $imm, $r5, $r2, 7        # 8: if(R5 != R2 ) -> back to line 7 (mem[0] wasn't increased by all cores yet )
add, $r5, $r5, $imm, 1        # 9: R5 = R5 + 1 
sw,  $r5, $zero, $zero, 0     # 10: Mem[0] = R5 
sub, $r4, $r4, $imm, 1        # 11: R4 = R4 - 1 (i--)
bne, $imm, $zero, $r4, 6      # 12: if(R4 != 0) -> line 6
add, $r2, $r5, $zero, 0       # 13: R2 = R5 

halt, $zero, $zero, $zero, 0     # 14: halt run - NEED TO CHECK VALUE WAS WRITTEN TO MAIN MEM 

