##Write a RISC-V assembly language program to count the number of ones in a 32- bit number

lw x1, 0x11111111   ## Input
lw x2, 0x1          ## Mask
li x10, 0           ## 1's counter

loop:
    and x11, x1, x2
    bnez x10, count
    srl x1, x1, 1
    j done

count:
    addi x10, x10, 1

done:
    bnez x1, loop
    mv  x20, x10