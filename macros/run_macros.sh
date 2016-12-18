

git pull
rm -rf /home/margusp/roofits/
mkdir /home/margusp/roofits/

#root-config --cflags
#root-config --glibs

#make
#./build/app.exe | grep pValue > /home/margusp/roofits/out-pvalue.log

cd src
root -q -b app.cpp
