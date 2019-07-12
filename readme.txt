/************************************************************************/
/* DropMusic - Gestor de Músicas					*/
/*									*/
/* Created by:	Bruno Oliveira		brunogoliveira8@gmail.com	*/
/*		Ricardo Barreto		rmcbarreto@gmail.com		*/
/*									*/
/* Developed in QtCreator IDE 5.11.2					*/
/*									*/
/************************************************************************/

Requirements:
The sofware was compiled to execute on Windows. 
The software was developed with Qt Creator 5.11.2 IDE and compiled with MinGW.
The softwre was tested on Windows 10 18.03 and 18.09 versions.

Install:
The compressed file "DropMusicEXE.zip" has the executable program. 
Uncompress the file and execute the "DropMusic.exe" file and the software will open.
It was necessary to add the files "libmysql.dll", "libmysql.lib" and "db_connect.txt" so the software could work.
The first 2 files are the library to connect to mySQL DB server. 
The last file is the file that contains the credentials to the DB server.

The "db_connect.txt" file have the following structure:
server: server_address
username: username
password: password
database-name: database_name


The file "DropMusicQTSourceFiles.zip" has the source code of our project.
The project is also available in GitLab:
https://gitlab.com/rmcbarreto/drop-music

The file "dropMusic_db_sql_code.sql" has the code to create de DataBase in mySQL server.

