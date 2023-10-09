#Bubble sort in C 
#void bubsort(int *array, int size) {
    #bool swapped;
    #do {
        #swapped = false;
        #for (int i = 1;i < size;i++) {
            #if (array[i-1] > array[i]) {
               #swapped = true;
               #long tmp = array[i-1];
               #array[i-1] = array[i];
               #array[i] = tmp;
           # }
        #}
    #} while (swapped);
#}

#Bubble Sort in Risc-V Assembly
.data
    .align 3
    array: .word 5, 3, 6, 8, 9, 1, 4, 7, 2, 10   # Integer array to be sorted
    size: .word 10                               # Size of the array

.text
    .globl bubsort

bubsort:
    # a0 = int *array
    # a1 = size
    # t0 = swapped
    # t1 = i
1:  # do loop
    li t0, 0         # swapped = false
    li t1, 1         # i = 1
2:  # for loop
    lw t3, a1        # Load the size
    bge t1, t3, 2f   # break if i >= size
    slli t4, t1, 2   # scale i by 4 (for int)
    add t4, a0, t4   # new scaled memory address
    lw t5, -4(t4)    # load array[i-1] into t5
    lw t6, 0(t4)     # load array[i] into t6
    ble t5, t6, 3f   # if array[i-1] <= array[i], it's in position

    # if we get here, we need to swap
    li t0, 1         # swapped = true
    sw t5, 0(t4)     # array[i] = array[i-1]
    sw t6, -4(t4)    # array[i-1] = array[i]
3:  # bottom of for loop body
    addi t1, t1, 1   # i++
    j 2b              # loop again 

2:  # bottom of do loop body
    bnez t0, 1b      # loop if swapped = true
    ret              # return via return address register
