  .globl main
  .intel_syntax

main:
  call get_ip
  ret

get_ip:
  mov %rax, [%rsp]     
  ret
