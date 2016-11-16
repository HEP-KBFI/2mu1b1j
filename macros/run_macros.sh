mkdir /home/margusp/roofits/
rm /home/margusp/roofits/*

root -q -b create_roofit_plots.cpp > out.log
cat out.log | grep pValue > out-pvalue.log
cp *.log /home/margusp/roofits/
