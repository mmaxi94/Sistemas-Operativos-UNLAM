#!/bin/bash

#################  ENCABEZADO  ##################
#												#
#	Nombre del script: ejercicio4.sh			#
#	Numero de APL: 1							#
# 	Numero de ejercicio: 4						#
#												#
#	Integrantes:								#
#  		Rodriguez, Cesar Daniel		39166725	#
# 		Bastante, Javier 			38621360	#
#  		Garcia Velez, Kevin 		38619312	#
# 		Morales, Maximiliano 		38176604	#
#		Baranda, Leonardo 			36875068	#
#												#
#################################################

function ayuda(){
	echo ""
    echo "Este script cuenta la cantidad de líneas de código y de comentarios que poseen los archivos en una ruta pasada por parámetro y controlando solo los archivos con cierta extensión, en forma recursiva en todos los subdirectorios"
    echo
    echo "Uso: ./ejercicio4.sh [--ruta] [--ext]"
    echo
    echo "--ruta: la ruta a analizar"
    echo "--ext: listado de extensiones de los archivos de código a analizar, separadas por coma"
    echo
	echo "Ejemplo de ejecución:"
	echo
	echo "./ejercicio4.sh --ruta /home/usuario/proyecto1 --ext java,c"
	echo "NOTA: rutas con espacios entre si, escribir el path entre comillas dobles"
	echo ""
	echo "Para AYUDA, ejecute -h, --help o -?"
}

function resolver(){

IFS=','
cantTotalArchivos=0
cantLineasCodTotales=0
acumuladorArchivos=0
acumuladorLineasCodigoTotales=0
acumuladorTotalLineas=0
acumuladorComentariosTotales=0

for palabra in $extensiones
	do
	
	shopt -s globstar
	cantArch=0

	for arch in "${directorio}"**/*."$palabra"; do
	cantArch=0
		

		if [ -f "$arch" ] ; then
		
			flag_no_existe_extension=0
			((cantArch++))
			lineas=0
			comentarios=0
			codigoYcomentario='^[[:blank:]]*[^[:blank:]]+.*\/\/.*'
			soloComentario='^[[:blank:]]*\/\/.*'
			codigoYmultilineaStart='^[[:blank:]]*[^[:blank:]]+.*\/\*.*'
			multilineaStart='^[[:blank:]]*\/\*.*'
			#multilineaEnd='.*(\*\/)$'
			multiline=false
			total=0

			echo "Analizando archivo: $arch"

			while IFS= read -r line;
			do

				if  [ $multiline = false ]; then				
					# Cuenta línea de código mas comentario
					# ej: codigo // comentario
					if [[ $line =~ $codigoYcomentario ]]; then
						((lineas++))
						((comentarios++))
					# Cuenta comentarios de una línea. ej.: //comentario
					elif [[ $line =~ $soloComentario ]]; then
						((comentarios++))
					# Si encontramos el inicio de un comentario multilinea, activamos el flag
					elif [[ $line =~ $multilineaStart ]]; then
						multiline=true
						((comentarios++))
						#Si encuentra en la misma linea el cierre del comentario multilinea, desactivamos flag
						if [[ $(echo "$line" | grep "*/" | wc -l) -eq 1 ]]; then
							multiline=false
						fi
					# Si encontramos linea de codigo mas comienzo de comentario multilinea, activamos el flag
					# ej.: codigo /*Comentario multilinea
					elif [[ $line =~ $codigoYmultilineaStart ]]; then
						multiline=true
						((lineas++))
						((comentarios++))
						# Si encuentra en la misma linea el cierre del comentario multilinea, desactivamos flag
						if [[ $(echo "$line" | grep "*/" | wc -l) -eq 1 ]]; then
							multiline=false
						fi
					else
						# Si no cumple ninguna de las condiciones, la consideramos linea de codigo
						((lineas++))
					fi
	
				else
					# Nos fijamos si encontramos el fin de un commentario multilinea
					if [[ $(echo "$line" | grep "*/" | wc -l) -eq 1 ]]; then
						multiline=false
					fi
					((comentarios++))
				fi
			done < <(grep "" "$arch")

			total=$(cat $arch | wc -l)
			
			if ! [[ $total -eq 0 ]]; then
				((total++))
			fi
			
		acumuladorLineasCodigoTotales=$((acumuladorLineasCodigoTotales+lineas))
		acumuladorComentariosTotales=$((acumuladorComentariosTotales+comentarios))
		acumuladorTotalLineas=$((acumuladorTotalLineas+total))
		
		acumuladorArchivos=$((acumuladorArchivos+cantArch))
			
		
		else
			flag_no_existe_extension=1
			echo "extension '$palabra' inexistente"
		fi
	done	

	# Desactivamos globstar
	shopt -u globstar	
	
	done
	
	if [[ $acumuladorTotalLineas != 0 ]]; then
		porcentajeLineas=`bc -l <<< "scale=2; ($acumuladorLineasCodigoTotales/$acumuladorTotalLineas)*100"`
		porcentajeComm=`bc -l <<< "scale=2; ($acumuladorComentariosTotales/$acumuladorTotalLineas)*100"`

	fi
	
	echo ""		
	echo "Total de archivos analizados: $acumuladorArchivos"
	echo ""
	echo "Cantidad de lineas de codigo totales: $acumuladorLineasCodigoTotales | Porcentaje: $porcentajeLineas"
	echo "Cantidad de lineas de comentario totales: $acumuladorComentariosTotales | Porcentaje: $porcentajeComm"
	echo ""

}

validarParametros(){

	if [[ $1 != "" ]]
	then
		if [ -e "$1" ]
		then
			if [ -d "$1" ]
			then
				if [ ! -r "$1" ]
				then
					echo "$1 no tiene permisos de lectura"
					echo "$0 -h, --help o -? para ver el menu de ayuda"
					exit 1
				fi
			else
				echo "$1 no es un directorio"
				echo "$0 -h, --help o -? para ver el menu de ayuda"
				exit
			fi
		else
			echo "El directorio $1 no existe"
			echo "$0 -h, --help o -? para ver el menu de ayuda"
			exit 1
		fi
	else
		echo "El valor de --ruta es obligatorio"
		echo "$0 -h, --help o -? para ver el menu de ayuda"
		exit 1
	fi
	
	if [[ $2 == "" ]]
	then
		echo "El valor de --ext es obligatorio"
		echo "$0 -h, --help o -? para ver el menu de ayuda"
		exit 1
	fi
	
}	



options=$(getopt -o "h?" -l "help,ruta:,ext:" -n "argumentos" -a -- '' "$@")

if [[ $? == 1 && "$1" != "-?" ]];
then
    echo "Ha habido un error al parsear los argumentos"
    exit 1
fi

if [[ "$1" != "-?" ]]
then
	eval set -- "$options";
fi

while [[ $# > 0 ]]
do
        case $1 in

                --ruta)
						directorio=$2
                        shift
                        ;;

                --ext)		
                        extensiones=$2
                        shift
                        ;;

                -h|--help|-\?)
                        ayuda
                        exit 0
                        ;;
						
					--) 
						shift
						break
						;;
					*)
						echo "$1 no es una opcion"
						echo "$0 -h, --help o -? para ver el menu de ayuda"
						exit 1
						;;
		
        esac
        shift
done


validarParametros "$directorio" "$extensiones"
resolver "$directorio" "$extensiones"