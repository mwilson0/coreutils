#include<stdio.h>
#include<sys/stat.h>
#include<unistd.h>
#include<dirent.h>
#include<string.h>
#include<stdlib.h>
#include<ctype.h>
#include<pwd.h>

int firstfunc();

// ps clone. 

struct stat fileStat;
char* theprocdir = "/proc";
DIR *pDir;
struct dirent *pDirent;
struct passwd *pwd;
char* items[5096]; //files listed in proc
int x=0;

int firstfunc() {

// check and open dir
stat(theprocdir, &fileStat);

if (access(theprocdir, F_OK|R_OK) != -1) {
	if (S_ISDIR (fileStat.st_mode)) {
		pDir=opendir(theprocdir);

		while ((pDirent=readdir(pDir)) != NULL) {

			// first skip over . and .. and files that are not dirs
			// need *before var name in isdigit to open only numbered dirs
			if ( ( (! isdigit(*pDirent->d_name))  ||
				strcmp(pDirent->d_name, ".") == 0 ||
				strcmp(pDirent->d_name, "..") == 0 ||
				pDirent->d_name[0] == '.') ||
				(pDirent->d_type !=  DT_DIR) ) {
					continue;
			}
			items[x] = malloc(strlen(pDirent->d_name) + 1);
			strcpy(items[x], pDirent->d_name);
			//printf("%d: %s\n", x, items[x]);
			x++;
		}	
	closedir(pDir);
	}
}
else {
	printf("error unable to read: %s\n", theprocdir);
	exit(1);
}

// open each dir, check for the files with the info i want

int i;
char* file1 = "status";
char* file2 = "cmdline";

char embuf[99];
char dispbuf[200];
char* disparray[x];

FILE* fp;

int linecnt=0;
char line[300];
char* lineitems[45];


printf("  PID  NAME              USER          STATE\n");
printf("-----  ----------------  ------------  ------------\n");

for (i = 0; i < x; i++) {
	// construct the path: /proc/<PID>/status
	snprintf(embuf, sizeof embuf, "%s%s%s%s%s", theprocdir, "/", items[i], "/", file1);
	//printf("%s\n", embuf);

	if (access(embuf, F_OK|R_OK) == -1) {
		printf("%5s not found\n", embuf);
		continue;
	}
	else {

	// open the file, count the lines

	fp = fopen(embuf, "r");
	
	while (fgets(line, sizeof line, fp) != NULL) {
		lineitems[linecnt]=strdup(line);	
		linecnt++;
	}

	if (! linecnt > 0) {
		printf("%5s  unable to read file for process\n", items[i]);
		continue;
	}

	// copy the user line into newstring and tokenize at first space
	char newstring[30];
	char* delim = "\t";
	char* uuid;
	
	strncpy(newstring, &lineitems[7][5], sizeof newstring - 1);
	// get the userID and covert to an int
	uuid = strtok(newstring, delim);
	int q = atoi(uuid);
	// now test it using pwd struct
	if (getpwuid(q) != NULL) { 
		pwd = getpwuid(q); 
		//printf("%-13s", pwd->pw_name); //userID num
	}
	//printf(" %s", &lineitems[1][7]); // state of process

	// put everything into a single array and print it

	snprintf(dispbuf, sizeof dispbuf, "%5s  %-18s%-13s %s", items[i], strtok(&lineitems[0][6], "\n"), pwd->pw_name, &lineitems[1][7]);
	disparray[i] = malloc(strlen(dispbuf) + 1);
	strcpy(disparray[i], dispbuf);
	printf("%s", disparray[i]);
	fclose(fp);
	linecnt=0;
	}

} // end of loop

} // end of function

int main(int argc, char* argv[]) {

firstfunc();

exit(0);
}
