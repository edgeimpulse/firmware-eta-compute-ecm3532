sed '/#include/d' $1 > dsp_temp.h
sed -e 's/\<const\>//g' dsp_temp.h > dsp_new.h
# sed '27 a #pragma pack(1)' dsp_temp1.h > dsp_new.h
gcc -m32 -o bw2bin weights2bin.c
./bw2bin $1
rm dsp_new.h $2>/dev/null
rm dsp_temp.h $2>/dev/null
# rm dsp_temp1.h $2>/dev/null
rm -f bw2bin
