# MIA-P1-202012039

Este proyecto se enfoca en el desarrollo de una aplicación de comandos en consola, que permitirá a los usuarios realizar diversas tareas relacionadas con la administración de archivos, como el formateo de particiones, la creación de aplicaciones de comandos, la aplicación de la teoría de ajustes y particiones, la generación de reportes en Graphviz y la restricción y administración del acceso a los archivos y carpetas en ext3/ext2 por medio de usuarios.

## Tabla de Contenidos
### [1. MKDISK](#1-mkdisk-1)
### [2. RMDISK](#2-rmdisk-1)
### [3. FDISK](#3-fdisk-1)
### [4. MOUNT](#4-mount-1)
### [5. UNMOUNT](#5-unmount-1)
### [6. MKFS](#6-mkfs-1)
### [7. LOGIN](#7-login-1)
### [8. LOGOUT](#8-logout-1)
### [9. MKGRP](#9-mkgrp-1)
### [10. RMGRP](#10-rmgrp-1)
### [11. MKUSR](#11-mkusr-1)
### [12. RMUSR](#12-rmusr-1)
### [13. MKFILE](#13-mkfile-1)
### [14. CAT](#14-cat-1)
### [15. REMOVE](#15-remove-1)
### [16. EDIT](#16-edit-1)
### [17. RENAME](#17-rename-1)
### [18. MKDIR](#18-mkdir-1)
### [19. COPY](#19-copy-1)
### [20. MOVE](#20-move-1)
### [21. FIND](#21-find-1)
### [22. CHMOWN](#22-chmown-1)
### [23. CHGRP](#23-chgrp-1)
### [24. CHMOD](#24-chmod-1)
### [25. PAUSE](#25-pause-1)

## 1. MKDISK
Este comando permite crear un disco virtual en el sistema de archivos del sistema operativo, el cual se creará en la ruta especificada por el usuario. El comando MKDISK tiene la siguiente sintaxis:

### Ejemplo
```bash
//Crea un disco de 3000 Kb en la carpeta home
mkdisk >size=3000 >unit=K >path=/home/user/Disco1.dsk

//Se ponen comillas la ruta de path ya que uno de sus elementos tiene espacios en
blanco, se crea si no está o no existe
mkdisk >size=5 >unit=M >path="/home/mis discos/Disco3.dsk"
```
## 2. RMDISK
Este comando permite eliminar un disco virtual en el sistema de archivos del sistema operativo, el cual se eliminará en la ruta especificada por el usuario. El comando RMDISK tiene la siguiente sintaxis:

### Ejemplo
```bash
//Elimina el disco de la carpeta home
rmdisk >path=/home/user/Disco1.dsk
```

## 3. FDISK
Este comando permite crear, eliminar y modificar particiones en un disco virtual. El comando FDISK tiene la siguiente sintaxis:

### Ejemplo
```bash
//Crea una partición primaria de 1000 Mb en el disco de la carpeta home
fdisk >size=1000 >unit=M >path=/home/user/Disco1.dsk >type=P
```

## 4. MOUNT
Este comando permite montar una partición en el sistema de archivos del sistema operativo. El comando MOUNT tiene la siguiente sintaxis:

### Ejemplo
```bash
//Monta la partición primaria del disco de la carpeta home
mount >path=/home/user/Disco1.dsk >name=part1
```

## 5. UNMOUNT
Este comando permite desmontar una partición en el sistema de archivos del sistema operativo. El comando UNMOUNT tiene la siguiente sintaxis:

### Ejemplo
```bash
//Desmonta la partición primaria del disco de la carpeta home
unmount >path=/home/user/Disco1.dsk >name=part1
```

## 6. MKFS
Este comando permite formatear una partición en el sistema de archivos del sistema operativo. El comando MKFS tiene la siguiente sintaxis:

### Ejemplo
```bash
//Formatea la partición primaria del disco de la carpeta home
mkfs >type=fast >fs=3fs >id=part1
```

## 7. LOGIN
Este comando permite iniciar sesión en el sistema de archivos del sistema operativo. El comando LOGIN tiene la siguiente sintaxis:

### Ejemplo
```bash
//Inicia sesión en el sistema de archivos del sistema operativo
login >user="mi usuario" >pass="mi pwd" >id=061Disco1
```

## 8. LOGOUT
Este comando permite cerrar sesión en el sistema de archivos del sistema operativo. El comando LOGOUT tiene la siguiente sintaxis:

### Ejemplo
```bash
//Cierra sesión en el sistema de archivos del sistema operativo
logout
```

## 9. MKGRP
Este comando permite crear un grupo en el sistema de archivos del sistema operativo. El comando MKGRP tiene la siguiente sintaxis:

### Ejemplo
```bash
//Crea un grupo en el sistema de archivos del sistema operativo
mkgrp >name=usuarios
```

## 10. RMGRP
Este comando permite eliminar un grupo en el sistema de archivos del sistema operativo. El comando RMGRP tiene la siguiente sintaxis:

### Ejemplo
```bash
//Elimina un grupo en el sistema de archivos del sistema operativo
rmgrp >name=usuarios
```

## 11. MKUSR
Este comando permite crear un usuario en el sistema de archivos del sistema operativo. El comando MKUSR tiene la siguiente sintaxis:

### Ejemplo
```bash
//Crea un usuario en el sistema de archivos del sistema operativo
mkusr >name=usuario1 >pass=usuario1 >grp=usuarios
```

## 12. RMUSR
Este comando permite eliminar un usuario en el sistema de archivos del sistema operativo. El comando RMUSR tiene la siguiente sintaxis:

### Ejemplo
```bash
//Elimina un usuario en el sistema de archivos del sistema operativo
rmusr >name=usuario1
```





