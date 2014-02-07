all:
	clang rickascii.c -DDEBUG -o rickascii
	bzcat rick.bz2 | ./rickascii

tinycore:
	cat rickascii.c | ssh zoidberg 'gcc -x c - -std=gnu99 -m32 -pipe -o /dev/stdout' > rickascii_tc
	chmod +x rickascii_tc
