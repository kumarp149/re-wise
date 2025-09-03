BUILD_ENV = local

CC = gcc

BIN_DIR := bin

ifeq ($(BUILD_ENV),local)

    LIB = -L/opt/homebrew/Cellar/openssl@3/3.5.0/lib/ -L/opt/homebrew/Cellar/libzip/1.11.4/lib/ -lzip \
	  -lssl -lcrypto
    
	INC = -I/opt/homebrew/Cellar/openssl@3/3.5.0/include/ -I/opt/homebrew/Cellar/libzip/1.11.4/include/

else ifeq ($(BUILD_ENV),ci)
    LIB = -Ldeps/openssl/lib/ -Ldeps/libzip/lib/ -lzip \
	  -lssl -lcrypto
    
	INC = -Ideps/openssl/include/ -Ideps/libzip/include/
endif

OPTIONS = -Wall -Wextra -Werror -Wconversion -Wno-error=deprecated-declarations -O1 -fno-omit-frame-pointer -fsanitize=address -O2 -MMD -MP

SRC := $(shell find src -name '*.c')
HEADER := $(shell find include -name '*.h')

OBJ := $(patsubst src/%.c, bin/%.o, $(SRC))

DEPS := $(OBJ:.o=.d)

$(BIN_DIR)/a.out: $(BIN_DIR) $(OBJ) makefile
	$(CC) -g $(OPTIONS) $(OBJ) $(LIB) -o $(BIN_DIR)/a.out

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

bin/%.o: src/%.c include/%.h
	@mkdir -p $(dir $@)
	$(CC) $(OPTIONS) -c $< $(INC) -o $@

-include $(DEPS)


# BUILD_ENV ?= local


# objects = bin/structures.o bin/track.o bin/utils.o bin/file.o bin/sha256.o bin/timer.o bin/commit.o \
# 		  bin/tree.o bin/blob.o bin/args.o bin/status.o bin/print.o bin/regex.o bin/restore.o \
# 		  bin/tag.o bin/log.o bin/fingerprint.o bin/diff.o bin/checkout.o bin/stdutils.o
# CC = clang

# BIN_DIR := bin

# ifeq ($(BUILD_ENV),local)

#     LIB = -L/opt/homebrew/Cellar/openssl@3/3.5.0/lib/ -L/opt/homebrew/Cellar/libzip/1.11.4/lib/ -lzip \
# 	  -lssl -lcrypto
    
# 	INC = -I/opt/homebrew/Cellar/openssl@3/3.5.0/include/ -I/opt/homebrew/Cellar/libzip/1.11.4/include/

# else ifeq ($(BUILD_ENV),ci)
#     LIB = -Ldeps/openssl/lib/ -Ldeps/libzip/lib/ -lzip \
# 	  -lssl -lcrypto
    
# 	INC = -Ideps/openssl/include/ -Ideps/libzip/include/
# endif


# OPTIONS = -Wall -Wextra -Werror -Wconversion -Wno-error=deprecated-declarations -O1 -fno-omit-frame-pointer -fsanitize=address

# a.out: $(BIN_DIR) $(objects) makefile
# 	$(CC) -g $(OPTIONS) $(objects) $(LIB) -o a.out

# $(BIN_DIR):
# 	mkdir -p $(BIN_DIR)

# bin/structures.o: src/structures.c include/structures.h include/print.h include/global.h
# 	$(CC) -g $(OPTIONS) -c $< $(INC) -o $@

# bin/utils.o: src/utils.c include/utils.h include/structures.h include/print.h include/global.h include/constants.h
# 	$(CC) -g $(OPTIONS) -c $< $(INC) -o $@

# bin/track.o: src/track.c include/track.h include/structures.h include/sha256.h include/timer.h include/commit.h \
# 			 include/args.h include/print.h include/global.h include/constants.h include/fingerprint.h
# 	$(CC) -g $(OPTIONS) -c $< $(INC) -o $@

# bin/file.o: src/file.c include/print.h include/track.h include/utils.h include/status.h include/regex.h include/commit.h include/restore.h \
# 			include/global.h include/constants.h include/tag.h include/log.h include/diff.h include/checkout.h
# 	$(CC) -g $(OPTIONS) -c $< $(INC) -o $@

# bin/sha256.o: src/sha256.c include/sha256.h  include/print.h include/global.h include/constants.h
# 	$(CC) -g $(OPTIONS) -c $< $(INC) -o $@

# bin/timer.o: src/timer.c include/timer.h include/print.h include/global.h include/constants.h
# 	$(CC) -g $(OPTIONS) -c $< $(INC) -o $@

# bin/commit.o: src/commit.c include/commit.h include/tree.h include/utils.h include/blob.h include/print.h \
# 			  include/args.h include/global.h include/constants.h
# 	$(CC) -g $(OPTIONS) -c $< $(INC) -o $@

# bin/tree.o: src/tree.c include/tree.h include/structures.h  \
# 			include/blob.h include/utils.h include/print.h include/global.h include/constants.h
# 	$(CC) -g $(OPTIONS) -c $< $(INC) -o $@

# bin/blob.o: src/blob.c include/blob.h  include/print.h include/commit.h include/global.h include/constants.h
# 	$(CC) -g $(OPTIONS) -c $< $(INC) -o $@

# bin/args.o: src/args.c include/args.h include/print.h include/global.h include/constants.h include/stdutils.h
# 	$(CC) -g $(OPTIONS) -c $< $(INC) -o $@

# bin/status.o: src/status.c include/status.h  include/args.h include/commit.h include/print.h include/global.h include/constants.h
# 	$(CC) -g $(OPTIONS) -c $< $(INC) -o $@

# bin/print.o: src/print.c include/print.h include/global.h include/constants.h
# 	$(CC) -g $(OPTIONS) -c $< $(INC) -o $@

# bin/regex.o: src/regex.c include/regex.h include/global.h include/constants.h
# 	$(CC) -g $(OPTIONS) -c $< $(INC) -o $@

# bin/restore.o: src/restore.c include/restore.h include/args.h include/blob.h include/print.h include/utils.h include/global.h include/constants.h
# 	$(CC) -g $(OPTIONS) -c $< $(INC) -o $@

# bin/tag.o: src/tag.c include/tag.h include/args.h include/print.h include/constants.h include/utils.h include/print.h include/commit.h
# 	$(CC) -g $(OPTIONS) -c $< $(INC) -o $@

# bin/log.o: src/log.c include/log.h include/commit.h include/args.h include/print.h include/constants.h include/utils.h
# 	$(CC) -g $(OPTIONS) -c $< $(INC) -o $@

# bin/fingerprint.o: src/fingerprint.c include/fingerprint.h include/args.h include/constants.h include/commit.h include/tag.h
# 	$(CC) -g $(OPTIONS) -c $< $(INC) -o $@

# bin/diff.o: src/diff.c include/diff.h include/args.h include/constants.h
# 	$(CC) -g $(OPTIONS) -c $< $(INC) -o $@

# bin/checkout.o: src/checkout.c include/checkout.h include/args.h include/commit.h include/tree.h include/status.h include/blob.h include/utils.h include/tag.h
# 	$(CC) -g $(OPTIONS) -c $< $(INC) -o $@

# bin/stdutils.o: src/stdutils.c include/stdutils.h
# 	$(CC) -g $(OPTIONS) -c $< $(INC) -o $@