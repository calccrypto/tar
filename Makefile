CC?=gcc
CFLAGS=-Wall -std=c99
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

test: exec clean-test
	@echo "create fake directory entries"
	@touch file
	@mkdir folder
	@touch folder/a
	@mkfifo pipe
	@ln -s file sym
	@mknod block b 1 2
	@mknod char c 3 4

	@echo "archive the files with GNU tar"
	@tar -cf test.tar file folder pipe sym block char char block sym pipe folder file

	@echo "remove original directory entries"
	@rm -r file folder pipe sym block char

	@echo "test extraction with tarball created by GNU tar"
	@./exec x test.tar || (echo "fail" && exit 1)

	@echo "test archive"
	@./exec c test.tar file folder pipe sym block char char block sym pipe folder file || (echo "fail" && exit 1)

	@echo "remove nonexistent entry from tarball"
	@./exec r test.tar nonexistent && (echo "fail" && exit 1) || true

	@echo "remove entries from tarball"
	@./exec r test.tar folder/ block || (echo "fail" && exit 1)

	@echo "diff tar -t and exec -t"
	@tar -vtf test.tar > real
	@./exec tv test.tar > out
	@diff -bu real out || (echo "fail" && exit 1)
	@rm -f real out

	@echo "restore removed entries"
	@./exec a test.tar block folder/ || (echo "fail" && exit 1)

	@echo "diff tar -t and exec -t"
	@tar -vtf test.tar > real
	@./exec tv test.tar > out
	@diff -bu real out || (echo "fail" && exit 1)
	@rm real out

	@echo "extract the files with GNU tar"
	@tar -xf test.tar || (echo "fail" && exit 1)

	@echo "clean up"
	@$(MAKE) clean-test

clean-test:
	rm -rf test.tar char block sym pipe folder file real out

clean: clean-test
	rm -f tar.o $(TARGET) ./exec
