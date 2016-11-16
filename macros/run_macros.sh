mkdir /home/margusp/roofits/
rm /home/margusp/roofits/*

root -q -b create_roofit_plots.cpp > out.log
cp out.log /home/margusp/roofits/
