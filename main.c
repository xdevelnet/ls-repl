#include <stdio.h>
#include <sys/stat.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#define ADDR_REDUNDANCY 1
#define DEFAULT_DIR "./"
#define DIRECTORY_SEPARATOR '/'

int main(int argc, char **argv) {
	char *dir_addr = DEFAULT_DIR;
	if (argc > 1) dir_addr = argv[1];

	unsigned long int dir_addr_length = strlen(dir_addr);
	char *addr_buffer = malloc(sizeof(char) * (dir_addr_length + NAME_MAX + ADDR_REDUNDANCY) );
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

	struct stat file_stat;
	long int bytes_size;

	while ( file_entry = readdir(dir_stream) ) {
		bytes_size = 0;
		unsigned long int filename_length = strlen(file_entry->d_name);
		memcpy(addr_buffer_file_offset, file_entry->d_name, filename_length);
		addr_buffer_file_offset[filename_length] = 0;
		if ( stat(addr_buffer, &file_stat) != -1 ) bytes_size = file_stat.st_size;
		printf("%s - %li\n", file_entry->d_name, bytes_size);
	}
	closedir(dir_stream);

	return EXIT_SUCCESS;
}
