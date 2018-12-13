
######################################################################################
# This scripts runs all four traces
# You will need to first compile your code in ../src.BC before launching this script
# the results are stored in the ../results/ folder 
######################################################################################



########## ---------------  B.1 ---------------- ################

../src.BC/sim -pipewidth 1 -schedpolicy 0 -loadlatency 1 ../traces/bzip2.ptr.gz > ../results/B1.bzip2.res
../src.BC/sim -pipewidth 1 -schedpolicy 0 -loadlatency 1 ../traces/gcc.ptr.gz > ../results/B1.gcc.res
../src.BC/sim -pipewidth 1 -schedpolicy 0 -loadlatency 1 ../traces/libq.ptr.gz > ../results/B1.libq.res
../src.BC/sim -pipewidth 1 -schedpolicy 0 -loadlatency 1 ../traces/mcf.ptr.gz > ../results/B1.mcf.res

########## ---------------  B.2 ---------------- ################

../src.BC/sim -pipewidth 1 -schedpolicy 1 -loadlatency 1 ../traces/bzip2.ptr.gz > ../results/B2.bzip2.res
../src.BC/sim -pipewidth 1 -schedpolicy 1 -loadlatency 1 ../traces/gcc.ptr.gz > ../results/B2.gcc.res
../src.BC/sim -pipewidth 1 -schedpolicy 1 -loadlatency 1 ../traces/libq.ptr.gz > ../results/B2.libq.res
../src.BC/sim -pipewidth 1 -schedpolicy 1 -loadlatency 1 ../traces/mcf.ptr.gz > ../results/B2.mcf.res


########## ---------------  B.3 ---------------- ################

../src.BC/sim -pipewidth 1 -schedpolicy 0 -loadlatency 4 ../traces/bzip2.ptr.gz > ../results/B3.bzip2.res
../src.BC/sim -pipewidth 1 -schedpolicy 0 -loadlatency 4 ../traces/gcc.ptr.gz > ../results/B3.gcc.res
../src.BC/sim -pipewidth 1 -schedpolicy 0 -loadlatency 4 ../traces/libq.ptr.gz > ../results/B3.libq.res
../src.BC/sim -pipewidth 1 -schedpolicy 0 -loadlatency 4 ../traces/mcf.ptr.gz > ../results/B3.mcf.res

########## ---------------  B.4 ---------------- ################

../src.BC/sim -pipewidth 1 -schedpolicy 1 -loadlatency 4 ../traces/bzip2.ptr.gz > ../results/B4.bzip2.res
../src.BC/sim -pipewidth 1 -schedpolicy 1 -loadlatency 4 ../traces/gcc.ptr.gz > ../results/B4.gcc.res
../src.BC/sim -pipewidth 1 -schedpolicy 1 -loadlatency 4 ../traces/libq.ptr.gz > ../results/B4.libq.res
../src.BC/sim -pipewidth 1 -schedpolicy 1 -loadlatency 4 ../traces/mcf.ptr.gz > ../results/B4.mcf.res



########## ---------------  C.1 ---------------- ################

../src.BC/sim -pipewidth 2 -schedpolicy 0 -loadlatency 1 ../traces/bzip2.ptr.gz > ../results/C1.bzip2.res
../src.BC/sim -pipewidth 2 -schedpolicy 0 -loadlatency 1 ../traces/gcc.ptr.gz > ../results/C1.gcc.res
../src.BC/sim -pipewidth 2 -schedpolicy 0 -loadlatency 1 ../traces/libq.ptr.gz > ../results/C1.libq.res
../src.BC/sim -pipewidth 2 -schedpolicy 0 -loadlatency 1 ../traces/mcf.ptr.gz > ../results/C1.mcf.res

########## ---------------  C.2 ---------------- ################

../src.BC/sim -pipewidth 2 -schedpolicy 1 -loadlatency 1 ../traces/bzip2.ptr.gz > ../results/C2.bzip2.res
../src.BC/sim -pipewidth 2 -schedpolicy 1 -loadlatency 1 ../traces/gcc.ptr.gz > ../results/C2.gcc.res
../src.BC/sim -pipewidth 2 -schedpolicy 1 -loadlatency 1 ../traces/libq.ptr.gz > ../results/C2.libq.res
../src.BC/sim -pipewidth 2 -schedpolicy 1 -loadlatency 1 ../traces/mcf.ptr.gz > ../results/C2.mcf.res


########## ---------------  C.3 ---------------- ################

../src.BC/sim -pipewidth 2 -schedpolicy 0 -loadlatency 4 ../traces/bzip2.ptr.gz > ../results/C3.bzip2.res
../src.BC/sim -pipewidth 2 -schedpolicy 0 -loadlatency 4 ../traces/gcc.ptr.gz > ../results/C3.gcc.res
../src.BC/sim -pipewidth 2 -schedpolicy 0 -loadlatency 4 ../traces/libq.ptr.gz > ../results/C3.libq.res
../src.BC/sim -pipewidth 2 -schedpolicy 0 -loadlatency 4 ../traces/mcf.ptr.gz > ../results/C3.mcf.res

########## ---------------  C.4 ---------------- ################

../src.BC/sim -pipewidth 2 -schedpolicy 1 -loadlatency 4 ../traces/bzip2.ptr.gz > ../results/C4.bzip2.res
../src.BC/sim -pipewidth 2 -schedpolicy 1 -loadlatency 4 ../traces/gcc.ptr.gz > ../results/C4.gcc.res
../src.BC/sim -pipewidth 2 -schedpolicy 1 -loadlatency 4 ../traces/libq.ptr.gz > ../results/C4.libq.res
../src.BC/sim -pipewidth 2 -schedpolicy 1 -loadlatency 4 ../traces/mcf.ptr.gz > ../results/C4.mcf.res



########## ---------------  GenReport ---------------- ################

grep LAB3_CPI ../results/B?.*.res > report.txt
grep LAB3_CPI ../results/C?.*.res >> report.txt


######### ------- Goodbye -------- ##################

echo "Done. Check report.txt, and .res files in ../results";

