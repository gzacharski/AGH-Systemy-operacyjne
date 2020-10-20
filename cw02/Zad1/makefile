CC=gcc
CFLAGS=-Wall -g
REPORT=wyniki.txt
SIZES=4 512 4096 8192
RECORDS_SORT=3000 6000
RECORDS_COPY=150000 300000

all: program

program: program.o mycopy.o mygenerate.o mylog.o mysort.o
	${CC} ${CFLAGS} program.o mycopy.o mygenerate.o mylog.o mysort.o -o $@

program.o: program.c
	$(CC) ${FLAGS} -c program.c

mycopy.o: mycopy.c
	$(CC) $(FLAGS) -c mycopy.c

mygenerate.o: mygenerate.c
	$(CC) $(FLAGS) -c mygenerate.c

mylog.o: mylog.c
	$(CC) $(FLAGS) -c mylog.c

mysort.o: mysort.c
	$(CC) $(FLAGS) -c mysort.c

report_sort:
	for size in ${SIZES} ; do \
		for record in ${RECORDS_SORT} ; do \
			echo $$record $$ size ; \
			./program generate daneLib $$record $$size ; \
			./program generate daneSys $$record $$size ; \
			./program sort daneLib $$record $$size lib >> $(REPORT) ; \
			./program sort daneSys $$record $$size sys >> $(REPORT) ; \
			rm daneLib daneSys ;\
		done ; \
	done ; \

report_copy:
	for size in ${SIZES} ; do \
		for record in ${RECORDS_COPY} ; do \
			echo $$record $$ size ; \
			./program generate daneLib $$record $$size ; \
			./program generate daneSys $$record $$size ; \
			./program copy daneLib daneLibCopy $$record $$size lib >> $(REPORT) ; \
			./program copy daneSys daneSysCopy $$record $$size sys >> $(REPORT) ; \
			rm daneLib daneSys daneLibCopy daneSysCopy ; \
		done ; \
	done ; \

clean:
	rm *.o *.txt program