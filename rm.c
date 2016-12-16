#define _XOPEN_SOURCE 600
#include<stdio.h>
#include<getopt.h>
#include<stdlib.h>
#include<sys/utsname.h>
#include<string.h>
#include<errno.h>
#include<sys/stat.h>
#include<unistd.h>
#include<dirent.h>
#include<ftw.h>

/* rm clone. (c) 2016 matthew wilson
License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>
No warranty.
*/

int r = 0, f = 0, v = 0;
static int flag_help;
static int fail = 0;
char* program_name;

// call unlink on the files or directory
int rm(const char *path, const struct stat *s, int flag, struct FTW *f) {
	int status;
    	int (*rm_func)( const char * );

    	switch(flag) {
    		default:     
		    rm_func = unlink; 
		    break;
    		case FTW_DP: 
		    rm_func = rmdir;
    	}
    
	if(status = rm_func(path), status != 0 ) {
        printf("status %d\n", status);
	perror(path);
	//printf("error: %s\n", strerror(errno));
    	}
    	else {
		printf("item deleted: %s\n", path);
	}
    
	return status;
}

int file_checker(int argc, char* argv[]) {
	int x;
	DIR *pDir;
	struct dirent *pDirent; //structure has file info(d_name[])
	struct stat sb;

	for (x = 0; x < argc; x++) {
	
		// file exists
		//if (access(argv[x], F_OK|W_OK) == 0) {
			stat(argv[x], &sb);
			if (S_ISREG(sb.st_mode)) {
				printf("%d: %s - regular file\n", x, argv[x]);
				// delete it
				if (unlink(argv[x]) == 0) {
					// verbose
					if (v == 1) {
						printf("removed file: %s\n", argv[x]);
					}
				}
				// error out otherwise	
				else {
					// but don't print message if using --force
					if (f != 1) {
						printf("!%s: %s\n", argv[x], strerror(errno));
					}
				}
			}
			else if(S_ISDIR(sb.st_mode)) {
				printf("%d: %s - directory\n", x, argv[x]);
				// -r flag must be set to delete dir		
				if (r != 1) {
					printf("%s: cannot remove '%s': is a directory\n", program_name, argv[x]);
					fail = 1;
				}
				else {
					// next test for access
					if (access(argv[x], F_OK|W_OK) != 0) {
						printf("!! %s %s\n", argv[x], strerror(errno));
						//fail = 1;
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
						if (cnt == 0) {
							closedir(pDir);
							printf("good to delete dir now\n");			
						
							if (rmdir(argv[x]) != 0) {
								if (f != 1) {
									printf("%s: %s\n", argv[x], strerror(errno));
									fail = 1;
								}
							}
						}
						// delete recursively otherwise
						else {
							if ((nftw(argv[x], rm, 10, FTW_DEPTH)) != 0) {
								printf("unable to delete: %s\n", argv[x]);
								//fail = 1;
							}
						}
					}
				}
			}
			else {
				//printf("%d: %s - something else\n", x, argv[x]);
				if (stat != 0 && f != 1) {
					printf(":)%s %s\n", argv[x], strerror(errno));
				}
				if (f != 1) {
					fail = 1;
				}
			
			}
		//}
		
		/*else {
			if (f != 1) {
				printf("%s: %s\n", argv[x], strerror(errno));
				//printf("%d: %s - file does not exist\n", x, argv[x]);
				fail = 1;
			}
		}*/
	}
	printf("yo\n");
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

	if (file_checker(argc, argv) == 0 && fail != 1) {
		exit(EXIT_SUCCESS);
	}
	else {
		exit(EXIT_FAILURE);
	}
}
