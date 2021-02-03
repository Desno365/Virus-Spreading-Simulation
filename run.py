
from pwn import *
r = process("make debug",shell=True)
r.recvuntil("PID ")
pid = int(r.recvuntil("\n")[:-1])
print(pid)

r.wait_for_close()