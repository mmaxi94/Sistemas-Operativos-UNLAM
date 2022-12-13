##############################################################
##  Ejercicio nro 5 del APL 2 - 2C-2022 - Entrega
##  Script: Ejercicio05.ps1
##
##  Integrantes del grupo
##  Baranda, Leonardo, 36875068
##  Morales, Maximiliano, 38176604
##  Garcia Velez Kevin, 38619312
##  Bastante, Javier, 38621360
##  Rodriguez, Cesar Daniel, 39166725
##############################################################

<#
.SYNOPSIS
    Este script analiza las notas de los alumnos para poder obtener estadisticas 
    generales de aprobación y deserción de su alumnado.
.DESCRIPTION
    Dado el archivo correspondiente de notas, el proceso analiza alumno por alumno, 
    para saber el estado de su situacion actual. Luego de analizar todas las notas de
    los alumnos, se agrupan por departamento y materia, indicando la cantidad de alumnos
    que aprobaron, que promocionaron, que desertaron, o que desaprobaron.
    El detalle se genera en un nuevo archivo llamado salida.json
.PARAMETER -notas
    Indica el archivo correspondiente a las notas a procesar.
.PARAMETER -materias
    Indica el archivo correspondiente a cada materia de las notas a procesar, y el departamento correspondiente al cual pertenecen.
.EXAMPLE
    ./ejercicio05.ps1 -notas notas.txt -materias materias.txt
.EXAMPLE
    ./ejercicio05.ps1 -materias materias.txt -notas notas.txt
.OUTPUTS
#>

Param(

    [Parameter(Mandatory = $true, Position = 1)]
    [ValidateNotNullOrEmpty()]
    [ValidateScript({
        if( !(Test-Path $_)) {
            Write-Host -ForegroundColor DarkRed "Error en el argumento del parametro notas. El archivo $_ no existe o no es valido."
            $False
        }
        $True
    })]
    [string]$notas,

    [Parameter(Mandatory = $true, Position = 2)] 
    [ValidateNotNullOrEmpty()]
    [ValidateScript({
        if( !(Test-Path $_)) {
            Write-Host -ForegroundColor DarkRed "Error en el argumento del parametro materias. El archivo $_ no existe o no es valido."
            $False
        }
        $True
    })]
    [string]$materias
)

function generarReporte() {
    $materiasRecorridas = @{}
    $recursados = @{}
    $promocionados = @{}
    $abandonos = @{}
    $finales = @{}

    $departamentos = @()
    $descripcionPorMateria = @{}
    $mapaMateriaPorDepartamento = [ordered]@{}

    # Recorro el archivo de notas.
    $datosNotas = Import-CSV -Delimiter '|' -Path $notas
    $datosNotas | ForEach-Object {

        $idMateria = $_.IdMateria
        $primerParcial = [int]$_.PrimerParcial
        $segundoParcial = [int]$_.SegundoParcial
        $recuperatorio = [int]$_.Recuperatorio
        $final = [int]$_.Final

        if(!$materiasRecorridas.ContainsKey($idMateria)) {
            $materiasRecorridas.Add($idMateria,$idMateria)
            $promocionados.Add($idMateria, 0)
            $finales.Add($idMateria, 0)
            $abandonos.Add($idMateria, 0)
            $recursados.Add($idMateria, 0)
        }

        if (( $primerParcial -ge 7 -and $segundoParcial -ge 7 ) -or 
                ( $recuperatorio -ge 7 -and (( $primerParcial -ge 7 -and $segundoParcial -lt 7 ) -or ($primerParcial -le 7 -and $segundoParcial -lt 7 )))) {
            $cantPromocionados = $promocionados[$idMateria]
            $promocionados.remove($idMateria)
            $promocionados.Add($idMateria, $cantPromocionados + 1)
        } elseif (($primerParcial -eq "" -or $segundoParcial -eq "") -and $recuperatorio -eq "") {
            $cantAbandonos = $abandonos[$idMateria]
            $abandonos.remove($idMateria)
            $abandonos.Add($idMateria, $cantAbandonos + 1)
        } elseif (($primerParcial -lt 4 -and $segundoParcial -lt 4) -or 
                (($primerParcial -lt 4 -or $segundoParcial -lt 4) -and ($recuperatorio -lt 4 -or $recuperatorio -eq ""))) {
                $cantRecursados = $recursados[$idMateria]
                $recursados.remove($idMateria)
                $recursados.Add($idMateria, $cantRecursados + 1)
        } elseif ($final -eq "" -or $final -lt 4){
            $cantfinales = $finales[$idMateria]
            $finales.remove($idMateria)
            $finales.Add($idMateria, $cantfinales + 1)
        }
    }
    
    #Recorro el archivo de materias.
    $datosMaterias = Import-CSV -Delimiter '|' -Path $materias
    $datosMaterias | ForEach-Object {
        if($materiasRecorridas.ContainsKey($_.IdMateria)) {
            $descripcionPorMateria.add($_.IdMateria, $_.Descripcion)
            $mapaMateriaPorDepartamento.add($_.IdMateria, $_.Departamento)
            if(!$departamentos.Contains($_.Departamento)) {
                $departamentos += $_.Departamento
            }
        }
    }
    
    Write-Output "{"
    Write-Output "`t`"departamentos`": ["
    $nroDeptoArray = 1
    foreach($depto in $departamentos) {
        Write-Output "`t`t{"
        Write-Output "`t`t`t`"id`": $depto,"
        Write-Output "`t`t`t`"notas`": ["
        $nroMateriaArray = 1
        $materiasPorDepartamento = $mapaMateriaPorDepartamento.GetEnumerator() | Where-Object {$_.Value -eq $depto}
        foreach($materia in $materiasPorDepartamento) {
            Write-Output "`t`t`t`t{"
            Write-Output "`t`t`t`t`t`"id_materia`": $($materia.Key),"
            Write-Output "`t`t`t`t`t`"descripcion`": `"$($descripcionPorMateria[$materia.Key])`","
            Write-Output "`t`t`t`t`t`"final`": $($finales[$materia.Key]),"
            Write-Output "`t`t`t`t`t`"recursan`": $($recursados[$materia.Key]),"
            Write-Output "`t`t`t`t`t`"abandonaron`": $($abandonos[$materia.Key]),"
            Write-Output "`t`t`t`t`t`"promocionan`": $($promocionados[$materia.Key])"
            if($nroMateriaArray -eq $materiasPorDepartamento.Count) {
                Write-Output "`t`t`t`t}"
            } else {
                Write-Output "`t`t`t`t},"
            }
            $nroMateriaArray++
        }
        Write-Output "`t`t`t ]"
        if($nroDeptoArray -eq $departamentos.Length) {
            Write-Output "`t`t}"
        } else {
            Write-Output "`t`t},"
        }
        $nroDeptoArray++
    }
    Write-Output "`t]"
    Write-Output "}"
}
if( (Test-Path $materias) -and (Test-Path $notas) ) {
    generarReporte | Out-File .\salida.json
}