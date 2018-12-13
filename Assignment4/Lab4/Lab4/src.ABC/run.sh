
######################################################################################
# This scripts runs all three traces
# You will need to uncomment the configurations that you want to run
# the results are stored in the ../results/ folder
######################################################################################



########## ---------------  A.1 ---------------- ################

./sim -mode 1 ../traces/bzip2.mtr.gz > ../results/A1.bzip2.res
./sim -mode 1 ../traces/lbm.mtr.gz  > ../results/A1.lbm.res
./sim -mode 1 ../traces/libq.mtr.gz   > ../results/A1.libq.res

########### ---------------  B ---------------- ################

./sim -mode 2 -L2sizeKB 1024 ../traces/bzip2.mtr.gz  > ../results/B.S1MB.bzip2.res
./sim -mode 2 -L2sizeKB 1024 ../traces/lbm.mtr.gz    > ../results/B.S1MB.lbm.res &
./sim -mode 2 -L2sizeKB 1024 ../traces/libq.mtr.gz    > ../results/B.S1MB.libq.res

########### ---------------  C ---------------- ################

./sim -mode 3 -L2sizeKB 1024 ../traces/bzip2.mtr.gz  > ../results/C.S1MB.bzip2.res
./sim -mode 3 -L2sizeKB 1024 ../traces/lbm.mtr.gz    > ../results/C.S1MB.lbm.res &
./sim -mode 3 -L2sizeKB 1024 ../traces/libq.mtr.gz    > ../results/C.S1MB.libq.res


echo "All Done. Check the .res file in ../results directory";
