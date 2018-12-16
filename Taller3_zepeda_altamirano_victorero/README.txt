Como compilar
--------------
Utilizando MinGW en su PATH de sistema, ubiquese en el directorio donde se encuentran los archivos de codigo fuente (con extension .cpp) y compilelos de la siguiente manera:
g++ main.cpp -std=c++11 -static-libgcc -static -lpthread -o lectoresEscritores.exe (Para el ejecutable puede escoger otro nombre o direccion si es que lo desea. Agregue los comandos "-static" si desea utilizar esta aplicacion en sistemas que no tengan estas librerias).

Como utilizar
-------------
Este programa simula el problema de lectores y escritores con pioridad. los hilos se crearan segun los datos proporcionados por el archivo "entrada.txt" estructurado de la siguiente forma:

numero de hilos
tipo de hilo/ id / tiempo en el recurso compartido
.
.
.

Si es que este programa es abierto mediante un doble click al ejecutable, este buscara "entrada.txt" en el directorio en el que se encuentra, en cambio si que es ejecutado por consola (con el comando "." y la direccion del ejecutable), buscara "entrada.txt" en el directorio donde estaba la consola antes de ejecutar el programa.

Al inicio del programa ingrese 'Y' si desea cambiar el nombre del archivo de salida o 'N' si desea utilizar en nombre por defecto "salida.txt". Si escoge cambiar de nombre, ingrese el nuevo nombre del archivo sin extension. Al igual que el archivo de entrada, el archivo de salida se creara en el directiorio donde esta el programa si se ejecuta por doble-click, o en el directorio de la consola si se ejecuta mediante esta.

Requerimientos
---------------
MinGW (Para compilar la aplicacion).
Sistema operativo Windows (No se garantiza funcionamiento en versiones obsoletas de Windows Ej: Windows 95).