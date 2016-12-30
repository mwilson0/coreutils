#define _XOPEN_SOURCE 500
#include<ftw.h>
#include<stdio.h>
#include<getopt.h>
#include<stdlib.h>
#include<grp.h>
#include<pwd.h>
#include<string.h>
#include<sys/types.h>
#include<unistd.h>
#include<fcntl.h>
#include<sys/stat.h>
#include<dirent.h>

// chown clone 0.1.2. matthew wilson. june-july 2015, dec 2016.
// based on chown from GNU Coreutils 8.23. 
// License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>

void usage();
int chownfunc();
int recursive();
int userchecker();

int r = 0;
const char* program_name;
static int fail = 0;	
char* delimsearch;
char* uname;
char* grname;
int grouper = 0;
int nontokenuser = 0;
struct passwd *pw;
struct group *gr;
	
// nftw recursive function to walk thru directories
int recursive(const char *path, const struct stat *s, int flag, struct FTW *f) {
 	// user: and user:group
	if (nontokenuser == 1 || delimsearch != NULL) {
		chown(path, pw->pw_uid, gr->gr_gid);
	}	
	//:group
	if (grouper == 1) {
		chown(path, -1, gr->gr_gid);
	}
	// user	
	if (delimsearch == NULL && nontokenuser == 0 && grouper != 1){
		chown(path, pw->pw_uid, -1);
	}
	return 0;
}

int userchecker(int argc, char* argv[]) {
	// look at argvv[0]. check format -  "user:"
	if (argv[0][strlen(argv[0])-1] == ':') {
		int len = strlen(argv[0]);
		argv[0][len - 1] = '\0';
		uname = argv[0];

		// this is key, copy uname to grname if using "user:"
		grname = strdup(uname);

		if (((pw = getpwnam(uname)) == NULL) || ((gr = getgrnam(grname)) == NULL)) {
			printf("%s: invalid user: '%s'\n", program_name, argv[0]);
			return(1);
		}
		// if so, chown'd files become user:user if user: command given
		nontokenuser = 1;
	}
	// or :group
	else if (argv[0][0] == ':') {
		grname = &argv[0][1];
		
		if ((gr = getgrnam(grname)) == NULL) {
			printf("%s: invalid group: '%s'\n", program_name, grname);
			return(1);
		}
		grouper = 1;
	}
	// OR check for tokens in argvv[0]. see if argument is delimited or not
	else {
		delimsearch = strchr(argv[0], ':'); //search string
		if (delimsearch == NULL) {
			if ((pw = getpwnam(argv[0])) == NULL) {
				printf("%s: invalid user: '%s'\n", program_name, argv[0]);	
				return(1);
			}
		}
		else {
			//tokenize the user:group
			char* argvcopy;		
			argvcopy = strdup(argv[0]);

			uname = strtok(argvcopy, ":");
			grname = strtok(NULL, ":");
			
			if (strtok(NULL, ":")) { 	
				printf("%s: invalid user: '%s'\n", program_name, argv[0]);
				return(1);
			}
			if ((pw = getpwnam(uname)) == NULL) {
				printf("%s: invalid user: '%s'\n", program_name, argv[0]);
				return(1);
			}
			if ((gr = getgrnam(grname)) == NULL) {
				printf("%s: invalid group: '%s'\n", program_name, grname);
				return(1);
			}
		}
	} 
	return(0);
}

int chownfunc(int argc, char*argv[]) {
	struct stat fileStat;
	char* items[999];
	char* newitems[999];
	int x;
	int i;
	int j = 0;
	int counter = 0;

	// files now in items[] array. starting at 1 to skip over user:group
	for (i = 1; i < argc; i++) {
		items[j] = malloc(strlen(argv[i]) + 1);
		strcpy(items[j], argv[i]);
		j++;
	}

	// now test access and write perms of files and move all existing and
	// writable items to newitems[counter] 

	for (x = 0; x < j; x++) {
		if (access(items[x], F_OK|R_OK)!=-1) {
			if (access(items[x], W_OK)==-1) {
				printf("%s: changing ownership of '%s': ", program_name, items[x]);
				printf("operation not permitted\n");
				fail = 1;
			}
			else {
				newitems[counter]=malloc(strlen(items[x])+1);
				strcpy(newitems[counter], items[x]);
				counter++;
			}
		}
		else {
			printf("%s: cannot access '%s': ", program_name, items[x]);	
			printf("No such file or directory\n");
			fail = 1;
		}
	}

	// loop through filenames and differentiate between files and dirs
	// if dirs and if r=1, go into dirs and chown each file inside using nftw
	for (x = 0; x < counter; x++) {
		stat(newitems[x], &fileStat);
		// using format of :group
		if ((grouper == 1) && (S_ISREG (fileStat.st_mode))) {
			chown(newitems[x], -1, gr->gr_gid);
		}
		else if ((grouper == 1) && (S_ISDIR (fileStat.st_mode))) {
			if (r == 1) {
				if (nftw(newitems[x], recursive, 1024, FTW_DEPTH )) {
					perror(newitems[x]);
				}
			}	
			else {
				chown(newitems[x], -1, gr->gr_gid);
			}
		}
		// using format of user: or user
		else if ((delimsearch == NULL || nontokenuser == 1) && (S_ISREG (fileStat.st_mode))) {
			if (nontokenuser == 1) {
				chown(newitems[x], pw->pw_uid, gr->gr_gid);
			}
			else {
				chown(newitems[x], pw->pw_uid, -1);
			}
		}
		else if ((delimsearch == NULL || nontokenuser == 1) && (S_ISDIR (fileStat.st_mode))) {
			if (r == 1) {
				if (nftw(newitems[x], recursive, 1024, FTW_DEPTH )) {
					perror(newitems[x]);
				}
			}
			else {
				if (nontokenuser == 1) {
					chown(newitems[x], pw->pw_uid, gr->gr_gid);
				}
				else {
					chown(newitems[x], pw->pw_uid, -1);
				}
			}
		}
		// using format of user:group
		if (delimsearch != NULL && (S_ISREG (fileStat.st_mode))) {
			chown(newitems[x], pw->pw_uid, gr->gr_gid);
		}
		else if (delimsearch != NULL && (S_ISDIR (fileStat.st_mode))) {
			if (r == 1) {
				if (nftw(newitems[x], recursive, 1024, FTW_DEPTH )) {
					perror(newitems[x]);
				}
			}			
			else {
				chown(newitems[x], pw->pw_uid, gr->gr_gid);
			}
		}
	} 
	return(0);
}

static int flag_help;

static struct option const long_options[] = {
	{"recursive", no_argument, 0, 'R'},
	{"help", no_argument, &flag_help, 1},
    	{0, 0, 0, 0}			
};

void usage(void) {
	if (flag_help) {
		printf("chown! change file ownership!\n");
	}
	
	printf("usage: 'chown root:staff somefile ... somedir [-R | --recursive]'\n");

	if (flag_help) {
		exit(EXIT_SUCCESS);
	}
	else {
		exit(EXIT_FAILURE);
	}
}

int main (int argc, char* argv[]) {
	program_name = argv[0];
	int optc;
	int index;

	while ((optc = getopt_long (argc, argv, "R", long_options, (int *) 0)) != EOF) {
		switch (optc) {
	   		case 0:
		            break; 
	   		case 'R':
	        	    r = 1;
			    break;
	   		case '?':
			    usage();
			    break;
	   		default:	
		            usage();
			    break;
		}
	}

	for (index = optind; index < argc; index++) {
		;
	}
	
	argc -= optind;
	argv += optind;
	
	if (flag_help) {
		usage();
	}

	if (argc == 0) {
		printf("%s: missing operand\n", program_name);
		usage();
	}
	else if (argc < 2) {
		printf("%s: missing operand after: %s\n", program_name, argv[0]);
		usage();
	}
	else {
		if (userchecker(argc, argv) != 0) {
			exit(EXIT_FAILURE);
		}
		if (chownfunc(argc, argv) != 0 || fail == 1) {
			exit(EXIT_FAILURE);
		}
	}
	exit(EXIT_SUCCESS);
}
