import argparse
from pwn import *
import sys
# NOTE: if you want to debug another process change the DEBUG option in the Makefile
# NOTE: you can stop execution in the main method with:(use the DEBUGGED PROCESSOR value in which you are interested in)
# if(my_rank==0) asm("int $3");   


# Setup the launched terminal. You have to install the Terminator terminal if you want to use it with:
# $ sudo apt-get install terminator
# In order for a better debug use https://github.com/pwndbg/pwndbg
# The usage of gdb.attach() requires to execute the following line:
# $ echo 0 > /proc/sys/kernel/yama/ptrace_scope
context.terminal = ["terminator", "-e"]


# # Make the program
r = process("make -B",shell=True)
print("receving line")
makeOutput = r.recvall().decode("utf-8")
r.wait_for_close()

if "error" in makeOutput:
    print(makeOutput)
    sys.exit("Cannot compile")

# Get the pid of the process to attach from the arguments of the program.
# parser = argparse.ArgumentParser()
# parser.add_argument("pid", type=str)
# args = parser.parse_args()
# pidArg = int(args.pid)
# New version start the program and obtain automatically the PID returned by it.
r = process("make debug",shell=True)
r.recvuntil("PID ")
pid = int(r.recvuntil("\n")[:-1])

# Attach GDB to the program, here you can specify the instructions that gbd has to execute before
# returning the control.
# NOTE: the first break has always to coincide with the address of the sleep.
gdb.attach(pid, """
    break main.cpp:71
    break main.cpp:314
    break main.cpp:290
    c
    set var ifl =7
    c
""")

# Wait for the process to terminate execution
r.wait_for_close()

