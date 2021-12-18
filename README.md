# Embed2-opdracht6

a)  Een kernelmodule om enkele outputs aan te sturen aan de hand van module parameters.
b)  Extra parameter om de togglesnelheid van een IO aan te geven - waarde 0 => niet togglen.

## Compileren:

    make    

## Module starten:

    sudo insmod main.ko outputs=5,12 level=1,0 togglespeed=0,5

## Module stoppen:

    sudo rmmod main.ko

## Lijst met modules die gestart zijn:

    lsmod

## Print output tonen:
    
    dmesg

## Info van module tonen

    modinfo main.ko