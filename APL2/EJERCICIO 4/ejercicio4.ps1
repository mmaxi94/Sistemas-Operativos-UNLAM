#################  ENCABEZADO  ##################
#												#
#	Nombre del script: ejercicio4.ps1			#
#	Numero de APL: 2							#
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

<#
.SYNOPSIS
    El script cuenta la cantidad de comentarios y lineas de codigo de todos los archivos dentro de un directorio.  
.DESCRIPTION
    Este script cuenta la cantidad de lineas de codigo y de comentarios que poseen los archivos en una ruta pasada por parametro y controlando solo los archivos con cierta extension, en forma recursiva en todos los subdirectorios
    El script se invoca de la siguiente forma:
    ./ejercicio4.ps1 -ruta <directorio a analizar> -ext <lista de extensiones>
.PARAMETER ruta
    Indica el directorio a evaluar. Para directorios con espacios en blanco entre si, colocar la ruta entre comillas dobles   
.PARAMETER ext
    lista de extensiones a analizar dentro del parametro ruta
    las extensiones estaran separadas por , (punto y coma). Ejemplo: sh,cs,js,css
.EXAMPLE
    ./ejercicio4.ps1 -ruta \Pruebas -ext css,txt,java
    ./ejercicio4.ps1 -ruta "\carpeta test" -ext css,txt,java
#>

Param(  <# -ruta <String> -ext <String> #>
    
    [Parameter(Mandatory=$True)]
#    [ValidateScript({  If(Test-Path $_){$true}else{Throw "ruta invalida: $_"}})]
    #[ValidateScript({Test-Path $_})]
    [ValidateNotNullOrEmpty()]
    [string] $ruta,
    
    [Parameter(Mandatory=$True)]
    [ValidateNotNullOrEmpty()]
    [string[]] $ext
    
)

function Get-Validaciones()
{
    # valido si -ruta es un directorio
    if ( (Test-Path $ruta -PathType Container -ErrorAction Ignore) -eq $False)
    {
        Write-Output "El valor de -ruta no es un directorio"
        Write-Output "Para ayuda ingrese:"
        Write-Output 'Get-Help ./ejercicio4.ps1 -Detailed'
        exit 1
    }

}

function Get-Archivos()
{
    # leemos las extensiones del archivo -ext
    #$extensiones = $ext.Split(",")
    $extensiones = $ext

    # buscamos los archivos en el directorio -ruta y los cargamos en un array
    foreach ($extension in $extensiones)
    {
        $arrayArchivos += @(Get-ChildItem -Path "$ruta" -Filter *.$extension -Recurse).Fullname
    }

    return $arrayArchivos
}

function resolver()
{

$acumuladorArchivos=0
$acumuladorLineasCodigoTotales=0
$acumuladorTotalLineas=0
$acumuladorComentariosTotales=0
$cantArch=0

$arrayArchivos= Get-Archivos

foreach ($item in $arrayArchivos)
    {
        if ($item) 
        {
        $cantArch++
        $lineas=0
		$comentarios=0
        $codigoYcomentario='^[\s]*[^\s]+.*\/\/.*'
		$soloComentario='^[\s]*\/\/.*'
		$codigoYmultilineaStart='^[\s]*[^\s]+.*\/\*.*'
		$multilineaStart='^[\s]*\/\*.*'
        $multilineaEnd='.*(\*\/)$'
		$multiline=$false
		$total=0

        Write-Output "Analizando archivo: $item"

        $regex = ''

        foreach($line in Get-Content -Path $item) 
        {
            if($line -match $regex)
            {

                if  ( $multiline -eq $false ) {				
					# Cuenta línea de código mas comentario
					# ej: codigo // comentario
					if ( $line -match $codigoYcomentario  ) {
						$lineas++
						$comentarios++
                    }
					# Cuenta comentarios de una línea. ej.: //comentario
					elseif ( $line -match $soloComentario ) {
						$comentarios++
                    }
					# Si encontramos el inicio de un comentario multilinea, activamos el flag
					elseif ( $line -match $multilineaStart ){
						$multiline=$true
						$comentarios++
						#Si encuentra en la misma linea el cierre del comentario multilinea, desactivamos flag
						if ( $line -match $multilineaEnd ){
							$multiline=$false
                        }
                    }
					# Si encontramos linea de codigo mas comienzo de comentario multilinea, activamos el flag
					# ej.: codigo /*Comentario multilinea
					elseif ( $line -match $codigoYmultilineaStart ){
						$multiline=$true
						$lineas++
						$comentarios++
						# Si encuentra en la misma linea el cierre del comentario multilinea, desactivamos flag
						if ( $line -match $multilineaEnd){
							$multiline=$false
                        }
                    }
					else{
						# Si no cumple ninguna de las condiciones, la consideramos linea de codigo
						$lineas++}
                    
                }
	
				else {
					# Nos fijamos si encontramos el fin de un commentario multilinea
					if ( $line -match $multilineaEnd ) {
						$multiline=$false
                    }
					$comentarios++
                    }
            }

                #Write-Output "##############"
                #Write-Output "linea: $line"
                #Write-Output "lineas: $lineas"
                #Write-Output "comentarios: $comentarios"
                #Write-Output "multiline: $multiline"
        }

        $acumuladorLineasCodigoTotales=$acumuladorLineasCodigoTotales+$lineas
        $acumuladorComentariosTotales=$acumuladorComentariosTotales+$comentarios
        
        $total=(Get-Content $item).Length
			
			    if ($total -ne 0 ){
			    	$total++ } 
			    
        
        $acumuladorTotalLineas=$acumuladorTotalLineas+$total

        }
    }
       
        $acumuladorArchivos=$cantArch

    
    if ( $acumuladorTotalLineas -ne 0 ){
		$porcentajeLineas=[math]::round(($acumuladorLineasCodigoTotales/$acumuladorTotalLineas)*100,2)
		$porcentajeComm=[math]::round(($acumuladorComentariosTotales/$acumuladorTotalLineas)*100,2)
    }
	
	Write-Output ""		
	Write-Output "Total de archivos analizados: $acumuladorArchivos"
	Write-Output ""
	Write-Output "Cantidad de lineas de codigo totales: $acumuladorLineasCodigoTotales | Porcentaje: $porcentajeLineas"
	Write-Output "Cantidad de lineas de comentario totales: $acumuladorComentariosTotales | Porcentaje: $porcentajeComm"
	Write-Output ""

}

function Main()
{
    Get-Validaciones
    resolver
}

Main