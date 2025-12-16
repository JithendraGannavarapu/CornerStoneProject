CC = gcc
CFLAGS = -Wall -Wextra -g

all: dbg target

# Compile the debugger
dbg: debugger.c
	$(CC) $(CFLAGS) debugger.c -o dbg

# Compile the target binary (Disable PIE for fixed addresses)
target: sample_code.c
	$(CC) -g -no-pie -fno-pie sample_code.c -o target

clean:
	rm -f dbg target