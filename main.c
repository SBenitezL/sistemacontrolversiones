/**
 * @file main.c
 * @authors Santiago Benitez López <sbenitez@unicauca.edu.co> y Anderson Camilo Bonilla <acbonilla@unicauca.edu.co> bajo la asesoría de Erwin Meza Vega <emezav@gmail.com>
 * @brief Programa principal que reune y ejecuta las funcionalidades Sistema de Control de Versiones
 * @note Uso: 
 *      versions add ARCHIVO "Comentario" : Adiciona una version del archivo al repositorio
 *      versions list ARCHIVO             : Lista las versiones del archivo existentes
 *      versions get numver ARCHIVO       : Obtiene una version del archivo del repositorio
 * @date 2022-11-06
 * @copyright Copyright (c) 2022
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "versions.h"

#define ERROR_SIZE 25 /**< Longitud máxima del msj de error*/
/**
* @brief Imprime la ayuda
*/
void print_help();

/**
 * @brief Determinar la función a ejecutar e interpretar el resultado
 * 
 * @param argc Número de argumentos ingresados por Consola
 * @param argv Valor de los argumentos ingresados por consola
 * @return int EXIT_FAILURE o EXIT_SUCCESS
 */
int main(int argc, char *argv[]) {
	struct stat s;
	//Código de retorno
	return_code resultado;
	//Numero de version
	int num_ver;
	//Mensaje de error
	char * msjError = (char *) malloc(ERROR_SIZE);
	//Si no existe el directorio, crearlo
	if(stat(VERSIONS_DIR,&s) != 0)
	{
		if(mkdir(VERSIONS_DIR, 0755)!=0)
		{
			perror("No se pudo iniciar el repositorio");
			exit(EXIT_FAILURE);
		}
	}
	//Crea la base de datos su no existe
	if (stat(VERSIONS_DB_PATH, &s) != 0) {
		if(creat(VERSIONS_DB_PATH, 0755)<0)
		{	
			perror("No se pudo iniciar la base de datos");
			exit(EXIT_FAILURE);
		}
	}

	if (argc == 4
			&& EQUALS(argv[1], "add")) {
		// ./versions add Filename Coment
		resultado = add(argv[2], argv[3]);
		if (resultado != VERSION_ADDED) {
			strcpy(msjError,"No se puede adicionar: ");
			if(resultado == FILE_DOES_NOT_EXISTS)
			{
				fprintf(stderr,"%s %s: No existe el archivo\n",msjError, argv[2]);
				exit(EXIT_FAILURE);
			}
			if(resultado == FILE_DOES_NOT_REGULAR)
			{
				fprintf(stderr,"%s %s: No es un archivo regular\n",msjError, argv[2]);
				exit(EXIT_FAILURE);
			}
			if(resultado == HASH_ERROR)
			{
				fprintf(stderr,"%s %s: No se pudo calcular el hash\n",msjError, argv[2]);
				exit(EXIT_FAILURE);
			}
			if(resultado == VERSION_ALREADY_EXISTS)
			{
				fprintf(stderr,"%s %s: Ya existe la versión\n",msjError, argv[2]);
				exit(EXIT_FAILURE);
			}
			if(resultado == VERSION_ERROR)
			{
				fprintf(stderr,"%s %s: Ocurrió un problema con el repositorio\n",msjError, argv[2]);
				exit(EXIT_FAILURE);
			}
		}
	}else if (argc == 2
			&& EQUALS(argv[1], "list")) {
		// ./versions list
		//Listar todos los archivos almacenados en el repositorio
		list(NULL);
	}else if(argc == 3 && EQUALS(argv[1],"check")){
		num_ver = actualVersion(argv[2]);
		printf("%d \n",num_ver);
		exit(EXIT_SUCCESS);
	}
	else if (argc == 3
			&& EQUALS(argv[1], "list")) {
		// ./versions list FileName
		//Listar el archivo solicitado
		list(argv[2]);
	}else if (argc == 4
			&& EQUALS(argv[1], "get")) {
		// ./versions get FileName
		int version = atoi(argv[2]);
		if (version <= 0) {
			fprintf(stderr, "Numero de version invalido\n");
			exit(EXIT_FAILURE);
		}
		if (get(argv[3], version) != VERSION_RETRIEVED) {
			fprintf(stderr, "No se puede obtener la version %d de %s\n", version, argv[3]);
			exit(EXIT_FAILURE);
		}
	}else {
		print_help();
	}

	exit(EXIT_SUCCESS);

}

void print_help() {
	printf("Uso: \n");
	printf("versions add ARCHIVO \"Comentario\" : Adiciona una version del archivo al repositorio\n");
	printf("versions list ARCHIVO             : Lista las versiones del archivo existentes\n");
	printf("versions get numver ARCHIVO       : Obtiene una version del archivo del repositorio\n");
	printf("versions check ARCHIVO		  : Busca si la versión actual existe y cuál es\n");
}
