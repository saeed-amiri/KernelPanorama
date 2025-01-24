obj-m += mychardev.o

all:
	$(MAKE) -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	$(MAKE) -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

clean-all:
	rm -f *.o *.ko *.mod.c *.order *.symvers *.cmd .*.cmd .tmp_versions
