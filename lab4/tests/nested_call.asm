# Nested function calls
CALL F
HALT

F:
CALL G
PUSH 10
ADD
RET

G:
PUSH 20
RET
