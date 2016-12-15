#include<stdio.h>
#include<getopt.h>
#include<stdlib.h>
#include<sys/utsname.h>
#include<string.h>
#include<errno.h>
#include<sys/stat.h>
#include<unistd.h>
#include<dirent.h>

/* rm clone. (c) 2016 matthew wilson
License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>
No warranty.
*/

int r = 0, f = 0, v = 0;
static int flag_help;
static int fail = 0;
char* program_name;

int file_checker(int argc, char* argv[]) {
	int x;
	DIR *pDir;
	struct dirent *pDirent; //structure has file info(d_name[])
	struct stat sb;

	for (x = 0; x < argc; x++) {
	
		// file exists
		if (access(argv[x], F_OK) == 0) {
			stat(argv[x], &sb);
			if (S_ISREG(sb.st_mode)) {
				printf("%d: %s - regular file\n", x, argv[x]);
				
				if (unlink(argv[x]) == 0) {
					// verbose
					if (v == 1) {
						printf("removed file: %s\n", argv[x]);
					}
				}
				// error out if not exist	
				else {
					// but don't print message if using --force
					if (f != 1) {
						printf("%s: %s\n", argv[x], strerror(errno));
					}
				}
			}
		
			else if(S_ISDIR(sb.st_mode)) {
				// how to handle recursive? 
				// likely open dir and delete files, then delete dir
				printf("%d: %s - directory\n", x, argv[x]);
				
				// -r flag must be set to delete dir		
				if (r != 1) {
					printf("%s: cannot remove '%s': is a directory\n", program_name, argv[x]);
				}
				// walk inside directory
				else {
					int cnt = 0;
					strcat(argv[x], "/");
					pDir = opendir(argv[x]);
                			char* items[999];
					
					while ((pDirent = readdir(pDir)) != NULL) {
						if (strcmp(pDirent->d_name, ".") == 0 || strcmp(pDirent->d_name, "..") == 0 ||
     						pDirent->d_name[0] == '.' ) {
               						continue; // skips over . and .. listings
                        			}
						// pull filename out of struct
                				items[cnt] = malloc(strlen(pDirent->d_name) + 1);
				                strcpy(items[cnt], pDirent->d_name);
				                cnt++;
			                }
					
					printf("items in dir: %d\n", cnt);
					
					// delete now, or walk deeper and delete invididual files
					// see ntfw
					if (cnt == 0) {
						closedir(pDir);
						printf("good to delete dir now\n");			
						
						if (rmdir(argv[x]) != 0) {
							printf("%s: %s\n", argv[x], strerror(errno));
						}
					}
					// check for files or directories again
					else {
						;	
					}

				}	

			}
			else {
				printf("%d: %s - something else\n", x, argv[x]);
			}
		}
		else {
			if (f != 1) {
				printf("%d: %s - file does not exist\n", x, argv[x]);
			}
		}
	}
	return(0);
}



int rm_func(int argc, char* argv[]) {

	int x;

	printf("argc:%d r:%d f:%d v:%d\n", argc, r, f, v);

	for (x = 0; x < argc; x++) {
		//printf("%d %s\n", x, argv[x]);
		
		//error
		if (unlink(argv[x]) != 0) {
			printf("%s: %s\n", argv[x], strerror(errno));
		}	
		
		else {
			// verbose
			if (v == 1) {
				printf("removed %s\n", argv[x]);
			}
		}
	}
	return(0);
}



static struct option const long_options[] = 
{
    {"recursive", no_argument, 0, 'r'},
    {"force", no_argument, 0, 'f'},	
    {"verbose", no_argument, 0, 'v'}, 
    {"help", no_argument, &flag_help, 1},
    {0, 0, 0, 0}			
};

void usage() {
	fprintf(stderr, "Usage: %s [file 1] .. [file n] [-r | --recursive] [-f | --force] [-v | --verbose] [--help]\n", program_name);

	if ((flag_help) && (fail != 1))
		exit(EXIT_SUCCESS);
	else
		exit(EXIT_FAILURE);
}

int main (int argc, char* argv[]) {
	program_name = argv[0];
	int index;
	int optc;
	int x;
	

	while ((optc = getopt_long (argc, argv, "rfv", long_options, (int *) 0)) != EOF) {
		switch (optc) {
	    		case 0:
	        	    break;
	    		case 'r':
			    r = 1;
			    break; 	
	    		case 'f':
			    f = 1;
			    break; 	
	      		case 'v':
			    v = 1;
                	    break;
			case '?':
			    fail = 1;
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

	for (index = optind; index < argc; index++) {
		;
	}

	argc -= optind;
	argv += optind;

	if (file_checker(argc, argv) == 0) {
		exit(EXIT_SUCCESS);
	}
	else {
		exit(EXIT_FAILURE);
	}
}
