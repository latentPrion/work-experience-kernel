LIBC_A_DEPS:=libc/printf.o libc/start.o libc/string.o libc/rt.o

libc.a: ${LIBC_A_DEPS}
	${AR} -rcs $@ $^

clean-libc:
	rm -f libc.a ${LIBC_A_DEPS}
