#include<stdio.h>
#include<getopt.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>

// wc clone 0.1.2 matthew wilson. oct 2015. 
// based on wc from GNU Coreutils 8.23. No warranty. 
// License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>

int leftover=0;
int l=0, w=0, m=0;
int lines=0;
int words=0;
int charz=0;
int linesT[999];
int wordsT[999];
int charzT[999];
int counter=0;

// printing function
display() {
if (l == 1) {
	printf("lines: %d ", lines);
	if (w != 1 && m != 1) {
		printf("\n");
	}
}

if (w == 1) {
	printf("words: %d ", words);
	if (m != 1) {
		printf("\n");
	}
}

if (m == 1) {
	printf("chars: %d\n", charz);
}

else if ((l == 0 && w == 0 && m == 0) || (l == 1 && w == 1 && m == 1)) {
	printf("lines: %d words: %d chars: %d\n", lines, words, charz);
}

linesT[counter]=lines; // array of totals[counter]
wordsT[counter]=words;
charzT[counter]=charz;

if (leftover > 1) { // reset the values
	lines=0;
	words=0;
	charz=0;
}

// when this counter matches number of argument vectors, print totals one-time only
if (counter + 1 == leftover) {
	int s;
	int aaa=0;
	int bbb=0;
	int ccc=0;

	for (s=0; s < counter + 1; s++) {
        	aaa=aaa+linesT[s];
        	bbb=bbb+wordsT[s];
        	ccc=ccc+charzT[s];
        }

	if (leftover > 1) {
		printf("total: ");
        	if (l == 1) {
        		printf("lines: %d ", aaa);
                	if (w != 1 && m != 1) {
                       		 printf("\n");
                	}
		}	
		if (w == 1) {
        		printf("words: %d ", bbb);
                	if (m != 1) {
                        	printf("\n");
                	}
		}
		if (m == 1) {
        		printf("chars: %d\n", ccc);
		}
		else if ((l == 0 && w == 0 && m == 0) || (l == 1 && w == 1 && m == 1)) {
			printf("lines: %d words: %d chars: %d\n", aaa, bbb, ccc);
		}		
	}
}

counter++;
}

// reading from stdin
noargs() {
int c;

while ((c=getchar()) != EOF) {
	++charz;
	if (c == '\n') {
		++lines;
	}
	if (isspace(c) || c == '\t' || c == '\n') {
		++words;
	}
}
display();
exit(0);
}

thefunc(int argcc, char* argvv[]) {
int x;
int y=0;
char* items[999];
// all exisiting files in items[y]

for (x=0; x < leftover; x++) {
	if (access(argvv[x], F_OK) == -1) {
		printf("wc: '%s': no such file\n", argvv[x]);
		continue;
	}
	else {
		items[y]=malloc(strlen(argvv[x])+1);
		strcpy(items[y], argvv[x]);
		y++;
	}
}

// now count all words, chars and lines in each file
int c;
FILE* fp;
char* thefile;

for (x=0; x < y; x++) {
	thefile=strdup(items[x]);
	if (!(fp=fopen(thefile, "r"))) {
		printf("file error\n");
	}
	else {
		while ((c=fgetc(fp)) != EOF) {
			++charz;
			if (c == '\n') {
				++lines;
			}	
			if (c == '\t' || c == '\n' || c == ' ') {
				++words;
			}
		}		
	printf("%s: ", items[x]);
	display(); // printing function
	}
	fclose(fp);
}

exit(0);
}

static int flag_help;

static struct option const long_options[] = {
	{"lines", no_argument, 0, 'l'}, 
    	{"words", no_argument, 0, 'w'}, 
    	{"chars", no_argument, 0, 'm'},
	{"help", no_argument, &flag_help, 1}, 
   	{0, 0, 0, 0}			
};

usage() {
printf("usage: wc [file] or stdin [-lwm]\n");
exit(1);
}

main (int argc, char* argv[]) {
int optc;
int index;

while ((optc = getopt_long (argc, argv, "lwm", long_options, (int *) 0)) != EOF) {
	switch (optc) {
	    case 0:
	        break; 
	    case 'l':
	        l=1;
		break;
	    case 'w':
		w=1;
		break;
	    case 'm':
		m=1;
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
	printf("wc - count lines, words, chars\n");
	usage();
}

if (argc == 1) {
	noargs(argc, argv);
}

else if (argc > 1) {
	for (index=optind; index<argc; index++) {
		;
	}

	argc -= optind;
	argv += optind;
	leftover=index-optind;

	if (leftover >= 1) {
		thefunc(argc, argv);
	}
	if (leftover == 0) {
		noargs(argc, argv);
	}
}

}
