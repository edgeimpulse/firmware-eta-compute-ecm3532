target remote localhost:2331
mon reset
file bin/lsm6dso.elf
b main
load bin/lsm6dso.elf
c


