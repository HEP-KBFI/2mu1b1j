git pull
mkdir /home/margusp/roofits/
rm /home/margusp/roofits/*

root -q -b create_roofit_plots.cpp | grep pValue > out-pvalue.log
cp *.log /home/margusp/roofits/
