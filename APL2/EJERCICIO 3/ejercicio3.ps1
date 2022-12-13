##############################################################
##  Ejercicio Nro 3 del APL 2 - 2C-2022 - Entrega 1
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
    Este script monitorea los cambios de un determinado directorio en donde se encuentra la ultima version del codigo fuente.
.DESCRIPTION
    Simula un sistema de integración continua que ejecuta una serie de acciones por cada cambio.
.PARAMETER -codigo
    Ruta del directorio a monitorear. Puede estar vacío.
.PARAMETER -acciones
    Lista de acciones de monitoreo separadas por coma a ejecutar. Listar, peso, publicar y compilar. 
    Solo se pueden pasar las siguientes acciones:
        "listar"   : muestra por pantalla el nombre de los archivos que sufrieron cambios.
        "peso"     : muestra por pantalla el peso de los archivos que sufrieron cambios.
        "compilar" : Se concatena el contenido de los archivos en uno solo, en un directorio llamado "bin".
        "publicar" : Copia el archivo compilado a un directorio pasado por parametro "salida". Se debe usar
                     esta accion, cuando se usa compilar.
.PARAMETER -salida
    Ruta del directorio utilizado por la acción “publicar". Si el directorio no existe, se genera. 
.EXAMPLE
    ./ejercicio3.ps1 -codigo repo -acciones listar,peso
.EXAMPLE
    ./ejercicio3.ps1 -codigo repo -acciones listar,peso,compilar,publicar -salida dirPublicacion
.OUTPUTS
#>

Param(
    [Parameter(Mandatory=$True)] 
    [string]$codigo,

    [Parameter(Mandatory=$True)]
    [ValidateCount(1,4)]
    [ValidateSet("peso","listar","compilar","publicar")]
    [String[]]$acciones,

    [Parameter(Mandatory=$False)]
    [string]$salida
)

$global:listar=$False
$global:peso=$False
$global:compilar=$False
$global:publicar=$False

if( !(Test-Path $codigo) ) {
    Write-Host -ForegroundColor DarkRed "Error en el directorio. No tiene permisos para monitorear $_."
    exit 1
}

foreach ($accion in $acciones) {
    
    # Validamos la accion de listar
    if ($accion -eq "listar") {
        $global:listar=$True
    }

    if ($accion -eq "peso") {
        $global:peso=$True
    }

    # Validamos la accion de compilar
    if ($accion -eq "compilar") {
        $global:compilar=$True
        if(!(Test-Path "./bin")){
            New-Item -Path "./bin" -ItemType "directory" | Out-Null
        }
    }

    if ($accion -eq "publicar") {
        $global:publicar=$True
    }    
}

# Validamos que si se agrego la accion de publicar, se encuentre la accion de compilar.
if ( $global:publicar -and !$global:compilar ) {
    Write-Host -ForegroundColor DarkRed "No se puede publicar sin compilar. Ingrese a la ayuda para mas informacion."
    exit 1
} elseif ( $global:publicar ) {
    if ( !$salida ) {
        Write-Host -ForegroundColor DarkRed "No se puede publicar sin determinar una direccion. Ingrese a la ayuda para mas informacion."
        exit 1
    } else { 
        if( !(Test-Path $salida) ) {
            New-Item -Path $salida -ItemType "directory" | Out-Null
        }
    }
}

$global:directorioMonitoreo = (Resolve-Path -Path "$codigo").Path
$global:pathCompilados = (Resolve-Path -Path "$salida").Path


try {

    Get-EventSubscriber -SourceIdentifier Created -ErrorAction SilentlyContinue | Unregister-Event
    Get-EventSubscriber -SourceIdentifier Changed -ErrorAction SilentlyContinue | Unregister-Event
    Get-EventSubscriber -SourceIdentifier Renamed -ErrorAction SilentlyContinue | Unregister-Event
    Get-EventSubscriber -SourceIdentifier Deleted -ErrorAction SilentlyContinue | Unregister-Event

    $watcher = New-Object -TypeName System.IO.FileSystemWatcher -Property @{
        Path = $global:directorioMonitoreo
        IncludeSubdirectories = $true
    }

    $action = {
        $details = $event.SourceEventArgs
        $Name = $details.Name
        $FullPath = $details.FullPath
        $OldName = $details.OldName
        
        $ChangeType = $details.ChangeType
        
        function Pesar() {
            $mensaje = "El archivo {0} pesa {1} KB. " -f $Name, ((Get-Item $FullPath).Length / 1KB)
            Write-Host $mensaje -ForegroundColor DarkGray
        }
        
        function Compilar() {
            Get-ChildItem -Path $global:directorioMonitoreo -Recurse | Get-Content | Out-File -FilePath ./bin/compilar.txt
        }
        
        function Publicar() {
            Copy-Item -Path ./bin/compilar.txt -Destination $global:pathCompilados
        }
        
        switch ($ChangeType) {
            'Created'  { 
                if ( $global:listar ) {
                    $mensaje = "El archivo {0} fue creado. " -f $Name
                    Write-Host $mensaje -ForegroundColor Green 
                }
                if ( $global:peso ) {
                    Pesar
                }
                if ( $global:compilar ) {
                    Compilar
                }
                if ( $global:publicar ) {
                    Publicar
                } 
            }
            'Changed'  { 
                if ( $global:listar ) {
                    $mensaje = "El archivo {0} fue modificado. " -f $Name
                    Write-Host $mensaje -ForegroundColor Blue
                }
                if ( $global:peso ) {
                    Pesar
                }
                if ( $global:compilar ) {
                    Compilar
                }
                if ( $global:publicar ) {
                    Publicar
                } 
            }
            'Renamed'  { 
                if ( $global:listar ) {
                    $mensaje = "El archivo {0} fue renombrado a {1}." -f $OldName, $Name
                    Write-Host $mensaje -ForegroundColor Yellow
                }
                if ( $global:peso ) {
                    Pesar
                }
                if ( $global:compilar ) {
                    Compilar
                }
                if ( $global:publicar ) {
                    Publicar
                } 
            }
            'Deleted'  {
                if ( $global:listar ) {
                    $mensaje = "El archivo {0} fue eliminado. " -f $Name
                    Write-Host $mensaje -ForegroundColor Red
                }
                if ( $global:peso ) {
                    Pesar
                }
                if ( $global:compilar ) {
                    Compilar
                }
                if ( $global:publicar ) {
                    Publicar
                } 
            }
            default   { }
        }
    }
    
    $handlers = . {
        Register-ObjectEvent -InputObject $watcher -EventName Created -Action $action -SourceIdentifier Created
        Register-ObjectEvent -InputObject $watcher -EventName Changed -Action $action -SourceIdentifier Changed
        Register-ObjectEvent -InputObject $watcher -EventName Renamed -Action $action -SourceIdentifier Renamed
        Register-ObjectEvent -InputObject $watcher -EventName Deleted -Action $action -SourceIdentifier Deleted
    }

    $watcher.EnableRaisingEvents = $True

    Write-Warning "Monitoreando $global:directorioMonitoreo"

} finally {
    
    <#
    $watcher.EnableRaisingEvents = $False
    
    $handlers | ForEach-Object {
        Unregister-Event -SourceIdentifier $_.Name
    }
    $handlers | Remove-Job
    
    $watcher.Dispose()

    Write-Warning "Fin del monitoreo"
    #>
}