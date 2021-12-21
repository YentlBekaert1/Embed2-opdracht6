# Embed2-opdracht6
## Basis
a)  Een kernelmodule om enkele outputs aan te sturen aan de hand van module parameters.
b)  Extra parameter om de togglesnelheid van een IO aan te geven - waarde 0 => niet togglen.

### Compileren:

    make    

### Module starten:

    sudo insmod main.ko outputs=5,12 level=1,0 togglespeed=0,5

### Module stoppen:

    sudo rmmod main.ko

### Lijst met modules die gestart zijn:

    lsmod

### Print output tonen:
    
    dmesg

### Info van module tonen

    modinfo main.ko

## Uitbreiding

### Module Compileren:

    make    

### Module starten:

    sudo insmod query_ioctl.ko

### Module stoppen:

    sudo rmmod query_ioctl

### App compileren:

    gcc -o  query_app.o query_app.c

### App starten:

    sudo ./query_app.o

./query_app.o       — to display the driver variables
./query_app.o -c    — to clear the driver variables
./query_app.o -g    — to display the driver variables
./query_app.o -s    — to set the driver variables (not mentioned above)
