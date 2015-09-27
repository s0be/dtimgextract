# dtimgextract
Tool to decompile a qualcomm dt.img into its split dtb files for the supported boards.

Currently only dt.img v2 tested

`Compile:
   gcc dtimgextract.c -o dtimgextract
Usage:
   ./dtimgextract dt.img`

# Example usage and output

`$ ./dtimgextract dt.img 
qc_magic: QCDT
version: 2
count: 3

Pid     Vid     Srev    Unknown Offset  len
239     8       0       0       800     32800
241     8       0       0       800     32800
263     8       0       0       800     32800

Writing 239_8_0.dtb(32800 bytes)
Writing 241_8_0.dtb(32800 bytes)
Writing 263_8_0.dtb(32800 bytes)`


# Decompiling the output
> $ dtc -I dtb -O dts 239_8_0.dtb -o 239_8_0.dts
