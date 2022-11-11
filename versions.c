/**
 * @file versions.c
 * @authors Santiago Benitez López <sbenitez@unicauca.edu.co> y Anderson Camilo Bonilla <acbonilla@unicauca.edu.co> bajo la asesoría de Erwin Meza Vega <emezav@gmail.com>
 * @brief Implementación de las funciones de versions.h, declaración de subrutinas y códigos de retorno internos
 * @version 0.1
 * @date 2022-11-06
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#include "versions.h"
/**
 * @brief Código de verificación sobre la base de datos
 * 
 */
typedef enum{
	RECORD_ALL_READY_EXIST,/**< Ya existe el registro en la base de datos*/
	RECORD_DOES_NOT_EXIST, /**< No existe el registro en la base de datos*/
	HASH_RECORD_EXIST,/**< Ya existe el hash en la base de datos*/
	RECORD_ADDED,	/**< Registro agregado*/
	RECORD_DID_NOT_ADD /**< Registro no agregado*/
}database_record;

/**
 * @brief Busca en la base de datos un archivo de nombre $filename y hash $filehash
 * 
 * @param filename Nombre del archivo
 * @param filehash Hash del archivo
 * @return find_record RECORD_ALL_READY_EXIST Si existe, RECORD_DOES_NOT_EXIST si no existe y HASH_RECORD_EXIST si existe el hash
 */
database_record findFileBy(char* filename, char* filehash);

/**
 * @brief Agrega el registro a la base de datos
 * 
 * @param s Registro a agregar
 * @return database_record 
 * RECORD_ADDED, Si se puedo agregar la versión
 * RECORD_DID_NOT_ADD, Si se produjo error
 * @note
 * Se debe asegurar de que cumpla con las condiciones de registro
 */
database_record add_version(file_version s);

/**
 * @brief Obtiene el hash de un archivo.
 * @param filename Nombre del archivo a obtener el hash
 * @param hash Buffer para almacenar el hash (HASH_SIZE)
 * @return Referencia al buffer, NULL si ocurre error
 */
char *get_file_hash(char * filename, char * hash);

/**
 * @brief Copia un archivo
 *
 * @param source Archivo fuente
 * @param destination Destino
 *
 * @return 1 en caso de exito, 0 si existe error.
 */
int copy(char * source, char * destination);



return_code add(char * filename, char * comment) {
	//Ver documentacion 2 stat
	struct stat s;
	//Hash del  archivo
	char * hash;
	//Registro para la base de datos
	file_version r;
	//Retorno de existencia de la version
	database_record result;
	//Verifica si el archivo existe
	if(stat(filename,&s) != 0)
	{
		return FILE_DOES_NOT_EXISTS;
	}
	//Verifica si es un archivo regular
	if(! S_ISREG(s.st_mode))
	{
		return FILE_DOES_NOT_REGULAR;
	}
	//Reservar espacio para el hash
	hash = (char *) malloc(HASH_SIZE);
	//Obtener el hash del archivo
	if(get_file_hash(filename,hash)==NULL)
	{
		return HASH_ERROR;
	}
	//Comprobar si la versión o el hash ya existe
	result = findFileBy(filename,hash);
	if(result == RECORD_ALL_READY_EXIST)
	{
		return VERSION_ALREADY_EXISTS;
	}
	if(result == RECORD_DOES_NOT_EXIST)
	{
		//Nuevo nombre del archivo: .versions/$hash
		//Consruir la cadena
		char * nuevo  = (char *) malloc(strlen(VERSIONS_DIR)+strlen("/")+HASH_SIZE);
		strcpy(nuevo,VERSIONS_DIR);
		strcat(nuevo,"/");
		strcat(nuevo,hash);
		//Copiar el archivo al directorio de versiones
		if(copy(filename,nuevo)==0)
		{
			return VERSION_ERROR;
		}
	}
	strcpy(r.filename,filename);
	strcpy(r.hash, hash);
	strcpy(r.comment, comment);
	if(add_version(r)== RECORD_ADDED)
	{
		return VERSION_ADDED;
	}
	return VERSION_ERROR;
}


void list(char * filename) {
	//Puntero a la base de datos
	FILE *db;
	//Buffer para la lectura
	file_version r;
	//Contador de versiones
	int cont = 1;
	//Abrir la base de datos
	db = fopen(VERSIONS_DB_PATH,"r");
	if(!db)
	{
		return;
	}
	if(filename != NULL)
	{
		while(!feof(db))
		{
			if(fread(&r,sizeof(file_version),1,db)==0)
			{
				break;
			}
			if(EQUALS(r.filename,filename))
			{
				printf("[Versión]: %d\t[Hash]: %.4s...%s\t[Comenterio]: %s\n", cont, r.hash, r.hash +strlen(r.hash)-4, r.comment);
				cont++;
			}
		}
	}
	if(filename == NULL)
	{
		while(!feof(db))
		{
			if(fread(&r,sizeof(file_version),1,db)==0)
			{
				break;
			}
			printf("[Nombre]: %s\t[Hash]: %.4s...%s\t[Comenterio]: %s\n", r.filename, r.hash, r.hash +strlen(r.hash)-4, r.comment);
			cont++;
		}
	}
	

	fclose(db);
}

char *get_file_hash(char * filename, char * hash) {
	char *comando;
	FILE * fp;

	struct stat s;

	//Verificar que el archivo existe y que se puede obtener el hash
	if (stat(filename, &s) < 0 || !S_ISREG(s.st_mode)) {
		perror("stat");
		return NULL;
	}

	sha256_hash_file_hex(filename, hash);

	return hash;

}

int copy(char * source, char * destination) {
	//Declaración de la referencia del archivo source
	FILE *f;
	//Declaracion de la referencia del archivo destino
	FILE *df;
	//Declaración del buffer donde se va a leer
	char buffer[BUFSIZ];
	//Abrir source en modo lectura
	f = fopen(source,"r");
	//Comprobar si se abrió source
	if(!f)
	{
		return 0;
	}
	//Crear y abrir el archivo destino, en modo escritura
	df = fopen(destination,"w");
	//Comprobar si se abrió el archivo
	if(!df)
	{
		// Se cierra el archivo abierto
		fclose(f);
		//Retornar bandera error
		return 0;
	}
	
	//Declaracion de la bandera de lineas leidas
	size_t nreads;
	while(!feof(f))
	{
		nreads = fread(buffer,sizeof(char), BUFSIZ,f);
		if(nreads == 0)
		{
			break;
		}
		fwrite(buffer, sizeof(char), nreads, df);

	}
	fclose(f);
	fclose(df);
	return 1;
}


return_code get(char * filename, int version) {
	//Declaración del puntero a la base de datos
	FILE *fp;
	//Declaracion de la estructura para lectura
	file_version r;
	//Contador de versiones
	int contVersion = 1;
	//Abrir la base de datos en modo lectura
	fp = fopen(VERSIONS_DB_PATH,"r");
	//Declaracion de la ruta a copiar
	char * rutaCopy;
	//Declaracion de la ruta a pegar
	char * rutaPaste;
	//Comprobar si se pudo abrir la base de datos
	if(!fp)
	{
		return FILE_DOES_NOT_EXISTS;
	}
	//Lectura sobre la base de datos mientras no sea fin del archivo
	while(!feof(fp))
	{
		//leer 1 elemento de la base de dats y almacenarlo en la estructura, comprobar si se leyó
		if(fread(&r,sizeof(file_version), 1, fp) !=1)
		{
			break;
		}
		//Comprobar si el nombre del archivo es el buscado
		if(EQUALS(filename,r.filename))
		{
			if(version == contVersion)
			{
				//Reservar memoria para la ruta a copiar
				rutaCopy = (char *) malloc(sizeof(VERSIONS_DIR)+strlen("/")+HASH_SIZE+1);
				//Reservar espacio para la ruta a pegar
				rutaPaste = (char *) malloc(sizeof("./")+sizeof(r.filename)+1);
				//Formar la ruta de copiado
				strcpy(rutaCopy,VERSIONS_DIR);
				strcat(rutaCopy,"/");
				strcat(rutaCopy,r.hash);
				//Formar la ruta a pegar
				strcpy(rutaPaste,"./");
				strcat(rutaPaste,r.filename);
				//Manda a crear o sobrescribir el archivo
				copy(rutaCopy,rutaPaste);
				//Cerrar el archivo
				fclose(fp);
				//La version se recuperó
				return VERSION_RETRIEVED;
			}
			contVersion++;
		}
	}
	fclose(fp);
	return VERSION_DOES_NOT_EXIST;
}
database_record findFileBy(char* filename, char* filehash)
{
	//Declaración del buffer de la base de datos
	FILE *bd;
	//Declaracion de la estructura a recuperar de la base de datos
	file_version r;
	//Apertura de la base de datos
	bd = fopen(VERSIONS_DB_PATH, "r");
	//Bandera de encuentro hash
	database_record resultado = RECORD_DOES_NOT_EXIST;
	//Comprobar si la base de datos se pudo abrir
	if(!bd)
	{
		//No se encontró el registro, return 0
		return RECORD_DOES_NOT_EXIST;
	}
	//Mientras no sea el final del archivo
	while(!feof(bd))
	{
		if(fread(&r,sizeof(file_version),1,bd) != 1)
		{
			//Si no pudo leer y no se encontró el archivo, retorna 0
			return resultado;
		}
		// Si el hash existe hay que comprobar si el nombre del archivo es diferente
		if(EQUALS(filehash,r.hash))
		{
			//Si el nombre del archivo es igual entonces la versión ya existe
			if(EQUALS(filename,r.filename))
			{
				//Cerrar la base de datos
				fclose(bd);
				//Se encontró el registro, retorna 1.
				return RECORD_ALL_READY_EXIST;
			}
			//Si el nombre del archivo es diferente, es una nueva versión que comparte contenido
			resultado = HASH_RECORD_EXIST;
		}
	}
	fclose(bd);
	return resultado;
	
}
database_record add_version(file_version s)
{
	//Puntero a la base de datos
	FILE *db;
	//Abrir la base de datos, en modo escritura en la última posición
	db = fopen(VERSIONS_DB_PATH,"a");
	//Comprobar si se abrió la base de datos
	if(!db)
	{
		return RECORD_DID_NOT_ADD;
	}
	//Escribir sobre la base de datos
	if(fwrite(&s,sizeof(file_version),1,db) == 0)
	{
		return RECORD_DID_NOT_ADD;
	}
	return RECORD_ADDED;
}

int actualVersion(char * filename)
{
	//Puntero a la base de datos
	FILE *db;
	//Contador de versión
	int num_ver = 1;
	//BUFFERHASH
	char hash[HASH_SIZE];
	//Estructura de lectura
	file_version r;
	//Abrir la base de datos en modo lectura
	db = fopen(VERSIONS_DB_PATH,"r");
	//Comprobar si se abrió la base de datos
	if(!db)
	{
		return -1;
	}
	//Generar el hash y comprobar 
	if(get_file_hash(filename,hash)==NULL)
	{
		return -1;
	}
	while(!feof(db))
	{
		if(fread(&r,sizeof(file_version),1,db) != 1)
		{
			fclose(db);
			return 0;
		}
		if(EQUALS(r.filename,filename))
		{
			if(EQUALS(r.hash,hash))
			{
				fclose(db);
				return num_ver;
			}
			num_ver++;
		}
	}
	fclose(db);
	return 0;
}

