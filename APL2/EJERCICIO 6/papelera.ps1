#################  ENCABEZADO  ##################
#												                        #
#	  Nombre del script: papelera.ps1			        #
#	  Numero de APL: 2						              	#
# 	Numero de ejercicio: 6					          	#
#												                        #
#   Integrantes:							              	  #
#  		Rodriguez, Cesar Daniel		39166725	      #
# 		Bastante, Javier 			    38621360	      #
#  		Garcia Velez, Kevin 	  	38619312      	#
# 		Morales, Maximiliano 	  	38176604      	#
#		  Baranda, Leonardo 		  	36875068      	#
#										                        		#
#################################################

<#
.SYNOPSIS
    El script emula el comportamiento de la papelera de reciclaje.
.DESCRIPTION
    Este script emula el comportamiento de la papelera de reciclaje con las opciones de
    recuperar, eliminar y borrar archivos, como tambien vaciar y listar el contenido de nuestra papelera.
.PARAMETER accion
    Indica la accion a realizar: listar, vaciar, recuperar, eliminar o borrar   
.PARAMETER archivo
    A la hora de querer recuperar, eliminar o borrar, se escribe el nombre del archivo que quiero accionar
.EXAMPLE
    .\papelera.ps1 -listar
    .\papelera.ps1 -recuperar nombreArchivo
    .\papelera.ps1 -vaciar
    .\papelera.ps1 -eliminar nombreArchivo
    .\papelera.ps1 -borrar nombreArchivo
#>

Param(
    [Parameter(Position=1,ParameterSetName='listar')][switch]$listar,
    [Parameter(Position=1,ParameterSetName='vaciar')][switch]$vaciar,
    [Parameter(Position=1,ParameterSetName='eliminar')][String]$eliminar,
    [Parameter(Position=1,ParameterSetName='borrar')][String]$borrar,
    [Parameter(Position=1,ParameterSetName='recuperar')][String]$recuperar
)
function listar() {
    verificoPapeleraExista
    verificoPapeleraVacia
    
    Write-Output ""
    Add-Type -Assembly 'System.IO.Compression.FileSystem'
    $zip=[System.IO.Compression.ZipFile]::Open("$papelera", "read")
    foreach($arch in $zip.Entries.FullName){
        $basename=$(Split-Path -Leaf "$arch")
        $dirname=$(Split-Path -Path "$arch")
        Write-Host $basename"   "$dirname
    }
    Write-Output ""    
    $zip.Dispose()
}

function eliminar() {
    verificoArchivoExiste
    
    $aZipear=$(get-childItem  $eliminar).FullName
    Add-Type -Assembly 'System.IO.Compression.FileSystem'
    $nivelDeCompresion = [System.IO.Compression.CompressionLevel]::Fastest

    if (Test-Path -Path "$papelera" -PathType Leaf){ #si existe la papelera actualizo contenido
        $zip = [System.IO.Compression.ZipFile]::Open("$papelera", "update");
    }else{
        $zip = [System.IO.Compression.ZipFile]::Open("$papelera", "create");
    }

    [System.IO.Compression.ZipFileExtensions]::CreateEntryFromFile($zip, $aZipear, $aZipear, $nivelDeCompresion) | Out-Null
    $zip.Dispose()
    Remove-Item "$eliminar"
    Write-Output "Se elimina archivo '$eliminar'"
}

function borrar() { #elimino archivo del zip
    verificoPapeleraExista
    verificoPapeleraVacia

    $contadorArchivosIguales=0
    $archivosIguales = ""
    $arrayArchivos = @()
  
    Add-Type -Assembly 'System.IO.Compression.FileSystem'
    $zip = [System.IO.Compression.ZipFile]::Open("$papelera", "update");
    foreach($archivoDelZip in $zip.Entries.FullName){      
      $nombreArchivo=$(Split-Path -Leaf "$archivoDelZip")
      $rutaArchivo=$(Split-Path -Path "$archivoDelZip")
  
      if("$nombreArchivo".Equals("$borrar")){
        $contadorArchivosIguales++
        $archivosIguales="$archivosIguales$contadorArchivosIguales - $nombreArchivo $rutaArchivo;"
        $arrayArchivos += "$archivoDelZip"
      }
    }
  
    if($contadorArchivosIguales -eq 0){
      Write-Host "No existe el archivo en la papelera"
      $zip.Dispose()
      exit 1
    }elseif($contadorArchivosIguales -eq 1){
      $indice=0
      foreach($archivoDelZip in $zip.Entries.FullName){
          $nombreArchivo=$(Split-Path -Leaf "$archivoDelZip")
      
          if("$nombreArchivo".Equals("$borrar")){
              break
          }
          $indice++
      }
      $zip.Entries[$indice].Delete();
    }else{
      foreach($linea in "$archivosIguales".Split(";")){
        Write-Host "$linea";
      }
      
      $opcion = Read-Host "¿Qué archivo desea borrar? ";
      if(($opcion -le 0) -or ($opcion -gt $arrayArchivos.Count )){
        Write-Host "Opción invalida";
        $zip.Dispose();
        exit 1;
      }
      
      try {
        $seleccion = $arrayArchivos[$opcion-1];
        $indice=0;
      }
      catch {
        Write-Host "Opción invalida";
        $zip.Dispose();
        exit 1;
      }
  
      foreach($archivoDelZip in $zip.Entries.FullName){    
        if("$archivoDelZip".Equals("$seleccion")){
            break;
        }
        $indice++;
      }
      try {
        $zip.Entries[$indice].Delete();
      }
      catch {
        Write-Host "Opción invalida"; 
        $zip.Dispose();
        exit 1;
      }
    }
    $zip.Dispose();
    Write-host "Se borra archivo '$borrar'"
}

function vaciar() {
    verificoPapeleraExista
    verificoPapeleraVacia
    
    Remove-Item "$papelera"
    Add-Type -Assembly 'System.IO.Compression.FileSystem'
    $zip = [System.IO.Compression.ZipFile]::Open("$papelera", 'create')
    $zip.Dispose();
    Write-Output "La papelera fue vaciada"
}

function recuperar(){
    verificoPapeleraExista
    verificoPapeleraVacia

    $contadorArchivosIguales=0
    $archivosIguales = ""
    $arrayArchivos = @()
  
    Add-Type -Assembly 'System.IO.Compression.FileSystem'
    $zip = [System.IO.Compression.ZipFile]::Open("$papelera", "update");
    
    foreach($archivoDelZip in $zip.Entries.FullName){      
      $nombreArchivo=$(Split-Path -Leaf "$archivoDelZip")
      $rutaArchivo=$(Split-Path -Path "$archivoDelZip")
  
      if("$nombreArchivo".Equals("$recuperar")){
        $contadorArchivosIguales++
        $archivosIguales="$archivosIguales$contadorArchivosIguales - $nombreArchivo $rutaArchivo;"
        $arrayArchivos += "$archivoDelZip"
      }
    }
  
    if($contadorArchivosIguales -eq 0){
      Write-Host "No existe el archivo en la papelera"
      $zip.Dispose()
      exit 1
    }elseif($contadorArchivosIguales -eq 1){
      $indice=0
      foreach($archivoDelZip in $zip.Entries.FullName){
          $nombreArchivo=$(Split-Path -Leaf "$archivoDelZip")
      
          if("$nombreArchivo".Equals("$recuperar")){
             [System.IO.Compression.ZipFileExtensions]::ExtractToFile($zip.Entries[$indice], "$archivoDelZip", $true)
             break
          }
          $indice++
      }
      $zip.Entries[$indice].Delete();
    }else{
      foreach($linea in "$archivosIguales".Split(";")){
        Write-Host "$linea";
      }
      
      $opcion = Read-Host "¿Que archivo desea recuperar? ";
      if(($opcion -le 0) -or ($opcion -gt $arrayArchivos.Count )){
        Write-Host "Opción invalida";
        $zip.Dispose();
        exit 1;
      }
      
      try {
        $seleccion = $arrayArchivos[$opcion-1];
        $indice=0;
      }
      catch {
        Write-Host "Opción invalida";
        $zip.Dispose();
        exit 1;
      }
  
      foreach($archivoDelZip in $zip.Entries.FullName){
        if("$archivoDelZip".Equals("$seleccion")){
          [System.IO.Compression.ZipFileExtensions]::ExtractToFile($zip.Entries[$opcion-1], "$archivoDelZip", $true);
          break;
        }
        $indice++;
      }
      try {
        $zip.Entries[$opcion-1].Delete();
      }
      catch {
        Write-Host "Opción invalida"; 
        $zip.Dispose();
        exit 1;
      }      
    }
    
    $zip.Dispose();
    Write-host "Archivo recuperado"
}

function verificoPapeleraExista() {
  if(!(Test-Path "$papelera")){
    Write-host "Error, no existe la papelera"
    exit 1
  }    
}
function verificoPapeleraVacia() {
  if( $(get-childItem "$papelera").Length -le 22 ){ #un archivo zip vacio pesa 22bytes
      Write-Output "Error, la papelera está vacía"
      Exit 1
  }
}
function verificoArchivoExiste() {
    if (!( Test-Path -Path $eliminar )){
      Write-Output "Error, el archivo '$eliminar' no existe"
      Exit 1
    }
}

  function errorParametros(){
    Write-Output "
      #####################################################
  
      Error revisar parametros ingresados
    
      Se recomienda usar la ayuda -> Get-Help ./papelera.ps1
  
      #####################################################
  
      "
    Exit
}
  
$papelera="${HOME}/papeleraPS.zip"
if($listar){
    listar
    Exit
}
elseif($vaciar){
    vaciar
    Exit
}
elseif($eliminar){
    eliminar
    Exit
}
elseif($borrar){
    borrar
    Exit
}
elseif($recuperar){
    recuperar
    Exit
}else{
    errorParametros
    Exit 1
}