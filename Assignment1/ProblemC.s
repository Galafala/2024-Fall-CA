.data
str1: .asciz "'s leading zero "
str2: .asciz "is "
str3: .asciz "\nThe number of "
str4: .asciz "undefined"

.text
.globl main

main:
  li t0, 0
  sw t0, 0(s0)
  li t0, 0x345
  sw t0, 4(s0)
  li t0, 0x80000000
  sw t0, 8(s0)
  li t0, 0x1
  sw t0, 12(s0)
  li t0, 0xc2440000
  sw t0, 16(s0)
  li t0, 0xf00001
  sw t0, 20(s0)
init:
  li s1, 6 # test data number
  li s2, 32 # undefined number
  addi sp, sp, -4 # save sp
main_loop:
  lw a3, 0(s0) # a3 = a[i]
  jal ra, clz32 # call clz32 function
  jal ra, print
  addi s0, s0, 4 # a[i+1]
  addi s1, s1, -1 # s1 -= 1
  bnez s1, main_loop # if s1 != 0, loop the next test data
exit:
  addi sp, sp, 4 # restore sp
  li a7, 93 # exit syscall number
  ecall
clz32:
  bnez a3, none_zero # if a3 != 0, jump to none_zero
  is_zero:
    li t0, 32 # a3 = 32
    sw t0, 0(sp) # save the clz answer
    jalr zero, ra, 0 # return
  none_zero:
    li t2, 0 # count
    li t0, 31 # int i = 31
  clz32_loop:
    li t1, 1 # 1U
    sll t1, t1, t0 # (1U << i)
    and t3, a3, t1 # x & (1U << i)
    addi t0, t0, -1 # i--
    addi t2, t2, 1 # count++
    beqz t3, clz32_loop # if(x & (1U << i)) == 0 (false)
    addi t2, t2, -1 # count--
    sw t2, 0(sp) # save count
    jalr zero, ra, 0 # return
print:
  la a0, str3 # "\nThe number of "
  li a7, 4 # Print string syscall number
  ecall
  mv a0, a3 # test data
  li a7, 35 # Print binary syscall number
  ecall
  la a0, str1 # "'s leading zero "
  li a7, 4 # Print string syscall number
  ecall  
  la a0, str2 # "is "
  li a7, 4 # Print string syscall number
  ecall
  lw a4, 0(sp)
  beq a4, s2, undifine
  difine:
    lw a0, 0(sp) # clz answer
    li a7, 1 # Print integer syscall number
    ecall
    jalr zero, ra, 0
  undifine:
    la a0, str4 # "\nThe number of "
    li a7, 4 # Print string syscall number
    ecall
    jalr zero, ra, 0   