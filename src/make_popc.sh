arch_cpu=x86-64-popc
make --no-print-directory -j build ARCH=${arch_cpu} COMP=mingw
strip bbc+nnue.exe
mv bbc+nnue.exe bbc+nnue_x64_popc.exe
make clean 
