# Script para gerar os arquivos apostila22378.tgz (linux)
# e o apostila2378.zip (para windows) a partir de um unico direorio fonte em Linux
# escrito por Marcos A. Stemmer
# Versao 2009-2
rm *~ *.tgz
for ARM in [a-z]* ; do
if [ -d $ARM ] ; then
cd $ARM
pwd
rm -f *~ *.o *.elf *.hex *.bin *.map lixo.*
rm -f Makefile.linux Makefile.win
sed -i {s/"SERIALDEV=\/dev\/ttyS0"/"SERIALDEV = \/dev\/ttyS0"/} Makefile
sed -i {s/"SERIALDEV = \/dev\/ttyS0"/"SERIALDEV = com1"/} Makefile
sed -i {s/"ltser"/"wtser"/} Makefile
cd ..
fi
done
cd ..
rm apostila2378.zip
zip -r apostila2378.zip apostila2378
cd -
for ARM in [a-z]* ; do
if [ -d $ARM ] ; then
cd $ARM
sed -i {s/"SERIALDEV = com1"/"SERIALDEV = \/dev\/ttyS0"/} Makefile
sed -i {s/"wtser"/"ltser"/} Makefile
cd ..
fi
done
cd ..
tar czf apostila2378.tgz apostila2378
cd apostila2378
for ARM in [a-z]* ; do
if [ -d $ARM ] ; then
tar czf $ARM.tgz $ARM
fi
done
cd -
