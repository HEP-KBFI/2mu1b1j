
git pull
mkdir /home/margusp/roofits/
rm -rf /home/margusp/roofits/*


root -q -b create_roofit_plots.cpp | grep pValue > /home/margusp/roofits/out-pvalue.log
