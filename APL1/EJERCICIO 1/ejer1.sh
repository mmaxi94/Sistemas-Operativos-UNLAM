#!/bin/bash
ErrorS() #funcion con mensajes de error que se muestra en caso de que la cantidad de parametros ingresados sea menor a 2, o el archivo no sea regular o el segundo parametro no sea L,C o M
{
echo "Error. La sintaxis del script es la siguiente:"
echo "cantidad saltos de linea del archivo: $0 nombre_archivo L" # muestra por pantalla la sintaxis a ejecutar para obtener la cantidad de saltos de linea del archivo
echo "cantidad de caracteres del archivo: $0 nombre_archivo C" # muestra por pantalla la sintaxis a ejecutar para obtener la cantidad de caracteres del archivo
echo "longitud de caracteres de la linea mas larga del archivo: $0 nombre_archivo M" # muestra por pantalla la sintaxis a ejecutar para obtener la longitud de caracteres de la linea mas larga del archivo
}
ErrorP()
{
echo "Error. nombre_archivo no existe o no tiene permisos de lectura" # muestra el sig mensaje por pantalla si el archivo no existe o si no tiene permisos de lectura
}
if test $# -lt 2; then
ErrorS
elif test ! -r $1; then
ErrorP
elif test -f $1 && (test $2 = "L" || test $2 = "C" || test $2 = "M"); then #si el archivo existe y el segundo parametro ingresado es L,C o M obtiene los calculos correspondientes.
	if test $2 = "L"; then
	res=`wc -l $1`
	echo "cantidad de saltos de linea del archivo: $res" # si el segundo parametro=L, muestra por pantalla la cantidad de saltos de linea del archivo
	elif test $2 = "C"; then
	res=`wc -m $1`
	echo "cantidad de caracteres contenidos dentro del archivo: $res" # si el segundo parametro=C,muestra por pantalla la cantidad de caracteres contenidos dentro del archivo
	elif test $2 = "M"; then
	res=`wc -L $1`
	echo "longitud de caracteres de la linea mas larga dentro del archivo: $res" # si el segundo parametro=M,muestra por pantalla la longitud de caracteres de la linea mas larga dentro del archivo
	fi
else
ErrorS;
fi


#1. ¿Cuál es el objetivo de este script?
#El objetivo del script es contar para un archivo la cantidad de saltos de lineas, caracteres y longitud de la linea mas larga.

#2. ¿Qué parámetros recibe?
#recibe 2 parametros. el primer parametro es la ruta/nombre de un archivo de texto, y el segundo parametro es el tipo de operacion de conteo a realizar (L,C o M)

#3. Comentar el código según la funcionalidad (no describa los comandos, indique la lógica)
#completado en el codigo

#4. Completar los “echo” con el mensaje correspondiente.
#completado en el codigo

#5. ¿Qué información brinda la variable “$#”? ¿Qué otras variables similares conocen? Explíquelas.
#la variable $# cuenta la cantidad de parametros pasados por linea de comando al archivo ejecutable. Otras variables especiales son el $0 (muestra el nombre del archivo ejecutable), $@ (lista todos los parametros separados entre si), $* (muestra la linea de llamada completa al ejecutable), $? (devuelve el ultimo valor retornado por el ultimo comando ejecutado), $$ (muestra el process id del shell interprete del script)

#6. Explique las diferencias entre los distintos tipos de comillas que se pueden utilizar en Shell scripts.
#Comillas simples verticales '': texto fuerte. En este string el Shell no realiza reemplazos de variables.
#Comillas dobles "" : texto débil. En este string el Shell realiza reemplazos de variables.
#Comillas francesas ` : ejecución de comandos.  En este string el Shell primero realiza reemplazos de variables y luego ejecuta el comando, guardando la salida en la variable.
