CC=gcc
CFLAGS=-Wall -g
BINS=mainStatic mainShared mainDynamic
REPORT=report_zad2.txt

all: $(BINS)

mainStatic: main.o libmystaticcode.a
	$(CC) $(CFLAGS) main.o -l mystaticcode -L ./ -o $@

mainShared: libmycode.so
	$(CC) $(CFLAGS) -o $@ main.c -L. -lmycode

mainDynamic: libmycode.so
	$(CC) $(CFLAGS) -c mainDynamic.c
	$(CC) $(CFLAGS) -o $@ mainDynamic.o libmycode.so -ldl
	
main.o: main.c
	$(CC) $(CFLAGS) -c main.c

libmystaticcode.a: libmycode.o
	ar rcs libmystaticcode.a libmycode.o

libmycode.o: libmycode.c mycode.h
	$(CC) $(CFLAGS) -c libmycode.c

libmycode.so: libmycode.c mycode.h
	$(CC) $(CFLAGS) -fPIC -shared -o $@ libmycode.c -lc 

useStatic: mainStatic
	./mainStatic

useShared: mainShared
	LD_LIBRARY_PATH=./ ./mainShared

useDynamic:
	./mainDynamic

report: $(BINS)
	echo "\nStatic library" >> $(REPORT)
	./mainStatic >> $(REPORT)

	echo "\nShared library" >> $(REPORT)
	LD_LIBRARY_PATH=./ ./mainShared >> $(REPORT)

	echo "\nDynamic library" >> $(REPORT)
	./mainDynamic >> $(REPORT)

	clear
	cat $(REPORT)

clean:
	rm *.o *.so *.a mainStatic mainShared mainDynamic