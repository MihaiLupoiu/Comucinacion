#!/bin/bash

inicialpth=$(pwd)

printf "\nChecking for missing packages...\n"
tmp=$(dpkg-query -W -f='${Package} ${Status} \n' libasound2-dev | grep "no package found")

if [[ $tmp =~ "no package found" ]]
then
printf "Working\n\n"
apt-get install $tmp
else
printf "Packages installed\n"
fi

printf "\nChecking for libconfig.h packages...\n"
FILE="/usr/local/include/libconfig.h"
if [ -f $FILE ];
then
   echo "File $FILE exists"
   echo "Libconfig package installed"

else
   echo "File $FILE does not exists"
   printf "Working\n\n"
   tar -xvf ./../../bin/configlib/libconfig-1.5.tar.gz -C ./../lib/
   cd ./../lib/libconfig-1.5/
   $(sudo ./configure && sudo make && sudo make check && sudo make install && sudo ldconfig -v)
fi

printf "Location:\n"
cd ${inicialpth}
cd ..
route=$(pwd)
#echo $route

printf "\nCompilating SuperWavServer...\n"
sudo gcc $route/src/SuperWavAppServer.c $route/lib/funcions.c $route/lib/connection/socketUtils.c $route/lib/connection/server.c -o SuperWavAppServer -lasound -lm

printf "\nClient:\n"
cd ./../client/
route=$(pwd)
#echo $route

printf "\nCompilating SuperWavClient...\n"
sudo gcc -Wall -Wextra $route/src/SuperWavAppClient.c $route/lib/funcionslib.c $route/lib/configuration.c $route/lib/connection/socketUtils.c $route/lib/connection/client.c $route/lib/sound/spatiallib.c $route/lib/sound/processSoundlib.c $route/lib/sound/superwavlib.c -o SuperWavAppClient -lasound -lm -lpthread -lconfig
