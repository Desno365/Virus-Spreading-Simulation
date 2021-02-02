import argparse
from pwn import *
from pwnlib.replacements import sleep

parser = argparse.ArgumentParser()
parser.add_argument("pid", type=str)

args = parser.parse_args()
pidArg = int(args.pid)

context.terminal = ["terminator", "-e"]

gdb.attach(pidArg, """
    break main.cpp:65
    break main.cpp:172
    break main.cpp:209
    c
    set var ifl =7
    c
""")