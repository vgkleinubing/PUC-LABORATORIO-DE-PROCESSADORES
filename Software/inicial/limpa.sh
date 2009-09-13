# Script para gerar os arquivos apostila2009-1.tgz (linux)
# e o apostila2009-1.zip (para windows) a partir de um unico direorio fonte em Linux
# escrito por Marcos A. Stemmer
rm *~ *.tgz
for ARM in [a-z]* ; do
if [ -d $ARM ] ; then
cd $ARM
pwd
rm -f *~ *.o *.elf *.hex *.bin *.map lixo.*
rm -f Makefile.linux Makefile.win
cd ..
fi
done
