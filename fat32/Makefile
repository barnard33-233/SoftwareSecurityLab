# Make the Linux version

.PHONY: clean clean-img virtual.img
clean:
	-rm ./build/*.exe ./build/*.o ./*.txt

clean-img:
	-rm ./virtual.img
	-rm -rf ./mnt

virtual.img:
	dd if=/dev/zero of=virtual.img bs=1M count=256
	du -h virtual.img
	mkfs.vfat -F 32 virtual.img
	-rm random.txt
	make -f Makefile.linux random.txt
	-rm -rf ./mnt
	-mkdir ./mnt
	sudo mount -o loop virtual.img ./mnt
	sudo cp ./random.txt ./mnt/
	sudo umount ./mnt/
	rm -rf ./mnt/


random.txt:
	base64 /dev/urandom | head -c 4096 > random.txt
