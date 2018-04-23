obj-m += leddrv.o
all:
	make -C /home/sice/kj_gjx_embedd_base/lab_8/linux-3.5 M=`pwd` modules
clean:
	make -C /home/sice/kj_gjx_embedd_base/lab_8/linux-3.5 M=`pwd` modules clean
