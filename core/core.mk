CORE_A_DEPS:=core/main.o core/multiboot.o

core.a: ${CORE_A_DEPS}
	${AR} -rcs $@ $^

clean-core:
	rm -f core.a ${CORE_A_DEPS}
