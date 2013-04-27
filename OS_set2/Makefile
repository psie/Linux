all: 1 1+ 2 3 4 5 

1+ : 1+.c
	$(CC) 1+.c -lncurses -o 1+

2: 2.c
	$(CC) 2.c -o 2 -lrt 

3: 3.c
	$(CC) -pthread 3.c -o 3

4: 4.c
	$(CC) 4.c -o 4 -lrt

5: 5.c
	$(CC) -pthread 5.c -o 5

clean:
	rm -f 1 1+ 2 3 4 5 
