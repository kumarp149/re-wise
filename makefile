objects = bin/structures.o bin/track.o bin/utils.o bin/file.o bin/sha256.o bin/timer.o bin/commit.o \
		  bin/tree.o bin/blob.o bin/args.o
CC = gcc
LIB = -L/opt/homebrew/Cellar/openssl@3/3.4.1/lib/ -L/opt/homebrew/Cellar/libzip/1.11.3/lib/ -lzip \
	  -lssl -lcrypto
	
INC = -I/opt/homebrew/Cellar/openssl@3/3.4.1/include/ -I/opt/homebrew/Cellar/libzip/1.11.3/include/

a.out: $(objects)
	$(CC) -g $(objects) $(LIB) -o a.out


bin/structures.o: src/structures.c include/structures.h
	$(CC) -g -c $< $(INC) $(LIB) -o $@

bin/utils.o: src/utils.c include/utils.h include/constants.h include/global.h
	$(CC) -g -c $< $(INC) $(LIB) -o $@

bin/track.o: src/track.c include/track.h include/structures.h include/sha256.h include/timer.h include/commit.h \
			 include/args.h
	$(CC) -g -c $< $(INC) $(LIB) -o $@

bin/file.o: src/file.c
	$(CC) -g -c $< $(INC) $(LIB) -o $@

bin/sha256.o: src/sha256.c include/sha256.h include/constants.h
	$(CC) -g -c $< $(INC) $(LIB) -o $@

bin/timer.o: src/timer.c include/timer.h
	$(CC) -g -c $< $(INC) $(LIB) -o $@

bin/commit.o: src/commit.c include/commit.h include/tree.h include/utils.h include/blob.h
	$(CC) -g -c $< $(INC) $(LIB) -o $@

bin/tree.o: src/tree.c include/tree.h include/constants.h include/structures.h
	$(CC) -g -c $< $(INC) $(LIB) -o $@

bin/blob.o: src/blob.c include/blob.h include/constants.h
	$(CC) -g -c $< $(INC) $(LIB) -o $@

bin/args.o: src/args.c include/args.h
	$(CC) -g -c $< $(INC) $(LIB) -o $@

