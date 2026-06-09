write_cfgmem -format bin -size 32 -interface SPIx4 -loadbit {up 0x00000000 ./output/download.bit } -loaddata {up 0x00D00000 ./output/app.bin } -checksum -force -disablebitswap -file ./output/BOOT.bin
exit
