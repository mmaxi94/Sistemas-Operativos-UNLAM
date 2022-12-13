################################################################################
##  Ejercicio nro 2 del APL 2 - 2c 2022 - Entrega nro 3
##  Script: Ejercicio03.sh
##
##  Integrantes del grupo
##  Rodriguez,Cesar Daniel, 39166725
##  Bastante, Javier, 38621360
##  Garcia Velez, Kevin, 38619312
##  Morales ,Maximiliano, 38176604
##  Baranda Leonardo, 36875068
################################################################################

<#
.SYNOPSIS
Esta función arroja una estadistica a partir de un log 
.DESCRIPTION
Resumen del Script: Dado 1 directorio que contiene 1 archivo logs donde se guardan los registros de llamadas de 1 semana, el script procesa los datos y muestra
algunas estadisticas solicitadas. Cada llamada debe contar con 2 registros, uno indicando su comienzo, y, el otro, su fin.
El log debe tener los registros en el siguiente formato:
AAA-MM-DD HH:MM:SS _ [usuario]
#donde [usuario] es el nombre del usuario (longitud variable)
.EXAMPLE
 ./ejercicio2.ps1 -logs "nombre_directorio"
#>
#VALIDACION DE PARAMETROS
param(
    [CmdletBinding()]
    [Parameter(Mandatory = $true)]
    [ValidateNotNullOrEmpty()]
    [ValidateScript( { Test-Path $_ })]
    $logs
)

# convierte un numero entero (que expresa una cantidad de segundos) a un string de formato "hh:mm:ss"
Function convertHora {
    [CmdletBinding()]
    param(
        [Parameter(Mandatory = $true)][int] $segundos
    )

    $horasAux = ([int]($segundos / 3600)) % 3600
    $minutosAux = ([int]($segundos / 60)) % 60
    $segundosAux = $segundos % 60

    if ($horasAux -lt 10) {
        $horasAux = "0" + $horasAux
    }
    if ($minutosAux -lt 10) {
        $minutosAux = "0" + $minutosAux
    }
    if ($segundosAux -lt 10) {
        $segundosAux = "0" + $segundosAux
    }
    return $horasAux + ":" + $minutosAux + ":" + $segundosAux
}

Function RestarHoras {
    [CmdletBinding()]
    param(
        [Parameter(Mandatory = $true)][string] $hora1,
        [Parameter(Mandatory = $true)][string] $hora2
    )

    $hora1Aux = $hora1.split(":")
    [int]$horas1 = $hora1Aux[0]
    [int]$minutos1 = $hora1Aux[1]
    [int]$segundos1 = $hora1Aux[2]

    $hora2Aux = $hora2.split(":")
    [int]$horas2 = $hora2Aux[0]
    [int]$minutos2 = $hora2Aux[1]
    [int]$segundos2 = $hora2Aux[2]

    #SI LOS DATOS VIENEN DESORDENADOS USO LA FUNCION ABS DE MATH
    return [System.Math]::Abs(($horas1 * 3600 + $minutos1 * 60 + $segundos1) - ($horas2 * 3600 + $minutos2 * 60 + $segundos2))
}

function Get-Values {
    param (
        [CmdletBinding()]
        [Parameter(Mandatory = $true)]
        [ValidateNotNullOrEmpty()]
        [ValidateScript( { Test-Path $_ })]
        $dir 
    )
    
    begin { 
        #declaramos variables
        $vacio = $true
        $archivosVacios = $true
        $usuarioEnLlamada = @{ }
        $duracionAux = @{ }
        $promedioPorDia = @{ }
        $cantidadPorDia = @{ }
        $textPromedioPorDia = @{ }
        $promedioPorDiaPorUsuario = @{ }
        $cantidadPorDiaPorUsuario = @{ }
        $textPorDiaPorUsuario = @{ }
        $cantidadPorUsuario = @{ }
        $duracionPorLlamadaPorUsuario = @{ }
        [int]$mediaLlamadas = 0
        [int]$cantidadLlamadas = 0
        [int]$llamadasBajoMedia = 0
        $usuariosBajoMedia = @{ }

        $fileList = Get-ChildItem $dir
        
    }
    
    process {

        foreach ($file in $fileList.Name) {
            $vacio = $false
            
            foreach ($line in Get-Content ("$dir/$file")) {
                
                $archivosVacios = $false                

                $aux = $line.Split(" ")
                $subAux = $aux[1].split("-")

                $dia = $aux[0]
                $hora = $subAux[0]
                $usuario = $subAux[1]
       

                if ($usuarioEnLlamada.Contains($usuario) -and $usuarioEnLlamada[$usuario] -eq $true ) {
                    $usuarioEnLlamada[$usuario] = $false
                    $duracionLlamada = RestarHoras "$hora" ($duracionAux[$usuario])
                    #punto 1
                    $promedioPorDia[$dia] += $duracionLlamada
                    $cantidadPorDia[$dia]++
                    #punto 2
                    $delimPorDiaPorUsuario = $dia + "-" + $usuario            
                    $promedioPorDiaPorUsuario[$delimPorDiaPorUsuario] += $duracionLlamada
                    $cantidadPorDiaPorUsuario[$delimPorDiaPorUsuario]++
                    #punto 3
                    $cantidadPorUsuario[$usuario]++
                    #punto 4
                    $duracionPorLlamadaPorUsuario[$usuario + [string]($cantidadPorUsuario[$usuario])] += $duracionLlamada
                    $cantidadLlamadas++
                    $mediaLlamadas += $duracionLlamada
                }
                else {
                    #si no estaba en llamada, genero una nueva y registro la hora a la que comienza

                    $usuarioEnLlamada[$usuario] = $true
                    $duracionAux[$usuario] = $hora

                }

            }
        }
    }
    
    end {

        if ($vacio -eq $true) {
            Write-Host El directorio esta vacio
            exit
        }
        else {
            if ($archivosVacios -eq $true) {
                Write-Host Todos los archivos del directorio están vacios
                exit
            } 
        }

        #1) Promedio de tiempo de las llamadas realizadas por día.
        foreach ($d in $cantidadPorDia.GetEnumerator() | Sort-Object -Property Name -Descending) {
            $textPromedioPorDia[$d.Name] = convertHora ($promedioPorDia[$d.Name] / $cantidadPorDia[$d.Name])
        }
        Write-Host
        Write-Output "_________________________________________________________________________"
        Write-Output "1) Promedio de tiempo de las llamadas por dia"
        Write-Output $textPromedioPorDia | Format-Table -HideTableHeaders

        #2) Promedio de tiempo y cantidad por usuario por día.
        foreach ($du in $cantidadPorDiaPorUsuario.GetEnumerator() | Sort-Object -Property Name) {
            $textPorDiaPorUsuario[$du.name] = "Cantidad: " + $cantidadPorDiaPorUsuario[$du.Name] + " - Promedio: " + 
            (convertHora ($promedioPorDiaPorUsuario[$du.Name] / $cantidadPorDiaPorUsuario[$du.Name]))
        }
        Write-Output "_________________________________________________________________________"
        Write-Output "2) Promedio de tiempo y cantidad por usuario por dia"
        Write-Output $textPorDiaPorUsuario | Format-Table -HideTableHeaders

        #3) Los 3 usuarios con más llamadas en la semana.
        Write-Output "__________________________________________________________________________"
        Write-Output "3)Los 3 usuarios con mas llamadas en la semana`n"
        foreach ($u in $cantidadPorUsuario.GetEnumerator() | Sort-Object -Property Value -Descending | Select-Object -First 3) {
            Write-Host $u.Name con $u.Value llamadas
        }
        Write-Host

        #4) Cuántas llamadas no superan la media de tiempo por día y el usuario que tiene más
        Write-Output "_________________________________________________________________________"
        Write-Output "4) Llamadas que no superan la media de tiempo por dia"
        $mediaLlamadas /= $cantidadLlamadas       
        foreach ($u in $cantidadPorUsuario.GetEnumerator() | Sort-Object -Property Value -Descending) {
            $n = $cantidadPorUsuario[$u.Name]
            for ($i = 1; $i -le $n; $i++) {

                $index = $u.Name + ($i.ToString())

                if (($duracionPorLlamadaPorUsuario[$index]) -lt $mediaLlamadas) {
                    $llamadasBajoMedia++
                    $usuariosBajoMedia[$u.Name]++
                }
            }
        }      
        Write-Host Hay $llamadasBajoMedia llamadas bajo la media `(media = (convertHora $mediaLlamadas)`)

        Write-Output "_________________________________________________________________________"
        Write-Output "5) usuario que tiene más cantidad de llamadas por debajo de la media en la semana."
        
        foreach ($u in $usuariosBajoMedia.GetEnumerator() | Sort-Object -Property Value -Descending | Select-Object -First 1) {
            Write-Host $u.Name tiene mas llamadas bajo la media, con $u.Value llamadas
        }
        Write-Output "_____________________________________________________________________"
        Write-Host       
    }
}

Get-Values $logs
