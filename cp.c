#include<stdio.h>
#include<getopt.h>
#include<stdlib.h>
#include<string.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<unistd.h>
#include<fcntl.h>
#include<time.h>
#define BUF_SIZE 9999

// cp clone 0.1. matthew wilson. july 2015. 
// based on cp from GNU Coreutils 8.2. No warranty.
// License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>

int leftover=0;
int i=0, v=0, u=0;

thefunc(int argcc, char*argvv[]) {
printf("i=%d v=%d u=%d\n", i, v, u);

int x;
struct stat fileStat;
time_t raw[2];
int c;
ssize_t file_in, file_out;
char buffer[BUF_SIZE];
int sourcefile, destfile;

// test source file
sourcefile=open(argvv[0], O_RDONLY);

if (sourcefile==-1) {
	printf("cannot stat '%s': No such file or directory\n", argvv[0]);
	exit(1);
}

// check if dest file exists
if (access(argvv[1], F_OK|R_OK|W_OK) != -1) {
	if (i==1 & u!=1) { // interactive mode
		printf("overwrite '%s'? ", argvv[1]);
		scanf("%s", &c);	
		if (c!='y') {
			exit(0);
		}
	}

	// update mode. compare times of source and dest files
	if (u==1) {
		for (x=0; x<leftover; x++) {
		stat(argvv[x], &fileStat);
		raw[x]=fileStat.st_mtime;
		}
		if (raw[0] < raw[1]) {
			printf("no overwrite (^.^)\n");
			exit(0);
		}
	}
}

destfile=creat(argvv[1], 0666);

while ((file_in=read(sourcefile, &buffer, BUF_SIZE)) >0 ) {
	file_out=write(destfile, &buffer, (ssize_t) file_in);
		if (file_out != file_in) {
			printf("write error\n");
			exit(1);
		}
}

// verbose mode
if (v==1) {
	printf("%s -> %s\n", argvv[0], argvv[1]);
}

close(sourcefile); close(destfile);

exit(0);
}

static int flag_help;

static struct option const long_options[] = 
{
	{"interactive", no_argument, 0, 'i'}, 
    	{"verbose", no_argument, 0, 'v'}, 
    	{"update", no_argument, 0, 'u'},
	{"help", no_argument, &flag_help, 1}, 
   	{0, 0, 0, 0}			
};

usage() {
printf("usage: cp [source file] [dest file] [-ivu]\n");
exit(1);
}

main (int argc, char* argv[]) {
int optc;
int index;

while ((optc = getopt_long (argc, argv, "ivu", long_options, (int *) 0)) !=EOF) {
	switch (optc) {
	   case 0:
		break; 
	   case 'i':
	        i=1;
		break;
	    case 'v':
	        v=1;
		break;
	    case 'u':
	        u=1;
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

if (argc==1) {
	printf("cp: missing file operand\n");
	usage();
}

else if (argc>1) {
	for (index=optind; index<argc; index++) {
		;
	}

	argc -= optind;
	argv += optind;
	leftover=index-optind;

if (leftover==1) {
	printf("cp: missing destination file operand after '%s'\n", argv[0]);
	usage();
}

if (leftover==2) {
	thefunc(argc, argv);
}

if (leftover>2) {
	usage();
}

}


}
