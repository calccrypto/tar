CC?=gcc
CFLAGS=-Wall -std=c11
LFLAGS=
TARGET=libtar.a
AR=ar

.PHONY: clean-test

all: $(TARGET) exec

tar.o: tar.h tar.c
	$(CC) $(CFLAGS) -c tar.c

$(TARGET): tar.o
	$(AR) -r $(TARGET) tar.o

exec: $(TARGET) main.c
	$(CC) $(CFLAGS) main.c -o exec -ltar -L.

test: exec
	@# create fake directory entries
	touch file
	mkdir folder
	touch folder/a
	mkfifo pipe
	ln -s file sym
	mknod block b 1 2
	mknod char c 3 4

	@# archive the files with tar
	tar -cf test.tar file folder pipe sym block char char block sym pipe folder file

	@# remove data
	rm -r file folder pipe sym block char

	@# test extraction
	./exec x test.tar

	@# archive the files
	./exec c test.tar file folder pipe sym block char char block sym pipe folder file

	@# remove entries from archive
	./exec r test.tar folder/ block

	@# diff real tar and this tar
	tar -vtf test.tar > real
	./exec tv test.tar > out
	diff -bu real out
	@rm -f real out

	@# put them back
	./exec a test.tar block folder/

	@# diff real tar and this tar
	tar -vtf test.tar > real
	./exec tv test.tar > out
	diff -bu real out
	@rm real out

	@# extract the files with tar
	tar -xf test.tar

	@# clean up
	$(MAKE) clean-test

clean-test:
	rm -rf test.tar char block sym pipe folder file real out

clean: clean-test
	rm -f tar.o $(TARGET) ./exec
