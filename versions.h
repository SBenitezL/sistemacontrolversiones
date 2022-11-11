/**
 * @file versions.h
 * @authors Santiago Benitez López <sbenitez@unicauca.edu.co> y Anderson Camilo Bonilla <acbonilla@unicauca.edu.co> bajo la asesoría de Erwin Meza Vega <emezav@gmail.com>
 * @brief Declaración de la biblioteca de versiones 
 * @version 0.1
 * @date 2022-10-27
 * @note Declara la funciones "Publicas" del control de versiones, estructuras necesarias para
 * la base de datos y retorno de códigos de retorno.
 * @copyright Copyright (c) 2022
 * 
 */
#ifndef VERSIONS_H
#define VERSIONS_H

#include <dirent.h>
#include <fcntl.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "sha256.h"

#define COMMENT_SIZE BUFSIZ /** < Longitud del comentario */
#define HASH_SIZE 65 /**< Longitud del hash incluyendo NULL*/

#define VERSIONS_DB "versions.db" /**< Nombre de la base de datos de versiones. */
#define VERSIONS_DIR ".versions" /**< Directorio del repositorio. */
#define VERSIONS_DB_PATH VERSIONS_DIR "/" VERSIONS_DB /**< Ruta completa de la base de datos.*/


#define EQUALS(s1, s2) (strcmp(s1, s2) == 0) /**< Verdadero si dos cadenas son iguales.*/

/**
 * @brief Version de un archivo.
 * Para cada version de un archivo se almacena el nombre original,
 * el comentario del usuario y el hash de su contenido.
 * El hash es a la vez el nombre del archivo dentro del 
 * repositorio.
 */
typedef struct {
	char filename[NAME_MAX]; /**< Nombre del archivo original. */
	char hash[HASH_SIZE];           /**< Hash del contenido del archivo. */
	char comment[COMMENT_SIZE];	   /**< Comentario del usuario. */
}file_version;

/**
 * @brief Código de retorno de las operaciones a realizar
 * 
 */
typedef enum {
	VERSION_ERROR, /**< Error*/
	VERSION_CREATED, /**< Versión creada*/
	VERSION_ADDED,	/**< Archivo agregado*/
	VERSION_ALREADY_EXISTS, /**< Versión ya existente*/
	VERSION_DOES_NOT_EXIST, /**<Versión no existente*/
	VERSION_RETRIEVED, /**<Versión recuperada*/
	FILE_DOES_NOT_EXISTS, /**<El archivo no existe */
	FILE_DOES_NOT_REGULAR, /**<No es un archivo regular*/
	HASH_ERROR /**< No se pudo generar el hash*/
	
}return_code;



/**
 * @brief Adiciona un archivo al repositorio.
 * 
 * @param filename Nombre del archivo a adicionar
 * @param comment Comentario de la versión actual
 * @return Código de la operación
 * FILE_DOES_NOT_EXISTS Si el archivo no existe
 * FILE_DOES_NOT_REGULAR Si no es un archivo regular
 * HASH_ERROR Si no se puede hacer el hash
 * VERSION_ALREADY_EXISTS Si ya existe la versión
 * VERSION_ERROR Si no se puede agregar el archivo
 * VERSION_ADDED Si se completó correctamente
 */
return_code add(char * filename, char * comment);

/**
 * @brief Lista las versiones de un archivo.
 *
 * @param filename Nombre del archivo, NULL para listar todo el repositorio.
 */
void list(char * filename);

/**
 * @brief Obtiene una version del un archivo.
 * Sobreescribe la version existente.
 *
 * @param filename Nombre de archivo.
 * @param version Numero secuencial de la version.
 *
 * @return Codigo de la operacion (VERSION_ERROR,VERSION_DOES_NOT_EXIST, VERSIO_RETRIEVED)
 */
return_code get(char * filename, int version);
/**
 * @brief Genera el hash del archivo referenciado y hace una busqueda en la base de datos
 * 
 * @param filename nombre del archivo
 * @return int mayor a 0 si encuentra la versión, 0 si no encuentra una versión y -1 si ocurre un error
 */
int actualVersion(char * filename);

#endif
