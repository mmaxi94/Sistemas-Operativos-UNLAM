#!/bin/bash

##############################################################
##  Ejercicio nro 5 del APL 1 - 2C-2022 - Re-entrega 2
##  Script: Ejercicio05.sh
##
##  Integrantes del grupo
##  Baranda, Leonardo, 36875068
##  Morales, Maximiliano, 38176604
##  Garcia Velez Kevin, 38619312
##  Bastante, Javier, 38621360
##  Rodriguez, Cesar Daniel, 39166725
##############################################################

#Funcion de Ayuda
ayuda() {
    echo "**************************************************"
    echo "   Este Script analiza las notas de los alumnos   "
    echo "     para poder obtener estadisticas generales    "
    echo "     de aprobación y deserción de su alumnado.    "
    echo "          "
    echo "Parametros:           "
    echo "          "
    echo "--notas \"archivo\": ruta del archivo a procesar. "
    echo "Puede ser una ruta relativa o absoluta.       "
    echo "          "
    echo "--materias \"archivo\": ruta del archivo con los  "
    echo "     los datos de las materias.       "
    echo "Puede ser una ruta relativa o absoluta.       "
    echo "          "
    echo "-h | --help | -?: consultar la ayuda      "
    echo "          "
    echo "**************************************************"
}

generarReporte () {
awk 'BEGIN {
    FS="|"
    OFS=" "
}
{
    if(NR > 1 && (FNR == NR)) {
        idMateria = $2
        parcial1 = $3
        parcial2 = $4
        rec = $5
        final = $6

        materias[idMateria] = idMateria
        recursados[idMateria] += 0
        promocionados[idMateria] += 0
        abandonos[idMateria] += 0
        finales[idMateria] += 0

        if((parcial1 >= 7 && parcial2 >= 7) || (rec >= 7 && ((parcial1 >= 7 && parcial2 < 7) || (parcial1 <= 7 && parcial2 < 7)))) {
            promocionados[idMateria] += 1
        } else if((parcial1 == "" || parcial2 == "") && rec == "") {
            abandonos[idMateria] += 1
        } else if((parcial1 < 4 && parcial2 < 4) || ((parcial1 < 4 || parcial2 < 4) && (rec < 4 || rec == ""))) {
            recursados[idMateria] += 1
        } else if(final == "" || final < 4){
            finales[idMateria] += 1
        }
    } else if (FNR < NR && FNR > 1) {
        if(materias[$1]) {
            idMateriaDepto = $1
            descripcionMateria = $2
            idDepartamento = $3
            descripcionMaterias[idMateriaDepto] = descripcionMateria
            materiasPorDepartamento[idDepartamento][idMateriaDepto] = idMateriaDepto
        }
    }
}
END {
    print "{"
    print "\t\"departamentos\": ["
    nroDeptoArray = 1
    for(i in materiasPorDepartamento) {
        print "\t\t{"
        print "\t\t\t\"id\": "i","
        print "\t\t\t\"notas\": ["
        nroMateriaArray = 1
        for (id in materiasPorDepartamento[i]) {
            print"\t\t\t\t{"
            print"\t\t\t\t\t\"id_materia\": "id","
            print"\t\t\t\t\t\"descripcion\": " "\""descripcionMaterias[id]"\","
            print"\t\t\t\t\t\"final\":" finales[id]","
            print"\t\t\t\t\t\"recursan\":" recursados[id]","
            print"\t\t\t\t\t\"abandonaron\":" abandonos[id]","
            print"\t\t\t\t\t\"promocionan\":" promocionados[id]
            if(nroMateriaArray == length(materiasPorDepartamento[i])) {
                print"\t\t\t\t}"     
            } else {
                print"\t\t\t\t},"  
            }
            nroMateriaArray++
        }
        print "\t\t\t]"
        if(nroDeptoArray == length(materiasPorDepartamento)) {
            print "\t\t}"    
        } else {
            print "\t\t},"    
        }
        nroDeptoArray++
    }
    print "\t]"
    print "}"
}' "$1" "$2" > salida.json
}

#Menu de ayuda
case $1 in
-h|--help|-\?)
    ayuda
    exit 0
    ;;
esac

if [ $# != 4 ]
then
    echo "Error en la cantidad de parametros. Para mas informacion, ingrese a la ayuda con los parametros -h | --help | -?"
    exit 1
fi

case "$1" in
--notas)
    notas=$2
    ;;    
--materias)
    materias=$2
    ;;
*)
    echo "Error en el parametro "$1". Para mas informacion, ingrese a la ayuda con los parametros -h | --help | -?"
    exit 0
    ;;
esac
case "$3" in
--notas)
    notas=$4
    ;;    
--materias)
    materias=$4
    ;;
*)
    echo "Error en el parametro "$3". Para mas informacion, ingrese a la ayuda con los parametros -h | --help | -?"
    exit 0
    ;;
esac

if [[ ! -f $2 || ! -f $4 ]]
then
    echo "Error al leer uno de los archivos."
else
    generarReporte "$notas" "$materias"
fi