################################################################################
##  Ejercicio nro 1 del APL 2 - 2c 2022 - Entrega nro 1
##  Script: Ejercicio03.sh
##
##  Integrantes del grupo
##  Rodriguez,Cesar Daniel, 39166725
##  Bastante, Javier, 38621360
##  Garcia Velez, Kevin, 38619312
##  Morales ,Maximiliano, 38176604
##  Baranda Leonardo, 36875068
################################################################################


Param (
 [Parameter(Position = 1, Mandatory = $false)]
 [String] $pathSalida = ".\procesos.txt",
 [int] $cantidad = 3
)

$existe = Test-Path $pathSalida
if ($existe -eq $true) {
 $procesos = Get-Process | Where-Object { $_.WorkingSet -gt 100MB }
 
 $procesos | Format-List -Property Id,Name >> $pathSalida
 
 for ($i = 0; $i -lt $cantidad ; $i++) {
 Write-Host $procesos[$i].Id - $procesos[$i].Name
 }
} else {
 Write-Host "El path no existe"
}


#RESPUESTAS

#1. El objetivo del script es guardar el listado de procesos (id y nombre) que este corriendo que pesen mas de 100MB
#   luego lista el id y el nombre de los 3 primeros por pantalla. Recibe como parametro la ruta junto al nombre del archivo donde debe guardarlo.

#2. Se podrian generar validaciones de permisos en las rutas pasadas por parametro. 

#3. Si el script se ejecuta sin parametros, existen dos posibilidaes. Si existe el archivo procesos.txt dentro de la carpeta donde se ejecuta
#   el script, escribira con los procesos mas pesados que 100MB. Sino, comunicara por pantalla que el path no existe. 