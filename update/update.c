#include <sys/types.h>
#include <sys/stat.h>

#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define DEBUG_0(fmt,args...) printf("%s(%d)-%s->"fmt, \
                __FILE__, __LINE__, __func__, ##args)
#define DEBUG_ERR(fmt,args...) printf("[ERROR]%s(%d)-%s->"fmt, \
                __FILE__, __LINE__, __func__, ##args)
typedef void (*func_file_code)(const char *fine);

void get_file_path(const char *path, const char *file_name,  char *file_path)
{
    strcpy(file_path, path);
    if(file_path[strlen(path) - 1] != '/')
        strcat(file_path, "/");
    strcat(file_path, file_name);
}

bool is_special_dir(const char *path)
{
    return strcmp(path, ".") == 0 || strcmp(path, "..") == 0;
}
void process_dir(const char *path, func_file_code ffc, void *ptr)
{
        DIR *dir;
        struct dirent *entry;
        char file_path[PATH_MAX], filename[PATH_MAX], dest[PATH_MAX], 
	     *src = NULL, buf[255] = {0}, ch;
	struct stat statbuf;
	int type = 0, i = 0;
	FILE *Fdest = NULL;
	FILE *Fsrc = NULL;

	sprintf(dest, "%s/index.html", path);
	Fdest = fopen(dest, "w");
	if (ptr != NULL) {
		src = (char *)ptr;
		Fsrc = fopen(src, "r");
		if (NULL != Fsrc) {
			while ((ch = fgetc(Fsrc)) != EOF) {
				fputc(ch, Fdest);
			}
			fclose(Fsrc);
		}
	} else {
	}

	if((dir = opendir(path)) == NULL)
		return;
	while((entry = readdir(dir)) != NULL) {
		get_file_path(path, entry->d_name, file_path);
		if(is_special_dir(entry->d_name))
			continue;
		if(lstat(file_path, &statbuf) != 0) {
			continue;
		}
		type = statbuf.st_mode & 0xE000;
		switch (type) {
			case S_IFREG:
				if (i == 0) {
					sprintf(buf, 
						"<br><a href='%s'>%.2d.%s</a>",
							entry->d_name,
						      i++, entry->d_name);
				} else {
					sprintf(buf, 
						"<a href='%s'>%.2d.%s</a>",
							entry->d_name,
						      i++, entry->d_name);
				}
				DEBUG_0("file :reg\n");
				break;
			case S_IFDIR:
				process_dir(file_path, ffc, ptr);
				sprintf(buf, 
			"<a href='%s/index.html'>IDR:%s</a>",
						entry->d_name,
					      entry->d_name);
				DEBUG_0("file :dir\n");
				break;
			default:
				DEBUG_ERR("%s:%d\n", file_path, type);
				break;
		}
		fwrite(buf, strlen(buf), 1, Fdest);
		fwrite("<br>\n", 5, 1, Fdest);
	}
	fwrite("</body>\n", 8, 1, Fdest);
	closedir(dir);
	fclose(Fdest);
}
void main(int argc, char **argv)
{
	char *p = "pre.html";
	p = NULL;
	if (argc > 1) {
		process_dir(argv[1], NULL, p);
	} else {
		printf("Usage: update [PATH]\n");
	}
}
