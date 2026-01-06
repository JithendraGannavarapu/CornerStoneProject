PUSH 2
STORE 0      # x = 2
#loop
LOAD 0
PUSH 10
CMP
JZ 20        # if x >= 10 exit

LOAD 0
PUSH 3
ADD
STORE 0
JMP 4

LOAD 0
HALT
