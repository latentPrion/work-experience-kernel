%.a: %.o
	${AR} -rcs $@ $?

%.elf: %.a
	${LD} ${LDFLAGS} -o $@ --start-group $? --end-group
