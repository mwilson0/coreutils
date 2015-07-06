#include<stdio.h>
#include<getopt.h>
#include<stdlib.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<unistd.h>

// mkdir clone 0.1 matthew wilson. july 2015. No Warranty. 
// based on mkdir from GNU Coreutils 8.4.
// License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>

int leftover=0;
int v=0;

thefunc(int argcc, char*argvv[]) {
int x;

for (x=0; x<leftover; x++) {
	if (mkdir(argvv[x], S_IRWXU|S_IRWXG|S_IROTH|S_IXOTH !=0)) {
		if (access(argvv[x], F_OK)!=-1) {
			printf("cannot create directory '%s'. ", argvv[x]);
			printf("file already exists.\n");
		}
		else {
			printf("unable to create dir: %s\n", argvv[x]);
		}
	}
	else { 
		if (v==1) {
			printf("mkdir: created directory '%s'\n", argvv[x]);	
		}
	}
}		

exit(0);
}

static int flag_help;

static struct option const long_options[] = 
{
	{"v", no_argument, 0, 'v'}, 
	{"help", no_argument, &flag_help, 1}, 
   	{0, 0, 0, 0}			
};

usage() {
printf("usage: mkdir dir1 dir2 [-v]\n");
exit(1);
}

main (int argc, char* argv[]) {
int optc;
int index;

while ((optc = getopt_long (argc, argv, "v", long_options, (int *) 0)) !=EOF) {
	switch (optc) {
	   case 0:
		break; 
	   case 'v':
	        v=1;
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
	printf("mkdir: missing operand\n");
	usage();
}

else if (argc>1) {
	for (index=optind; index<argc; index++) {
		;
	}

	argc -= optind;
	argv += optind;
	leftover=index-optind;

	if (leftover>=1) {
		thefunc(argc, argv);
	}
	if (leftover==0) {
		usage();
	}
}

}
