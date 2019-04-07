DRIVERS_A_DEPS:=drivers/vga.o drivers/rs232.o

drivers.a: ${DRIVERS_A_DEPS}
	${AR} -rcs $@ $^

clean-drivers:
	rm -f drivers.a ${DRIVERS_A_DEPS}
