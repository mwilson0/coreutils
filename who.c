#include<stdio.h>
#include<getopt.h>
#include<stdlib.h>
#include<ctype.h>
#include<time.h>
#include<utmp.h> 
#include<sys/types.h>
#include<string.h>

// who clone 0.1.2. matthew wilson. 2015,16. 
// who clone from spec as used in GNU coreutils 8.23. No warranty. 
// License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>

/*
ut_user = login user name
ut_id = id from rootdir/etc/inittab
ut_line = login terminal name
ut_pid = process id
ut_type = entry type - #7 is a logged in user 
ut_host = host user logged in from
*/

int a = 0, b = 0, q = 0, r = 0, u = 0;

int thefunc(int argc, char*argv[]) {
	int usercount = 0;
	struct utmp *ii;
	time_t logintime_raw;
	time_t boottime_raw;

	setutent();

	// number of logged in users. -q flag runs alone, thus following logic
	if (q == 1) {
		while ((ii = getutent()) != NULL) {
			if (ii->ut_type == 7) {
				printf("%s ", ii->ut_user);
				usercount++;
			}
		}
		printf("\n# users=%d\n", usercount);
		endutent();
		return(0);
	}

	if (a == 1) {
        	while ((ii = getutent()) != NULL) {
			char tempbuf[99]; // used to remove newline from ctime
			if (ii->ut_type == RUN_LVL) {
				logintime_raw = ii->ut_time;
				printf("           ");
                   		printf("run-level %c %s", ii->ut_pid, ctime(&logintime_raw));
			}
    			else if (ii->ut_type == BOOT_TIME) {
                		boottime_raw = ii->ut_time;
                        	printf("           ");
				printf("system boot %s", ctime(&boottime_raw));
         		}
			else {
				if (ii->ut_pid == 0) {
					printf("%16s     ", ii->ut_line);
				}
				else {
					printf("%-10s %-10s", ii->ut_user, ii->ut_line);
				}
                		logintime_raw = ii->ut_time;
                		strcpy(tempbuf, ctime(&logintime_raw));
                		tempbuf[strlen(tempbuf) - 1] = '\0';
                		printf("  %s %5d (%s)\n", tempbuf, ii->ut_pid, ii->ut_host);
			}
		}
		endutent();
		return(0);
	}

	else {
		while ((ii = getutent()) != NULL) {
			// runlevel
			if (r == 1) {
				if (ii->ut_type == RUN_LVL) {
					logintime_raw = ii->ut_time;	
					printf("run-level %c %s", ii->ut_pid, ctime(&logintime_raw));
				}
			}
			// boot time
			if (b == 1) {
				if (ii->ut_type == BOOT_TIME) {
					boottime_raw=ii->ut_time;	
					printf("system boot %s", ctime(&boottime_raw));
				}
			}
			// continue to fall through
			// no args reg command or -u
			if (ii->ut_type == 7) {
					if (((b == 1) || (r == 1)) && (u != 1)) {
						continue;			
					}	
					
					char tempbuf[99]; // used to remove newline from ctime
					logintime_raw = ii->ut_time;
					strcpy(tempbuf, ctime(&logintime_raw));
					tempbuf[strlen(tempbuf) - 1] = '\0';
				
					printf("%-10s %-10s", ii->ut_user, ii->ut_line);
										
					if (u == 1) 
						printf("%s %5d (%s)\n", tempbuf, ii->ut_pid, ii->ut_host);
					else 
						printf("%s (%s)\n", tempbuf, ii->ut_host);
			}
		}
		endutent();
	}
	return(0);
}

static int flag_help;

static struct option const long_options[] = 
{
	{"all", no_argument, 0, 'a'}, 
    	{"boottime", no_argument, 0, 'b'}, 
    	{"numandusers", no_argument, 0, 'q'}, 
    	{"runlevel", no_argument, 0, 'r'}, 
    	{"usersloggedin", no_argument, 0, 'u'},
	{"help", no_argument, &flag_help, 1}, 
    	{0, 0, 0, 0}			
};

void usage() {
	printf("usage: 'who [-abqru]'\n");
	exit(EXIT_FAILURE);
}

int main (int argc, char* argv[]) {
	int optc;
	int index;

	while ((optc = getopt_long (argc, argv, "abqru", long_options, (int *) 0)) != EOF) {
		switch (optc) {
	    		case 0:
		    	    break; 
	    		case 'a':
	            	    a = 1;
		    	    break;
	    		case 'b':
	            	    b = 1;
		    	    break;
	    		case 'q':
	            	    q = 1;
		            break;
	    		case 'r':
	                    r = 1;
		    	    break;
	    		case 'u':
	            	    u = 1;
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

	for (index = optind; index < argc; index++) {
		;
	}
		
	argc -= optind;
	argv += optind;

	if (argc > 0) 
		exit(EXIT_FAILURE);

	if (thefunc(argc, argv) != 0) 
			exit(EXIT_FAILURE); 

	exit(EXIT_SUCCESS);
}
