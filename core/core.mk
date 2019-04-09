CORE_A_DEPS:=core/main.o core/multiboot.o core/idle_thread.o core/thread.o \
	core/scheduler.o core/fleas.o

core.a: ${CORE_A_DEPS}
	${AR} -rcs $@ $^

clean-core:
	rm -f core.a ${CORE_A_DEPS}
