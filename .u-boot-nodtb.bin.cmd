cmd_u-boot-nodtb.bin := arm-himix100-linux-objcopy --gap-fill=0xff  -j .text -j .secure_text -j .image -j .secure_data -j .rodata -j .hash -j .data -j .got -j .got.plt -j .u_boot_list -j .rel.dyn -O binary  u-boot u-boot-nodtb.bin
