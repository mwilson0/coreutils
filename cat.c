#include<stdio.h>
#include<getopt.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>

// cat clone 0.1.1. matthew wilson. july 2015. 
// based on cat from GNU Coreutils 8.23. 
// currently accepts files on command line to cat. stdin in the future.
// License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>

int leftover=0;
int n=0, e=0, b=0;

thefunc(int argcc, char*argvv[]) {
int x;
int y=0;
char* items[999];

// catting many files
// existing files now in items[y]

for (x=0; x<leftover; x++) {
	if (access(argvv[x], F_OK|R_OK)==-1) {
		printf("no such file: %s\n", argvv[x]);
	}
	else {
		items[y]=malloc(strlen(argvv[x])+1);
		strcpy(items[y], argvv[x]);
		y++;
	}
}

int linecnt=0; // lines of each file
int metalinecnt=1; // total lines of many files
FILE *inputfile;
char *lineitems[9999];
char line[9999];
int blnk=0;

for (x=0; x<y; x++) {
	inputfile=fopen(items[x], "r");
		while (fgets(line, sizeof line, inputfile)!=NULL) {
			lineitems[linecnt]=strdup(line);
				if (b==1) {
					if (strcmp(lineitems[linecnt], "\n")==0) {
						blnk++;
							if (e==1) {
								printf("$");
							}		
						printf("\n");
						continue;
					}
				}
			//remove newline
			lineitems[linecnt][strlen(lineitems[linecnt])-1]='\0';
				if (b==1 && e==1) {
					printf("%6d  %s$\n", metalinecnt, lineitems[linecnt]); 
				}
				else if (e==1 && n==0) {
					printf("%s$\n", lineitems[linecnt]); 
				}
				else if (e==1 && n==1) {
					printf("%6d  %s$\n", metalinecnt, lineitems[linecnt]); 
				}
				else if (n==1 || b==1) {
                			printf("%6d  %s\n", metalinecnt, lineitems[linecnt]); 
				}
				else {
					printf("%s\n", lineitems[linecnt]); 
				}
			linecnt++;
			metalinecnt++;
		}
	fclose(inputfile);
}
// printf("blank: %d\n", blnk); // number of blank lines
exit(0);
}

static int flag_help;

static struct option const long_options[] = 
{
    	{"numlines", no_argument, 0, 'n'}, 
    	{"dollarsign", no_argument, 0, 'E'}, 
    	{"nonemptylines", no_argument, 0, 'b'},
	{"help", no_argument, &flag_help, 1}, 
    	{0, 0, 0, 0}			
};

usage() {
printf("usage: 'cat file1 file2 [-nEb]'\n");
exit(1);
}

main (int argc, char* argv[]) {
int optc;
int index;

while ((optc = getopt_long (argc, argv, "nEb", long_options, (int *) 0)) !=EOF) {
	switch (optc) {
	    case 0:
		break; 
	    case 'n':
	        n=1;
		break;
	    case 'E':
	        e=1;
		break;
	    case 'b':
		b=1;
		break;	
	    case '?':
		usage();
		break;
	    default:	
		usage();
		break;
	}
}

if (flag_help) {
	usage();
}

for (index=optind; index<argc; index++) {
	;
}

argc -= optind;
argv += optind;
leftover=index-optind;

if (leftover>=1) {
	thefunc(argc, argv);
}

else {
 	usage();
}

}
