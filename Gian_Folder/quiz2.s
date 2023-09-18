addi x7, x0, 0                      # initialize i=0
addi x29, x0, 0                     # initialize j=0
loop1:  bge x7, x5, exit1           # i loop
        addi x7, x7, 1              # i++        
        
        loop2:  bge x29, x6, exit2  # j loop
                addi x29, x29, 1    # j++
                
                slli x28, x29, 4    # multiply j by 4 twice => shift left 4 bits 
                add x10, x10, x28   # add the offset to the base address
                addi x28, x7, x29   # i+j
                sw x28, x10         # store i+j into the right address D[4*j]

                j loop2    
        exit2:
        j loop1 
exit1: 
