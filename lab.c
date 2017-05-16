#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <malloc.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <dirent.h>
#include <locale.h>
#include <time.h>
#include <libgen.h>

#define STRING_SIZE 512

FILE *file;
char *script_name;
int count_dir = 0;
int count_files = 0;

void print_error(const char *scr_name, const char *msg_err, const char *f_name)
{
    fprintf(stderr, "%s: %s: %s\n", scr_name, msg_err, (f_name) ? f_name : NULL);
}

void get_properties(struct stat buf, char *filepath)
{
	char *string;
	struct 	tm lt;
	string = (char *)malloc(sizeof(char)*STRING_SIZE);
	localtime_r(&buf.st_mtime, &lt);
	strftime(string, sizeof(char*)*STRING_SIZE, "%d %b %Y", &lt);				
	fprintf(file,"\n%s %ld %s %ld ", filepath, buf.st_size, string, (long)buf.st_ino);
	fprintf(file, (buf.st_mode & S_IRUSR) ? "r" : "-");
	fprintf(file, (buf.st_mode & S_IWUSR) ? "w" : "-");
	fprintf(file, (buf.st_mode & S_IXUSR) ? "x" : "-");
	fprintf(file, (buf.st_mode & S_IRGRP) ? "r" : "-");
	fprintf(file, (buf.st_mode & S_IWGRP) ? "w" : "-");
	fprintf(file, (buf.st_mode & S_IXGRP) ? "x" : "-");
	fprintf(file, (buf.st_mode & S_IROTH) ? "r" : "-");
	fprintf(file, (buf.st_mode & S_IWOTH) ? "w" : "-");
	fprintf(file, (buf.st_mode & S_IXOTH) ? "x" : "-");
	free(string);	
}

int process_directory(char *folder, char *filename, int *file_exist)
{
	DIR *selected_dir;
	struct dirent *dir;
	struct stat buf;
	char *fullpath, *filepath;
	setlocale(LC_TIME,"ru_RU.UTF-8");

	filepath = (char *)malloc(sizeof(char)*STRING_SIZE);

	if ((selected_dir = opendir(folder)) == NULL)
		print_error(script_name, strerror(errno), realpath(folder, filepath));
    else{
	while((dir = readdir(selected_dir)) != NULL) {
		if (dir->d_type != DT_DIR) {
			if (dir->d_type == DT_REG) 
				count_files++;

			if (strcmp(filename, dir->d_name) == 0) {

				if (realpath(folder, filepath) != NULL) {	
	            	strcat(strcat(filepath, "/"), filename);
	            }
	            else { print_error(script_name, strerror(errno), filepath); }
				
				if ((stat(filepath, &buf)) == 0 )
				{
					if (*file_exist == 1)
					{						
						if ((file = fopen("result.txt", "a")) != NULL) {
							get_properties(buf, filepath);
							fclose(file);
						}
						else { print_error(script_name, strerror(errno), filepath);}
					}
					else
					{
						if ((file = fopen("result.txt", "w")) != NULL) {
							get_properties(buf, filepath);
							fclose(file);
							*file_exist = 1;
						}
						else { print_error(script_name, strerror(errno), filepath);}
					}
				}
				else {
					fprintf(stderr, "%s : %s : %s", script_name, strerror(errno), filepath); continue;
				}		
			}
		}
		else {
			if (((dir->d_type == DT_DIR)) && ((strcmp(dir->d_name,".")) != 0) && ((strcmp(dir->d_name,"..")) != 0)) {
				count_dir++;
				if (realpath(folder, filepath) != NULL) {
					strcat(strcat(filepath,"/"), dir->d_name);
				}
				else { print_error(script_name, strerror(errno), realpath(folder, fullpath)); continue; }

            	process_directory(filepath, filename, file_exist);
			}
		}
	}

	if (closedir(selected_dir) == -1){
		print_error(script_name, strerror(errno), realpath(folder, fullpath));
    }
}
	free(filepath);
}

void print_result(int flag)
{
	int c;
	if (flag == 1) {
		if ((file = fopen("result.txt", "r")) != NULL) {
			while(1)
			{
				c = fgetc(file);
				if ( feof(file) )
					break;
				printf("%c", c);
			}
			fclose(file);
		}
		else
			print_error(script_name, strerror(errno), "result.txt");
	}
	else 
		fprintf(stderr, "%s : Файл не существует\n", script_name);
}

int main(int argc, char *argv[])
{
	char *dir_name, *file_name;
	script_name = basename(argv[0]);
	dir_name = argv[1];
	file_name = argv[2];
	int flag = 0;

	process_directory(dir_name, file_name, &flag);
	print_result(flag);

	printf("\n%d %d\n", count_dir, count_files);
	return 0;
}
