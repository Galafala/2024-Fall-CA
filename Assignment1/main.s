.global __start

.data
  val: .string "Is valid perfect square"
  nonval: .string "Is not valid perfect square"


# float fdiv2(float n) {
#   uint32_t num = *(uint32_t*)&n;

#   num -= 0x00800000;

#   return *(float*)&num;
# }

fdiv2:
  addi sp, sp, -4
  sw ra, 0(sp)

  lw t0, 0(a0)
  addi t0, t0, -0x00800000

  sw t0, 0(sp)
  lw t0, 0(sp)

  lw ra, 0(sp)
  addi sp, sp, 4

fdiv2:
  addi sp, sp, -4
  sw ra, 0(sp)

  lw s0, 0(a0)
  addi s0, s0, -0x00800000

  sw s0, 0(sp)
  lw s0, 0(sp)

  lw ra, 0(sp)
  addi sp, sp, 4




