.data
str1: .asciz "\nThe positive number of "
str2: .asciz " is "
str3: .asciz "\nThe number of leading zero of "

.text
.globl main

main:
    addi sp, sp, -24
    li t0, 0xc2440000
    sw t0, 0(sp)
    li t0, 0x001f0456
    sw t0, 4(sp)
    li t0, 0x80000000
    sw t0, 8(sp)
    li t0, 0x00000001
    sw t0, 12(sp)
    li t0, 0x8cc0ffff
    sw t0, 16(sp)
    li t0, 0x00f00001
    sw t0, 20(sp)
    addi sp, sp, 24
    li s3, -24
    li s4, 24
  
main_loop:
    add sp, sp, s3
    lw s0, 0(sp) 
    lw s1, 4(sp)
    add sp, sp, s4
    
    addi s3, s3, 8
    addi s4, s4, -8
    
    # call function part
    addi sp, sp, -36
    
    jal ra, fabsf
        
    
    jal ra, clz32
 
    jal ra, print
    
    addi sp, sp, 36
    
    # loop the next test data
    bnez s4, main_loop
    
    # exit
    li a7, 93     
    ecall
           

fabsf:
    li t0, 0x7fffffff
    and a0, s0, t0
    
    # Store the result into stack
    mv t1, a0
    sw t1, 4(sp)
 
    jalr zero, ra, 0
    
clz32:
    sw ra, 0(sp)
    li t0, 31 # int i = 31
    li t2, 0 # count
 
    jal ra, clz32_loop
    
    addi t2, t2, -1 # count--
    sw t2, 8(sp)
    
    lw ra, 0(sp)
    jalr zero, ra, 0

clz32_loop:
    li t1, 1 # 1U
    sll t1, t1, t0 # (1U << i)
    and t3, s1, t1 # x & (1U << i)
    addi t0, t0, -1 # i--
    addi t2, t2, 1 # count++
    beqz t3, clz32_loop # if(x & (1U << i)) == 0 (false)
    jalr zero, ra, 0

print:
    li a0, 1
    la a1, str1
    li a2, 24 # String length
    li a7, 64 # Print string syscall number  
    ecall
    
    mv a0, s0
    li a7, 34
    ecall
 
    li a0, 1
    la a1, str2 #is
    li a2, 4
    li a7, 64 # Print string syscall number
    ecall
 
    lw a0, 4(sp)
    li a7, 34
    ecall
 
    li a0, 1
    la a1, str3
    li a2, 31
    li a7, 64 # Print string syscall number
    ecall
    
    mv a0, s1
    li a7, 34
    ecall
    
    li a0, 1
    la a1, str2 # is
    li a2, 4
    li a7, 64 # Print string syscall number
    ecall
 
    lw a0, 8(sp)
    li a7, 34
    ecall
    
    jalr zero, ra, 0
