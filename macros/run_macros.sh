
git pull
rm -rf /home/margusp/roofits/
mkdir /home/margusp/roofits/

root -q -b create_roofit_plots.cpp | grep pValue > /home/margusp/roofits/out-pvalue.log
