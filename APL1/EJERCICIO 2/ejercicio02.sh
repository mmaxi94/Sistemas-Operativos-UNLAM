#!/bin/bash

################################################################################
##  Ejercicio nro 2 del APL 1 - 2c 2022 - Entrega nro 4
##  Script: Ejercicio02.sh
##
##  Integrantes del grupo
##  Rodriguez,Cesar Daniel, 39166725
##  Bastante, Javier, 38621360
##  Garcia Velez, Kevin, 38619312
##  Morales ,Maximiliano, 38176604
##  Baranda Leonardo, 36875068
################################################################################

#PARAMETROS RECIBIDOS PASADOS A VARIABLES
PARAM="$1"
DIR="$2"

#VALIDACIONES
if [[ $# == 1 ]]; then #SI LA CANTIDAD DE PARAMETROS ES IGUAL A 1

	if [[ "$PARAM" == "-h" ]] || [[ "$PARAM" == "-?" ]] || [[ "$PARAM" == "--help" ]]; then
		echo "

Funcionamiento Script: Dado 1 directorio que contiene 1 archivo logs donde se guardan los registros de llamadas de 1 semana, el script procesa los datos y muestra
algunas estadisticas solicitadas.

Formato de invocacion: ./ejercicio2.sh [--logs]

Parametros:
	--logs:	directorio en el que se encuentran los archivos de log

Ejemplo de invocacion: ./ejercicio2.sh --logs Directorio

NOTA: Si el nombre del directorio tiene espacios entre si, escribir el path entre comillas dobles

Para AYUDA, ejecute -h, --help o -?
"
		exit "1"
	elif [[ "$PARAM" == "--logs" ]]; then
		echo "El parametro \"--logs\" debe ir acompañado del path de un Directorio, ejecute el menu AYUDA a traves de los parametros -h, -? o --help"
		exit 1
	else
		echo "Parametro \"$PARAM\" inválido, ejecute el menu AYUDA a traves de los parametros -h, -? o --help"
		exit 1
	fi
fi

if [[ $# == 2 ]]; then

	if [[ "$PARAM" == "--logs" ]]; then

		if [ ! -d "$DIR" ]; then # SI EL ARCHIVO Q MANDASTE NO ES UN ARCHIVO
			echo 'ERROR: Ruta inválida o directorio inexistente'
			exit 1
		fi

		if [ ! -r "$DIR" ]; then #SI EL ARCHIVO QUE MANDASTE NO TIENE PERMISOS
			echo 'ERROR: El directorio no tiene permisos de lectura'
			exit 1
		fi

		if [ ! -w "$DIR" ]; then
			echo "ERROR: El directorio no tiene permisos de escritura"
			exit 1
		fi

		if [ -z "$(ls -A "$DIR")" ]; then #SI MANDASTE UN ARCHIVO VACIO
			echo "ERROR: El directorio especificado se encuentra vacio, no tiene archivos dentro para analizar"
			exit 1
		fi

		if [ $(ls "$DIR" | wc -l) -ne 1 ]; then #SI EL DIRECTORIO TIENE MAS DE 1 ARCHIVO
			echo "ERROR: El directorio especificado debe contener 1 solo archivo donde estaran los registros de toda la semana. No puede haber mas de un archivo."
			exit 1
		fi

	else
		echo "parametro \'$PARAM\' inválido, ejecute el menu AYUDA a traves de los parametros -h, -? o --help"
		exit 1
	fi
else
	echo "cantidad de parametros '$#' inválida, ejecute el menu AYUDA a traves de los parametros -h, -? o --help"
	echo "NOTA: Si el nombre del directorio tiene espacios entre si, escribir el path entre comillas dobles"
	exit 1
fi

#FIN VALIDACIONES-------------------------------------------------------------------


formato_hora() { #CONVIERTE LOS SEGUNDOS ENVIADOS POR PARAMETROS AL FORMATO DE HH:MM:SS CONVENCIONAL

	hs=$(($1 / 3600))
	ms=$(($(($1 - $hs * 3600)) / 60))
	ss=$(($1 - $hs * 3600 - $ms * 60))

	if [[ $hs -le 9 ]]; then
		hs=0$hs
	fi

	if [[ $ms -le 9 ]]; then
		ms=0$ms
	fi

	if [[ $ss -le 9 ]]; then
		ss=0$ss
	fi

	echo "$hs:$ms:$ss"

}
declare -A tiene_llamada
declare -A duracion_aux

declare -A duracion_total
declare -A cantidad_llamadas

declare -A duracion_por_dia
declare -A cant_x_dia
declare -A duracion_por_usuario_por_dia
declare -A cant_x_usuario_x_dia

declare -A duracion_por_llamada_por_dia
declare -A llamada_bajo_media_por_usuario
primer_dia=true

files=$(ls "$DIR")

if ! [[ "$files" =~ ^.+\.txt$ ]]; then #si no termina en .txt
			if ! [[ "$files" =~ ^.+\.log$ ]]; then
				echo "ERROR: El archivo del directorio debe tener la extension .txt o .log, revise la extension"
				exit 1
			fi
		fi

while IFS= read -r line; do #COMIENZO A PROCESAR EL ARCHIVO
	#lee una linea del archivo
	usuario="${line:20:${#line}}" #Como el nombre del usuario siempre viene a partir del caracter 20 lo recorto
	dia="${line:0:10}"            #Como el dia siempre viene a partir del comienzo hasta el caracter 10 lo recorto
	#si el usuario ya habia iniciado una llamada
	if [[ -n ${tiene_llamada[$usuario]} && ${tiene_llamada[$usuario]} == true ]]; then
		#termina la llamada
		tiene_llamada[$usuario]=false
		duracion_aux[$usuario]=$((-${duracion_aux[$usuario]} + ${line:11:2} * 3600 + ${line:14:2} * 60 + ${line:17:2}))
		duracion_total[$usuario]=$((${duracion_total[$usuario]} + ${duracion_aux[$usuario]}))
		((cantidad_llamadas[$usuario]++))
		duracion_por_dia[$dia]=$((${duracion_por_dia[$dia]} + ${duracion_total[$usuario]}))
		((cant_x_dia[$dia]++))
		duracion_por_usuario_por_dia[$usuario$dia]=$((${duracion_por_usuario_por_dia[$dia]} + ${duracion_total[$usuario]}))
		((cant_x_usuario_x_dia[$usuario$dia]++))
		duracion_por_llamada_por_dia[$usuario$dia${cant_x_usuario_x_dia[$usuario$dia]}]=$duracion_aux
	else
		#empieza la llamada
		tiene_llamada[$usuario]=true
		duracion_aux[$usuario]=$((${line:11:2} * 3600 + ${line:14:2} * 60 + ${line:17:2}))

	fi

	if [[ primer_dia==true || $dia_ant!=$dia ]]; then
		primer_dia=false
		dia_ant=$dia
		for usuario in ${!duracion_total[*]}; do
			duracion_total[$usuario]=0
			#con esto reseteo el total por dia
		done
	fi

done <"$DIR"/"$files"

#FIN DE PROCESAMIENTO DEL ARCHIVO
echo "___________________________________________
1)Promedio de tiempo de las llamadas realizadas por día:
"
declare -A media_tiempo_por_dia
#respuesta 1
for d in ${!duracion_por_dia[*]}; do
	tiempo_en_ss=$((${duracion_por_dia[$d]} / ${cant_x_dia[$d]}))
	media_tiempo_por_dia[$d]=$tiempo_en_ss
	echo "$d - promedio: $(formato_hora $tiempo_en_ss)"
done
echo "___________________________________________
2)Promedio de tiempo y cantidad por usuario por día:
"
for u in ${!duracion_total[*]}; do #obtengo la lista de usuarios
	echo "$u"
	for d in ${!duracion_por_dia[*]}; do #obtengo la lista de dias
		if [[ -n ${duracion_por_usuario_por_dia[$u$d]} ]]; then
			tiempo_en_ss=$((${duracion_por_usuario_por_dia[$u$d]} / ${cant_x_usuario_x_dia[$u$d]}))
			echo "--$d:		
	Cantidad de llamadas: ${cant_x_usuario_x_dia[$u$d]}
	Promedio de tiempo: $(formato_hora $tiempo_en_ss)"
		fi
	done
done
echo "___________________________________________
3) Los 3 usuarios con más llamadas en la semana:
"
n=${#duracion_total[*]}
i=0
primero=0
segundo=0
tercero=0

for u in ${!duracion_total[*]}; do
	i=$(($i + 1))
	if [[ $i -eq 1 ]]; then
		primero=$u
		continue
	fi
	if [[ $i -eq 2 ]]; then
		segundo=$u
		continue
	fi
	if [[ $i -eq 3 ]]; then
		tercero=$u
		break
	fi
done

echo ${!duracion_total[*]}

for u in ${!duracion_total[*]}; do

	if [[ ${cantidad_llamadas[$u]} -ge ${cantidad_llamadas[$primero]} ]]; then
		tercero=$segundo
		segundo=$primero
		primero=$u
	elif [[ ${cantidad_llamadas[$u]} -ge ${cantidad_llamadas[$segundo]} ]]; then
		tercero=$segundo
		segundo=$u
	elif [[ ${cantidad_llamadas[$u]} -ge ${cantidad_llamadas[$tercero]} ]]; then
		tercero=$u
	fi
done

echo "1: $primero, con ${cantidad_llamadas[$primero]} llamadas
2: $segundo, con ${cantidad_llamadas[$segundo]} llamadas
3: $tercero, con ${cantidad_llamadas[$tercero]} llamadas"
echo "___________________________________________
4) Cuántas llamadas no superan la media de tiempo por día:
"
cantidad_bajo_media=0
for u in ${!duracion_total[*]}; do    #obtengo la lista de usuarios
	for d in ${!duracion_por_dia[*]}; do #obtengo la lista de dias
		n=${cant_x_usuario_x_dia[$usuario$dia]}
		for ((i = 1; i <= n; i++)); do
			if [[ ${duracion_por_llamada_por_dia[$u$d$i]} -le ${media_tiempo_por_dia[$d]} ]]; then
				cantidad_bajo_media=$(($cantidad_bajo_media + 1))
				((llamada_bajo_media_por_usuario[$u]++))
			fi
		done

	done
done

echo $cantidad_bajo_media llamadas bajo la media
echo "___________________________________________
5) El usuario que tiene más cantidad de llamadas por debajo de la media en la 
semana:
"
mayor=0
for u in ${!duracion_total[*]}; do #obtengo la lista de usuarios
	if [[ ${llamada_bajo_media_por_usuario[$u]} -ge $mayor ]]; then
		mayor=${llamada_bajo_media_por_usuario[$u]}
		menor_usuario=$u
	fi
done
echo "$menor_usuario es el que tiene mas llamadas bajo la media, con $mayor llamadas"
echo "______________________________________________"
