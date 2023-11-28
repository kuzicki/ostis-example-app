#!/bin/bash

pipe=/tmp/signalPipe
source=/media/sf_SharedFolder/Ostis/.
path=/home/user/git/fork/ostis-example-app/scripts
kbPath=/home/user/git/fork/ostis-example-app/kb
mode=$1

trap "rm -f $pipe" EXIT

if [ -z "$1" ]; then
    echo "Arguments are not supplied!"
    exit
fi

if [ ! -z "$2" ] && (( $2 == 1  ||  $2 == 3 )); then
    cp -a $source $kbPath
fi

if [[ ! -p $pipe ]]; then
    mkfifo $pipe
fi

cd $path

gnome-terminal -- bash -c "
if [[ ! -p $pipe ]]; then
    echo wrong
fi
if [ $mode == 1 ]; then
    ./build_kb.sh
    echo 1 >$pipe 
elif [ $mode == 2 ]; then
    echo 0 >$pipe 
    ./run_sc_server.sh
elif [ $mode == 3 ]; then
    ./build_kb.sh
    echo 0 >$pipe 
    ./run_sc_server.sh
else
    echo 1 >$pipe
fi
exec bash;"

while true
do
    if read line <$pipe; then
        if [[ "$line" == 0 ]]; then
            break
        fi
        if [ "$line" == 1 ]; then
            exit
        fi
    fi
done

gnome-terminal -- bash -c "sleep 1; ./run_sc_web.sh; exec bash"
sleep 2;
if [ ! -z "$2" ] && (( $2 == 2  || $2 == 3 )); then
    xdg-open http://localhost:8000
fi
