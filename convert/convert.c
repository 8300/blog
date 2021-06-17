#include <stdio.h>
#include <string.h>

int main(int argc, char **argv)
{
	FILE *fp0 = NULL, *fp1 = NULL;
	char *filename = NULL, *tmp, *tmp2;
	char name2[128], buf[1024];

	if (argc > 1) {
		filename = argv[1];
	} else {
		printf("Usage: convert [filename]\n");
	}
	fp0 = fopen(filename, "r");
	if (fp0 == NULL) {
		printf("no this file\n");
		return -1;
	}
	strcpy(name2, strtok(filename, "."));
	sprintf(name2, "%s.html", name2);
	printf("create file:%s\n", name2);
	fp1 = fopen(name2, "w");
	do {
		tmp = fgets(buf, 1024, fp0);
		if (tmp != NULL) {
			tmp2 = strchr(buf, '<');
			if (tmp2 != NULL)
				memcpy(tmp2, "&lt", 3);
			tmp2 = strchr(buf, '>');
			if (tmp2 != NULL)
				memcpy(tmp2, "&gt", 3);
			tmp2 = strchr(buf, '\n');
			if (tmp2 != NULL)
				sprintf(tmp2, "<br>\n");
//			sprintf(buf, "%s<br>", buf);
			fwrite(buf, 1, strlen(buf), fp1);
		}
	} while (tmp);
	fclose(fp0);
	fclose(fp1);
}
