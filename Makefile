build: main.c
	$(CC) $(PACKAGE) -o 8080 main.c cpu.c 8080_dis.c $(LIBS)
