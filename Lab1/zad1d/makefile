CC=gcc -Wall
C_FILE=main.c
S_FILE=main.s
X_FILE=main
REPORT=report_zad1d.txt

all: 
	$(CC) $(C_FILE) -o $(X_FILE)

asm-O0:
	$(CC) -O0 -S -o $(S_FILE) $(C_FILE)

asm-O:
	$(CC) -O -S -o $(S_FILE) $(C_FILE)

asm-O1:
	$(CC) -O1 -S -o $(S_FILE) $(C_FILE)

asm-O2:
	$(CC) -O2 -S -o $(S_FILE) $(C_FILE)

asm-O3:
	$(CC) -O3 -S -o $(S_FILE) $(C_FILE)

asm-Os:
	$(CC) -Os -S -o $(S_FILE) $(C_FILE)

O0:
	$(CC) -O0 $(C_FILE) -o $(X_FILE)

O:
	$(CC) -O $(C_FILE) -o $(X_FILE)

O1:
	$(CC) -O1 $(C_FILE) -o $(X_FILE)

O2:
	$(CC) -O2 $(C_FILE) -o $(X_FILE)

O3:
	$(CC) -O3 $(C_FILE) -o $(X_FILE)

Os:
	$(CC) -Os $(C_FILE) -o $(X_FILE)

report: 
	make clean

	echo "\nFlag: -O0 (default)" >> $(REPORT)
	make O0
	./main >> $(REPORT)

	echo "\nFlag: -O" >> $(REPORT)
	make O
	./main >> $(REPORT)

	echo "\nFlag: -O1" >> $(REPORT)
	make O1
	./main >> $(REPORT)

	echo "\nFlag: -O2" >> $(REPORT)
	make O2
	./main >> $(REPORT)

	echo "\nFlag: -O3" >> $(REPORT)
	make O3
	./main >> $(REPORT)

	echo "\nFlag: -Os" >> $(REPORT)
	make Os
	./main >> $(REPORT)

	clear
	cat $(REPORT)

clean: 
	rm -f *.o *.s $(X_FILE)