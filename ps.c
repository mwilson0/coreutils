#include<stdio.h>
#include<sys/stat.h>
#include<unistd.h>
#include<dirent.h>
#include<string.h>
#include<stdlib.h>
#include<ctype.h>

// ps clone. 


struct stat fileStat;
char* theprocdir = "/proc";

DIR *pDir;
struct dirent *pDirent;

char* items[5096]; //files listed in proc
int x=0;

int main(int argc, char* argv[]) {

// check and open dir
stat(theprocdir, &fileStat);

if (access(theprocdir, F_OK|R_OK) != -1) {
	//printf("readable\n");

if (S_ISDIR (fileStat.st_mode)) {
	//printf("is a dir\n");

pDir=opendir(theprocdir);

while ((pDirent=readdir(pDir)) != NULL) {

	// first skip over . and .. and files that are not dirs
	// need *before var name in isdigit
	if (( 
		(! isdigit(*pDirent->d_name))  ||
		strcmp(pDirent->d_name, ".") == 0 ||
		strcmp(pDirent->d_name, "..") == 0 ||
		pDirent->d_name[0] == '.') ||
		(pDirent->d_type !=  DT_DIR) ) {
			continue;
	}

	items[x]=malloc(strlen(pDirent->d_name)+1);
	strcpy(items[x], pDirent->d_name);
	//printf("%d: %s\n", x, items[x]);
	x++;
}
closedir(pDir);
}

}

// next would ideally:
// open each dir, check for the files with the info i want, 
// and list them if they are there
int i;

//char* dirslah = "/";
char* file1 = "status";
char* file2 = "cmdline";

char embuff[99];

FILE* fp;

int linecnt=0;
char line[300];
char* lineitems[40];

for (i=0; i<x; i++) {
	
snprintf(embuff, sizeof embuff, "%s%s%s%s%s", theprocdir, "/", items[i], "/", file1);
printf("%s\n", embuff);

//if (access(embuff, F_OK|R_OK) == -1) {
//	printf("%s not found\n", embuff);
//	}

	// open the file, count the lines

	fp=fopen(embuff, "r");
	
	while (fgets(line, sizeof line, fp) != NULL) {
		lineitems[linecnt]=strdup(line);	
		linecnt++;
	}

	int a=0;

	for (a=0; a<6; a++) {
		printf("%s", lineitems[a]);
	}  	




fclose(fp);
linecnt=0;

} 

// and now search each text file for info i want







}
