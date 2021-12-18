# Embed2-opdracht6

## Compileren:

    make    

## Module starten:

    sudo insmod main.ko outputs=5,12 level=1,0

## Module stoppen:

    sudo rmmod main.ko

## Lijst met modules die gestart zijn:

    lsmod

## Print output tonen:
    
    dmesg

## info van module tonen

    modinfo main.ko