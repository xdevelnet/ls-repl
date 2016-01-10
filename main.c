#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <errno.h>
#include <iso646.h>
#include <string.h>
#include <dirent.h>

#define TRUE 1 // C89 compat, I dont like to #include <stdbool.h>
#define FALSE 0

#define LU unsigned long int
#define DEFAULT_DIR "./"
#define CURRENT_DIR "."
#define UPPER_DIR ".."
#define DIRECTORY_SEPARATOR '/' //it's still cross platform
#define FSYSTEM_MAX_NAME 256 //standart "types" header isn't always accurate

#define MEM4K 1024
#define MEM4K_reserve 300
#define PSEUDOST_TYPES 2
#define PSEUDOST_ELEMS 77
#define PSEUDOST_reserve 3
#define ARR_REDUNDANCY 1
size_t PSEUDOST_instances = 0;

size_t data_coll_size = MEM4K;
size_t ptr_coll_size = PSEUDOST_TYPES * PSEUDOST_ELEMS;
size_t data_collector_seek = 0;
size_t ptr_collector_seek = 0;
char *data_collector;
char **ptr_collector;

char *empty_string = "";
char write_lock_flag = FALSE;

void print_error_if_needed (const char *s) {
	if (errno) perror(s);
}

char *strcpy_last(char *s1, const char *s2) {
	while ((*s1++ = *s2++) != 0) ;
	return s1;
}

void more_space_if_needed() {
	if (TRUE == write_lock_flag) return;
	if (data_coll_size-MEM4K_reserve < data_collector_seek) {
		data_coll_size*=2;
		data_collector = realloc(data_collector, data_coll_size);
	}
	if (ptr_coll_size-PSEUDOST_reserve*PSEUDOST_TYPES < ptr_collector_seek) {
		ptr_coll_size*=2;
		ptr_collector = realloc(ptr_collector, sizeof(void *) * ptr_coll_size);
	}
	if (NULL == data_collector or NULL == ptr_collector) {
		write_lock_flag = TRUE;
		//just hold old data untouchable and don't let function to write more
		print_error_if_needed(empty_string); errno = FALSE;
		//we're already showed error. No reason to hold errno longer
	}
}

void data_add(char *str, LU sz) {
	if (write_lock_flag != TRUE) {
		ptr_collector[ptr_collector_seek++] = (void *) data_collector_seek;
		data_collector_seek = strcpy_last(data_collector+data_collector_seek, str)-data_collector;
		ptr_collector[ptr_collector_seek++] = (void *) data_collector_seek;
		LU *lu_assign;
		lu_assign = (LU *) (data_collector+data_collector_seek);
		*lu_assign = sz;
		data_collector_seek+=sizeof(LU);
		more_space_if_needed();
	}
}

void seek_to_ptr(char *anchor_point, char **target, size_t iterations) {
	size_t i=0;
	while (i <= iterations) {
		target[i] = anchor_point + (size_t) target[i];
		i++;
	}
}

int comparator(const void *a, const void *b) {
	const char **ia = * (const char ***) a;
	const char **ib = * (const char ***) b;
	return strcmp(*ia, *ib);
}


int main(int argc, char **argv) {
	char *dir_addr = DEFAULT_DIR;
	if (argc > 1) dir_addr = argv[1];

	size_t dir_addr_length = strlen(dir_addr);
	char *addr_buffer = malloc(sizeof(char) * (dir_addr_length + FSYSTEM_MAX_NAME + ARR_REDUNDANCY) );
	char *addr_buffer_file_offset = strcpy_last(addr_buffer, dir_addr) -1;
	if ( DIRECTORY_SEPARATOR != *(addr_buffer_file_offset-1)) *(addr_buffer_file_offset++) = DIRECTORY_SEPARATOR;

	DIR *dir_stream;
	struct dirent *file_entry;
	dir_stream = opendir(dir_addr);
	if (!dir_stream) {
		print_error_if_needed(empty_string);
		exit(EXIT_FAILURE);
	}
	data_collector = malloc(MEM4K);
	ptr_collector = malloc(sizeof(void *)*PSEUDOST_TYPES*PSEUDOST_ELEMS);

	struct stat file_stat;
	long int bytes_size;

	while ( file_entry = readdir(dir_stream) ) {
		bytes_size = 0;
		PSEUDOST_instances++;

		if ( !(strcmp(file_entry->d_name, CURRENT_DIR) and strcmp(file_entry->d_name, UPPER_DIR)) ) {
			bytes_size = 4096; //no reason to call stat() if it changes errno and we already know what it will return
		} else {
			strcpy(addr_buffer_file_offset, file_entry->d_name);
			if ( stat(addr_buffer, &file_stat) != -1 ) bytes_size = file_stat.st_size;
		}
		data_add(file_entry->d_name, (LU) bytes_size); //I dont even care about default LONGWORD MACRO, which is
		//deeply hidden and not redefined for this type. (__STD_TYPE __OFF_T_TYPE __off_t;)
	}
	closedir(dir_stream);
	free(addr_buffer);

	seek_to_ptr(data_collector, ptr_collector, PSEUDOST_instances*PSEUDOST_TYPES);

	char ***pseudostruct = calloc(PSEUDOST_instances, sizeof(void *));

	if (pseudostruct == NULL) {
		print_error_if_needed(empty_string);
		exit(EXIT_FAILURE);
	}

	size_t i=0;
	while (i < PSEUDOST_instances) {
		pseudostruct[i] = ptr_collector+i*PSEUDOST_TYPES;
		i++;
	}
	qsort(pseudostruct, PSEUDOST_instances, sizeof(void *), comparator);
	i=0;
	while (i < PSEUDOST_instances) {
		printf("%s - %lu\n", pseudostruct[i][0], * (LU *) pseudostruct[i][1]);
		i++;
	}

	return EXIT_SUCCESS;
}
