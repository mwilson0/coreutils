#include<stdio.h>
#include<sys/stat.h>
#include<unistd.h>
#include<dirent.h>
#include<string.h>
#include<stdlib.h>
#include<ctype.h>
#include<pwd.h>


// ps clone. 


struct stat fileStat;
char* theprocdir = "/proc";

DIR *pDir;
struct dirent *pDirent;

struct passwd *pwd;

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
char dispbuf[150];


FILE* fp;

int linecnt=0;
char line[300];
char* lineitems[45];


printf("  PID  NAME             USER          STATE\n");
printf("-----  ---------------  ------------  ----------\n");

for (i=0; i<x; i++) {
	
snprintf(embuff, sizeof embuff, "%s%s%s%s%s", theprocdir, "/", items[i], "/", file1);
//printf("%s\n", embuff);

//if (access(embuff, F_OK|R_OK) == -1) {
//	printf("%s not found\n", embuff);
//	}

	// open the file, count the lines

	fp=fopen(embuff, "r");
	
	while (fgets(line, sizeof line, fp) != NULL) {
		lineitems[linecnt]=strdup(line);	
		linecnt++;
	}

	
	//int a;
	//for (a=0; a<8; a++) {
	//	printf("%d: %s", a, lineitems[a]);
	//}  	

//snprintf(dispbuf, sizeof dispbuf, "%5s  %s%s", items[i], strtok(&lineitems[0][6], "\n"), &lineitems[1][6]);
//printf("%s", dispbuf);
	

	printf("%5s  ", items[i]); // pid
	printf("%-17s", strtok(&lineitems[0][6], "\n")); //progname
	


// copy the user line into newstring and tokenize at first space
char newstring[30];
char* delim = "\t";
char* uuid;
char* uname;

strncpy(newstring, &lineitems[7][5], sizeof newstring - 1);
// get the userID and covert to an int
uuid = strtok(newstring, delim);
int q = atoi(uuid);
// now test it using pwd struct
if (getpwuid(q) != NULL) { 
	pwd=getpwuid(q); 
	printf("%-13s", pwd->pw_name); //userID num
}


printf(" %s", &lineitems[1][7]); // state of process


fclose(fp);
linecnt=0;

} 


}
