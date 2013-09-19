/*-----------------------------------------------------------------------------
FICHERO: cmdelay.c
APLICACION: Ejecucion de comandos sobre la shell de Unix.
-----------------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <ctype.h>


/*DEFINICIONES DE PREPROCESADOR.*/
/*----------------------------------------------------------------------------*/
#define DEFECTO_CMD_FILE "cmd"	/*Archivo de comandos.*/
#define DEFECTO_DELAY 1			/*Retrazo por defecto.*/
#define DEFECTO_CMDS NULL
#define MAX_ENTRADAS 30 /*Cantidad maxima de entradas en el CMD_FILE.*/
#define STDIN 0
#define STDOUT 1
#define MAX_KEYB 5
#define MAX_LINEA 255

#define entrada_posible(x) isupper(x) || isdigit(x)
/*----------------------------------------------------------------------------*/


/*PROTOTIPOS DE FUNCIONES.*/
/*----------------------------------------------------------------------------*/
void sintaxis(void);
void ayuda(void);
int cargar_entradas(FILE *, char *, unsigned char *);
int strcmp_extendida(char *, char *);
void clrscr(void);
void cmdline(void);
void msg(char *);
unsigned char existe_entrada(char *, unsigned char, char);
int cargar_comandos(FILE *, char **, char);
void imprimir_comandos(char *);
char *buscar_parametros(char, char **, char);
int instanciar_variables(char **);
/*----------------------------------------------------------------------------*/
extern int errno;

/*Estructura de almacenamiento de datos.*/
typedef struct {
	int delay;
	char unidad_delay;
	char *cmd_file;
} ST_DATOS;

/*-----------------------------------------------------------------------------
FUNCION: main(). 
OBJETIVO: Funcion principal.

PARAM_IN:
	char		Numero de argumentos.
	char **		Doble puntero a los argumentos.
PARAM_OUT:
	int			Estado de ejecucion de la funcion. 
-----------------------------------------------------------------------------*/
int main(char argc, char **argv) {
	ST_DATOS st_datos;	/*Estructura de datos iniciales.*/
	FILE *cmd_file;
	unsigned char entradas, b_salir=0, i;
	char vec_entradas[MAX_ENTRADAS], *aux_tv, keyb[MAX_KEYB], 
		*cmd_buffer=NULL, *cmd_buffer_aux=NULL, *param, b_param_ent=0, param_ent;
	int num_desc;
	fd_set bolsa;
	struct timeval tv;

	/*Carga por defecto de st_datos.*/
	st_datos.delay=DEFECTO_DELAY;
	st_datos.unidad_delay='s';
	st_datos.cmd_file=DEFECTO_CMD_FILE;
	
	/*Chequeo de argumentos.*/
	for(i=1; i<argc; ++i) {
		if(*argv[i]=='-') {
			switch(argv[i][1]) {
				case 'd':
					if((param=buscar_parametros(argc, argv, i))==NULL) {
						sintaxis();
						exit(1);
					}
						
					st_datos.delay=atoi(param);
					if(param[strlen(param)-1]=='u')
						st_datos.unidad_delay='u';
					break;
				case 'c':
					if((param=buscar_parametros(argc, argv, i))==NULL) {
						sintaxis();
						exit(1);
					}
					st_datos.cmd_file=param;
					break;
				case 'e':
					if((param=buscar_parametros(argc, argv, i))==NULL) {
						sintaxis();
						exit(1);
					}
					param_ent=*param;
					b_param_ent=1;
					break;
				default:
					sintaxis();
					exit(1);
					break;
			}
		}
	}
	
	if((cmd_file=fopen(st_datos.cmd_file, "r+"))==NULL) {
		perror("main(): fopen()");
		exit(1);
	}
	if(cargar_entradas(cmd_file, vec_entradas, &entradas)==-1) {
		perror("main(): cargar_entradas()"); exit(1);
	} 

	if(b_param_ent) {/*Si se cargo por parametro una entrada a ejecutar.*/
		if(existe_entrada(vec_entradas, entradas, param_ent)) {
			if(cargar_comandos(cmd_file, &cmd_buffer, param_ent)==-1) {
				perror("main(): cargar_comandos()");
				exit(1);
			}
		}
		else {
			msg("No existe entrada");
			exit(1);
		}
	}
	else {
		if(DEFECTO_CMDS && (cmd_buffer=(char *) malloc(strlen(DEFECTO_CMDS)))==NULL) {
			perror("main(): malloc()");
			exit(1);
		}
		/*Comandos o por defecto o enviado por argumento.*/
		if(DEFECTO_CMDS) strcpy(cmd_buffer, DEFECTO_CMDS);
	}
	if(instanciar_variables(&cmd_buffer)==-1) {
		perror("main(): instanciar_variables()");
		exit(1);
	}
	
	/*Para optimizar el codigo.*/
	if((aux_tv=(char *) malloc(sizeof(struct timeval)))==NULL) {
		perror("main(): malloc()");
		exit(1);
	}
	memcpy((void *) aux_tv+ ((st_datos.unidad_delay=='u') ? sizeof(__time_t):0), (void *) &st_datos.delay, sizeof(st_datos.delay));
	
	cmdline();
	while(!b_salir) {
		/*Configuracion del select.*/
		FD_ZERO(&bolsa); FD_SET(STDIN, &bolsa);
		memcpy((void *) &tv, aux_tv, sizeof(struct timeval));

		switch (num_desc=select(STDIN+1, &bolsa, NULL, NULL, &tv)) {
		
			case -1:	/*ERROR.*/
				if(errno!=EINTR) { /*No fue una signal recibida.*/
					perror("main(): select()");
					exit(1);
				}
				break;
		
			case 0:		/*TIMEOUT.*/
				/*Ejecuta los comandos en la shell si cmd_buffer!=NULL.*/
				if(cmd_buffer)
					system(cmd_buffer);
				break;
			
			default:	/*STDIN.*/
				/*Lectura: (c1)(c2)(c3)...\n
					c1=':'			->	Comando especial.
					c2=[1..9][A..Z]	->	Imprimir comandos de entrada [c1].
					c3=xxx			->	Mirar menu.
				*/
				if(!FD_ISSET(STDIN, &bolsa)) break;
				fgets(keyb, MAX_KEYB, stdin);
				
				/*Seleccion de accion.*/
				if(keyb[0]==':' && isalnum(keyb[1])) { /*Comando de accion especial.*/
					if(entrada_posible(keyb[1])) {/*Es una entrada.*/
						if(!existe_entrada(vec_entradas, entradas, keyb[1]))
							msg("No existe entrada");
						else {
							switch(keyb[2]) {
								case 'p':	/*IMPRIMIR COMANDOS DE ENTRADA.*/
									/*En caso que se haya cargado anteriormente se libera su espacio, ya que se utilizara posiblemente otra direccion.*/
									if(cmd_buffer_aux) {
										free(cmd_buffer_aux);
										cmd_buffer_aux=NULL;
									}
									if(cargar_comandos(cmd_file, &cmd_buffer_aux, keyb[1])==-1) {
										perror("main(): cargar_comandos()");
										exit(1);
									}
									imprimir_comandos(cmd_buffer_aux);
									break;
								case 'e':	/*EJECUTAR COMANDOS.*/
									if(cmd_buffer){
										free(cmd_buffer);
										cmd_buffer=NULL;
									}
									if(cargar_comandos(cmd_file, &cmd_buffer, keyb[1])==-1)	{
										perror("main(): cargar_comandos()");
										exit(1);
									}
									if(instanciar_variables(&cmd_buffer)==-1) {
										perror("main(): instanciar_variables()");
										exit(1);
									}
									break;
							}
						}
					} /*if(entrada_posible).*/
					else { /*Es un comando de control.*/
						switch(keyb[1]) {
							case 'q':	/*SALIR.*/
								b_salir=1; break;
							case 'a':	/*ACTUALIZAR ENTRADAS DEL CMD_FILE.*/
								if(cargar_entradas(cmd_file, vec_entradas, &entradas)==-1) {
									perror("main(): cargar_entradas()");
									exit(1);
								}
								break;
							case 's':	/*DETENER LA EJECUCION.*/
								if(cmd_buffer){
									free(cmd_buffer);
									cmd_buffer=NULL;
								}
								msg("Ejecucion detenida");
								break;
							case '?':								
							case 'h': 	/*MUESTRA LA AYUDA.*/
								ayuda();
								break;
						}
					} /*end if(entrada_posible).*/
				} /*fin if(keyb[0]).*/
				cmdline();
				break;
		} /*fin switch(select).*/
	}
	msg("Fin de la ejecucion de la aplicacion");
	return 0;
}

/*-----------------------------------------------------------------------------
FUNCION: sintaxis(). 
OBJETIVO: Formato de llamada a la aplicacion.

PARAM_IN: void.
PARAM_OUT: void.
-----------------------------------------------------------------------------*/
void sintaxis(void) {
	printf("cmdelay v2004");
	printf("\nSintaxis:");
	printf("\n\tcmdelay [-d <delay>[u]] [-c <cmd_file>] [-e <entrada>]\n\tDonde:\n\t\tdelay: Tiempo de retrazo entre cada comando.\n\t\tu: Postfijo que indica que el tiempo es en microsegundos.\n\t\tcmd_file: Archivo de comandos.\n\t\tentrada: Valor de entrada de comandos a ejecutar.\n");
}

void ayuda(void) {
	printf("Sintaxis -> <valor>: Valor requerido");
	printf("\n\t:a\tActualiza entradas.\n\t:q\tCierra la aplicacion.\n\t:s\tTermina la ejecucion de los comandos actuales\n\t:h\tEste menu de ayuda.");
	printf("\n\t:<entrada>p\tImprime los comandos de la entrada <entrada>.\n\t:<entrada>e\tEjecuta los comandos de la entrada <entrada>.\n");
}
/*-----------------------------------------------------------------------------
FUNCION: cargar_entradas().
OBJETIVO: Carga en el vector las entradas posibles leidas del CMD_FILE.

PARAM_IN: 
	FILE *: Archivo CMD_FILE.
	char *: Vector de entradas.
	unsigned char *: Numero de entradas cargadas.
PARAM_OUT: 
	int: Estado de la funcion.
-----------------------------------------------------------------------------*/
int cargar_entradas(FILE *cmd_file, char *vec_entradas, unsigned char *entradas) {
	long pos_inicial;
	char entrada[5];

	pos_inicial=ftell(cmd_file);
	fseek(cmd_file, 0, SEEK_SET);

	*entradas=0;
	while(fgets(entrada, 5, cmd_file)!=NULL)
		if(strcmp_extendida(entrada, "[*]\n")==0) /*Es una entrada.*/
			if(*entradas<MAX_ENTRADAS) {
				vec_entradas[(*entradas)++]=entrada[1];
				msg("Se ha actualizado una entrada");
			}
			else
				return -1;
	
	fseek(cmd_file, pos_inicial, SEEK_SET);
	
	return *entradas;	
}

/*-----------------------------------------------------------------------------
FUNCION: strcmp_extendida().
OBJETIVO: Compara dos cadenas usando comodines.

PARAM_IN: 
	char *: Cadena fuente.
	char *: Cadena con el formato a comparar ('*'->Indiferente).
PARAM_OUT: 
	int: 0->igual | 1->Distinta.
-----------------------------------------------------------------------------*/
int strcmp_extendida(char *fuente, char *formato) {
	register i;

	for(i=0; i<strlen(fuente) && i<strlen(formato); ++i) {
		if(formato[i]!='*' && fuente[i]!=formato[i])
			return 1;
	}
	return 0;
}

void clrscr(void) {
	/*Borra la pantalla con una secuencia de escape con el formato:
		\e[xxx o en octal \033[xxx.*/
	printf("\033[25A"); /*Situa el cursor al comienzo.*/
	printf("\e[2J"); /*Borra la pantalla.*/
}

void cmdline(void) {
	write(STDOUT, "(cmd) ", 6);
}

void msg(char *msg) {
	write(STDOUT, "\t<MENSAJE>: ", 13);
	write(STDOUT, msg, strlen(msg)); write(STDOUT, ".\n", 2);
}

/*-----------------------------------------------------------------------------
FUNCION: existe_entrada().
OBJETIV: Analiza la existencia de una entrada en el vector de entradas.

PARAM_IN: 
	char *: Vector de entradas.
	unsigned char: Cantidad de entradas.
	char: Entrada.
PARAM_OUT: 
	unsigned char: 0->No existe | 1->Existe.
-----------------------------------------------------------------------------*/
unsigned char existe_entrada(char *vec_entradas, unsigned char num_ent, char entrada) {
	register i;

	for(i=0; i<num_ent; ++i)
		if(vec_entradas[i]==entrada) return 1;
	return 0;
}

/*-----------------------------------------------------------------------------
FUNCION: cargar_comandos().
OBJETIVO: Carga los comandos pertenecientes a una entrada especifica.

PARAM_IN: 
	FILE *: Archivo de comandos.
	char **: Doble puntero a buffer en donde se cargan los comandos(static).
	char: Entrada a buscar.
PARAM_OUT: 
	int: Estado de la funcion.
-----------------------------------------------------------------------------*/
int cargar_comandos(FILE *cmd_file, char **buffer, char entrada) {
	char lectura[MAX_LINEA];
	unsigned char b_cargar=0;
	char cadcmp[5]="[X]\n";
	char *buffer_aux=NULL;
	unsigned char num_bytes=0;
			
	cadcmp[1]=entrada;
	
	fseek(cmd_file, 0, SEEK_SET);
	while(fgets(lectura, MAX_LINEA, cmd_file)!=NULL) {
		if(b_cargar) {
			if(!strcmp_extendida(lectura, "[*]\n")) { /*Termino la entrada.*/
				*buffer=buffer_aux; /*Se cambia el puntero argumento.*/
				return 0;
			}
			else {/*Es un comando a cargar.*/

				/*El +1 es para almacenar el ';'.*/
				if((buffer_aux=(char *) realloc(buffer_aux, num_bytes+strlen(lectura)+1))==NULL)
					return -1;

				if(!num_bytes) /*Si era la primera carga.*/
					buffer_aux[0]='\0'; /*Se vacia.*/
				num_bytes+=strlen(lectura)+1;

				if(lectura[strlen(lectura)-1]=='\n') lectura[strlen(lectura)-1]='\0';
				if(strlen(lectura)>0)
					strcat(buffer_aux, lectura); strcat(buffer_aux, ";");
			}
		}
		else
			if(!strncmp(lectura, cadcmp, 5))
				b_cargar=1;
	}

	*buffer=buffer_aux;
	return 0;
}

void imprimir_comandos(char *comandos) {
	char *ptr_str;
	unsigned char i=0;

	ptr_str=strtok(comandos, ";");
	
	while(ptr_str) {
		printf("\tcomando[%d]:\t%s\n", i++, ptr_str);
		fflush(stdout);
		ptr_str=strtok(NULL, ";");
	}
}

char *buscar_parametros(char argc, char **argv, char offset) {
	if(argv[offset][2]=='\0') {
		if(argc>offset+1 && *argv[offset+1]!='-')
			return argv[offset+1];
		else
			return NULL;
	}
	else if(argv[offset][2]!='-')
		return &(argv[offset][2]);
	
	return NULL;
}
/*-----------------------------------------------------------------------------
FUNCION: instanciar_variables().
OBJETIVO: Asigna un valor a las variables.

PARAM_IN:
	char **: Doble Puntero a la cadena de comandos.
PARAM_OUT: 
	int: Estado de la funcion.
-----------------------------------------------------------------------------*/
int instanciar_variables(char **cmd_buffer) {
	char *indice_car, *indice=*cmd_buffer, *buffer=NULL, lectura[255], ind_var=0;
	unsigned long num_bytes=0;	
	
	if(!*cmd_buffer) return 0;
	
	while((indice_car=index(indice, '$'))!=NULL) {
		if(!num_bytes) {
			/*Rellena con ceros.*/
			if((buffer=(char *) malloc((size_t) indice_car-(size_t) indice))==NULL)
				return -1;
			if((size_t) indice_car- (size_t) indice>=0) buffer[0]='\0';
		}
		else {
			if((buffer=(char *) realloc(buffer, num_bytes+(size_t) indice_car-(size_t) indice))==NULL)
				return -1;
		}
		num_bytes+=(size_t) indice_car-(size_t) indice;

		strncat(buffer, indice, (size_t) indice_car-(size_t) indice);
		printf("\nVariable[%d]= ", ind_var++);
		fgets(lectura, 255, stdin);
		if(lectura[strlen(lectura)-1]=='\n') lectura[strlen(lectura)-1]='\0';

		if((buffer=(char *) realloc(buffer, num_bytes+strlen(lectura)))==NULL)
			return -1;
		num_bytes+=strlen(lectura);
		strncat(buffer, lectura, strlen(lectura));

		indice=indice_car+1;
	}
	
	if(!num_bytes) {
		if((buffer=(char *) malloc((size_t) strlen(indice)))==NULL)
			return -1;
		if(strlen(indice)) buffer[0]='\0';
	}
	else {
  	if((buffer=(char *) realloc(buffer, num_bytes+(size_t) strlen(indice)))==NULL)
			return -1;
	}
	strcat(buffer, indice);

	*cmd_buffer=buffer;
	return 0;
}
/*********************************<< F I N >>*********************************/
