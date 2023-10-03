slli x11, x11, 16 # B base address + 8*4 = base address + 2^5 => shift left 5 times

slli x28, x28, 2 # i = i*4 => get right address offset 
slli x29, x29, 2 # j = j*4 => get right address offset

add x5, x10, x28 # A[i] address = b.a. + i 
lw x6, 0(x5) # x6 = First part of the element A[i]
slli x5, x5, 2 # get address of the second part since each element is 8-byte word
lw x5, 0(x5) # x5 = Second part of element A[i]

add x10, x10, x29 # A[j] address = b.a. + j 
lw x7, 0(x10) # x7 = First part of the element A[j]
slli x10, x10, 2 # get the second part since each element is 8-byte word
lw x10, 0(x10) # x10 = Second part of element A[j]

# x6 + x7 = first part of the addition A[i] + A[j] => x6
# x5 + x10 = second part of the addition  A[i] + A[j] => x10
add x6, x6, x7 
add x10, x5, x10

sw x6, 0(x11) # store first part of addition at B[8]
slli x11, x11, 2 # get address of the second part since each element is 8-byte word
sw x10, 0(x11) # store second part of addition at B[8] + 1 memory cell


