#include <stdio.h>
#include <sys/stat.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include "ls-repl_defs.h"

#define ARR_REDUNDANCY 1
#define FSYSTEM_MAX_NAME 256 //I know about limits.h. But I wanna c99 compatibility (not gnu99)
#define FSYSTEM_MAX_PATH 4096
#define DEFAULT_DIR "./"
#define DIRECTORY_SEPARATOR '/'
#define LU unsigned long int

#define COLLECT_BUFFER 256*16
#define DEFAULT_ELEMENTS_COUNT 10

MEM_PDEF

char *strcpy_last(char *s1, const char *s2) {
	while ((*s1++ = *s2++) != 0) ;
	return s1;
}

void addel(char *fname, LU fsiz) {
	if ( ( coll_data+((DEFAULT_ELEMENTS_COUNT-2)*COLLECT_BUFFER+ARR_REDUNDANCY) ) < coll_da_next ) {
	//I shouldn't add sizeof(TYPE) when seeking on pointer, coz it's already typized. I spend around 2 hours 4 that, LOL
		coll_data = realloc(coll_data, (coll_da_next-coll_data)*3 ); dprintf(2, "\n\nCOLL_DATA_REALLOCED\n\n");
	}
	if ( ( coll_types+(DEFAULT_ELEMENTS_COUNT-2) ) < coll_ty_next) {
		coll_types = realloc(coll_types, (coll_ty_next-coll_types)*3 ); dprintf(2, "\n\nCOLL_TYPES_REALLOCED\n\n");
	}
	if ( ( coll_pseudostruct+(DEFAULT_ELEMENTS_COUNT-2) ) < coll_ps_next) {
		coll_pseudostruct = realloc(coll_pseudostruct, (coll_ps_next-coll_pseudostruct)*3 ); dprintf(2, "\n\nCOLL_PS_REALLOCED\n\n");
	}

	*coll_ps_next = coll_ty_next; coll_ps_next++;
	*coll_ty_next = coll_da_next; coll_ty_next++;
	coll_da_next = strcpy_last(coll_da_next, fname);
	*coll_ty_next = coll_da_next; coll_ty_next++;
	coll_int_next = (LU *) coll_da_next;
	*coll_int_next = fsiz;
	coll_da_next += sizeof(LU);
}

int main(int argc, char **argv) {
	char *dir_addr = DEFAULT_DIR;
	if (argc > 1) dir_addr = argv[1];

	unsigned long int dir_addr_length = strlen(dir_addr);
	char *addr_buffer = malloc(sizeof(char) * (dir_addr_length + FSYSTEM_MAX_NAME + ARR_REDUNDANCY) );
	memcpy(addr_buffer, dir_addr, dir_addr_length);
	if (addr_buffer[dir_addr_length-1] != DIRECTORY_SEPARATOR) {
		addr_buffer[dir_addr_length] = DIRECTORY_SEPARATOR;
		dir_addr_length++;
	}
	char *addr_buffer_file_offset = addr_buffer+dir_addr_length;

	DIR *dir_stream;
	struct dirent *file_entry;
	dir_stream = opendir(dir_addr);
	if (!dir_stream) {
		perror(dir_addr);
		exit(EXIT_FAILURE);
	}

	coll_pseudostruct = malloc(sizeof(void *)*DEFAULT_ELEMENTS_COUNT);
	coll_types = malloc(sizeof(void *)*DEFAULT_ELEMENTS_COUNT);
	coll_data = malloc(sizeof(char)*DEFAULT_ELEMENTS_COUNT*COLLECT_BUFFER+ARR_REDUNDANCY);

	coll_ps_next = coll_pseudostruct;
	coll_pseudostruct_backup = coll_pseudostruct;
	coll_ty_next = coll_types;
	coll_da_next = coll_data;

	struct stat file_stat;
	long int bytes_size;

	while ( file_entry = readdir(dir_stream) ) {
		bytes_size = 0;
		strcpy(addr_buffer_file_offset, file_entry->d_name);
		if ( stat(addr_buffer, &file_stat) != -1 ) bytes_size = file_stat.st_size;
		//addel(file_entry->d_name, (LU) bytes_size); //I dont even care about default LONGWORD MACRO, which is
		//deeply hidden and not redefined for this type. (__STD_TYPE __OFF_T_TYPE __off_t;)
	}
	closedir(dir_stream);
	free(addr_buffer);

	addel("ПЫЩЬПЫЩЬ", 15);
	addel("ЛООООООООООООООООООООЛ", 33);
	addel("зз", 4444);
	addel("YAYAYAAY", 3);
	addel("zzzz", 15);
	addel("aaaaaaaaa", 123);
	addel("sadsdasdasddasasdasd", 54321);
	addel("32489028403928490233284", 10);

	while (coll_pseudostruct < coll_ps_next) {
		printf("%s - %lu\n", coll_pseudostruct[0][0], * (LU *) coll_pseudostruct[0][1]);
		coll_pseudostruct++;
	}
	coll_pseudostruct = coll_pseudostruct_backup;

//	free(coll_pseudostruct);
//	free(coll_types);
//	free(coll_data);

	return EXIT_SUCCESS;
}
