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
  li t0, 0x00800000
  li a0, 0x3f800000
  sub a0, a0, t0




