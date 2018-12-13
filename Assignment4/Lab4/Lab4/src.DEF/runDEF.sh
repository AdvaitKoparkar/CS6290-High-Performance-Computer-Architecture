
######################################################################################
# This scripts runs all three traces
# You will need to uncomment the configurations that you want to run
# the results are stored in the ../results/ folder
######################################################################################



# ########## ---------------  D ---------------- ################
#
#
# ./sim -mode 4 ../traces/bzip2.mtr.gz ../traces/libq.mtr.gz > ../results/D.mix1.res
# ./sim -mode 4 ../traces/bzip2.mtr.gz ../traces/lbm.mtr.gz  > ../results/D.mix2.res
# ./sim -mode 4 ../traces/lbm.mtr.gz ../traces/libq.mtr.gz   > ../results/D.mix3.res
#
# ########## ---------------  E (Same as D, except L2rep=2l) -------------- ################
#
# ./sim -mode 5 -L2repl 2 -SWP_core0ways 4 ../traces/bzip2.mtr.gz ../traces/libq.mtr.gz  > ../results/E.Q1.mix1.res
# ./sim -mode 5 -L2repl 2 -SWP_core0ways 8 ../traces/bzip2.mtr.gz ../traces/libq.mtr.gz  > ../results/E.Q2.mix1.res
# ./sim -mode 5 -L2repl 2 -SWP_core0ways 12 ../traces/bzip2.mtr.gz ../traces/libq.mtr.gz > ../results/E.Q3.mix1.res
#
# ./sim -mode 5 -L2repl 2 -SWP_core0ways 4 ../traces/bzip2.mtr.gz ../traces/lbm.mtr.gz  > ../results/E.Q1.mix2.res
# ./sim -mode 5 -L2repl 2 -SWP_core0ways 8 ../traces/bzip2.mtr.gz ../traces/lbm.mtr.gz  > ../results/E.Q2.mix2.res
# ./sim -mode 5 -L2repl 2 -SWP_core0ways 12 ../traces/bzip2.mtr.gz ../traces/lbm.mtr.gz > ../results/E.Q3.mix2.res
#
# ./sim -mode 5 -L2repl 2 -SWP_core0ways 4 ../traces/lbm.mtr.gz ../traces/libq.mtr.gz  > ../results/E.Q1.mix3.res
# ./sim -mode 5 -L2repl 2 -SWP_core0ways 8 ../traces/lbm.mtr.gz ../traces/libq.mtr.gz  > ../results/E.Q2.mix3.res
# ./sim -mode 5 -L2repl 2 -SWP_core0ways 12 ../traces/lbm.mtr.gz ../traces/libq.mtr.gz > ../results/E.Q3.mix3.res


########## ---------------  F (Same as D, except L2repl=3) -------------- ################


./sim -mode 6 -L2repl 3  ../traces/bzip2.mtr.gz ../traces/libq.mtr.gz  > ../results/F.mix1.res
./sim -mode 6 -L2repl 3  ../traces/bzip2.mtr.gz ../traces/lbm.mtr.gz  > ../results/F.mix2.res
./sim -mode 6 -L2repl 3  ../traces/lbm.mtr.gz ../traces/libq.mtr.gz  > ../results/F.mix3.res



echo "All Done. Check the .res file in ../results directory";
