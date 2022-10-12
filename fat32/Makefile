# Make the Linux version

BUILD_D = build/
INC_D = include/
SRC_D = src/

SRC = $(wildcard $(SRC_D)*.c)
NAMES = $(basename $(notdir $(SRC)))
BIN = $(addprefix $(BUILD_D), $(addsuffix .o, $(NAMES)))
TARGET = $(BUILD_D)fat32-parser

CFLAGS = -g -Wall -I$(INC_D) #-DNDEBUG

# build:

all: $(TARGET)

# $(TARGET):$(BIN)
# 	gcc $< -o $@

# $(BIN): $(SRC)

$(TARGET): $(BIN)
	gcc $^ -o $@ $(CFLAGS)

$(BUILD_D)%.o: $(SRC_D)%.c
	gcc -c $^ -o $@ $(CFLAGS)



.PHONY: clean clean-img virtual.img

clean:
	-rm ./build/*.exe ./build/*.o

clean-img:
	-rm ./virtual.img
	-rm -rf ./mnt
	-rm ./*.txt

# make a disk image

virtual.img:
	dd if=/dev/zero of=virtual.img bs=1M count=256
	du -h virtual.img
	mkfs.vfat -F 32 virtual.img
	-rm random.txt
	base64 /dev/urandom | head -c 4096 > random.txt
	-rm -rf ./mnt
	-mkdir ./mnt
	sudo mount -o loop virtual.img ./mnt
	sudo cp ./random.txt ./mnt/
	sudo umount ./mnt/
	rm -rf ./mnt/



# test:

.PHONY: easy-test
easy-test:
	make clean-img
	make virtual.img
	./build/fat32-parser -o ./out.txt -i ./virtual.img -p random.txt
	diff random.txt out.txt
# TODO


