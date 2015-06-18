#include<stdio.h>
#include<sys/types.h>
#include<stdlib.h>
#include<getopt.h>
#include<string.h>
#include<mntent.h>
#include<sys/statvfs.h> 
#include<sys/stat.h>
#include<dirent.h>
#include<unistd.h>

// df clone 0.1. matthew wilson. june 2015. 
// based on df from GNU Coreutils 8.23. 
// License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>

int a = 0;
int leftover=0;
int noargs=0;
int h = 0;

newfunc(int argccc, char* argvv[]) {

struct stat theStat;
int maj[999];
int min[999];
int maj2[999];
int min2[999]; 

int x;

int e=0;
int y;
DIR *pDir;
struct dirent *pDirent;
char* items[999];
char* concatitems[999];
char* thepath="/dev/";

pDir=opendir("/dev");

char* newitems[999];
int counter=0;

// FIRST: CREATE NEW ARRAY OF VECTORS ONLY IF THEY EXIST IN THE FILESYSTEM

for (x=0; x<argccc; x++) {
	if (access(argvv[x], F_OK)!=-1) { 
		newitems[counter]=malloc(strlen(argvv[x])+1);
		strcpy(newitems[counter], argvv[x]);
		//printf("%d %s\n", counter, newitems[counter]);
		counter++;
	}
	else {
		printf("df: '%s': no such file or directory\n", argvv[x]);
	}
}

// GET MAJ & MINOR NUMBERS FROM ARG VECTORS
	
for (x=0; x<counter; x++) {
	stat(newitems[x], &theStat);
	maj[x]=major(theStat.st_dev);
	min[x]=minor(theStat.st_dev);
	//printf("arg:%s maj: %d  min: %d\n", newitems[x], maj[x], min[x]);
}

char* dmdevices[999];

//  NOW READ ITEMS FROM /DEV  AND CONCAT /DEV/ ON TO EACH ITEM 
  
while ((pDirent=readdir(pDir))!=NULL) {
	items[e]=malloc(strlen(pDirent->d_name)+1);
       	strcpy(items[e], pDirent->d_name);
        //printf("%d: %s\n",e, items[e]);
    	concatitems[e]=malloc(1+strlen(thepath)+strlen(items[e]));
        strcpy(concatitems[e], thepath);
        strcat(concatitems[e], items[e]);
	e++;
}	
closedir(pDir);

// NOW MATCH MAJ/MINOR PAIRS FROM VECTORS WITH PAIRS FROM /DEV/ ITEMS

char* blocknamearr[999];
	
for (x=0; x<counter; x++) {
	for (y=0; y<e; y++) {
		struct stat theStat;
		lstat(concatitems[y], &theStat);
		maj2[y]=major(theStat.st_rdev);
		min2[y]=minor(theStat.st_rdev);

	if (maj[x]==maj2[y] && min[x]==min2[y]) { 
		// each matching item goes in blocknamearr
		blocknamearr[x]=malloc(strlen(concatitems[y])+1); 
		strcpy(blocknamearr[x], concatitems[y]);
	}
	}
}

// NOW GETTING SIZE FOR EACH DIR PROVIDED, NOT ITS CORRESPODING BLOCK DEVICE

char* filename;

//capacity of disk
unsigned long result=0;
unsigned long result2=0;
unsigned long resfinal=0;
unsigned long free=0;
unsigned long used=0;
unsigned long perc=0;
char sizetype;

// OUTPUT DISPLAY

if (h!=1 && counter>=1) {
printf("Filesystem          1-K blocks       Used       Avail Use%% Mounted on\n");
}

if (h==1) {
printf("Filesystem          Size  Used  Avail  Use%% Mounted on\n");
}

for (x=0; x<counter; x++) {
	//printf("%d %s\n", x, newitems[x]);
	filename=strdup(newitems[x]);
	struct statvfs Diskinfo;
	statvfs(filename, &Diskinfo);
	printf("%s ", blocknamearr[x]);

if (h!=1) {
// no flag, 1-k blocks
result2=(double)Diskinfo.f_blocks*Diskinfo.f_frsize/(1024);

if (result2==0) {  // handles dummy filesystems
        used=0;
        free=0;
        perc=0;
}
else {
	free=(Diskinfo.f_bsize*Diskinfo.f_bfree/(1024));
	used=result2-free;
	perc=100*used/result2;
}

printf("%20d %10ld %11ld %3ld%% %-12s\n", result2, used, free, perc, filename);
}

if (h==1) {
result=((unsigned long)Diskinfo.f_bsize*Diskinfo.f_blocks/(1024*1024));

if (result==0) {
	resfinal=0;
	sizetype='0';
	used=0;
	free=0;
	perc=0;
}

else if (result>1000) {
	resfinal=((double)result/1000);
	free=(Diskinfo.f_bsize*Diskinfo.f_bfree/(1024*1024)/1000);
	used=resfinal-free;
	perc=100*used/resfinal;
	sizetype='G';
}

else if (result<1000) {
	resfinal=(double)result;
	free=(Diskinfo.f_bsize*Diskinfo.f_bfree/(1024*1024));
	used=resfinal-free;
	perc=100*used/resfinal;
	sizetype='M';
}

printf("%13ld%c %4d%c %5ld%c %4ld%%  %-23s\n", resfinal, sizetype, used, sizetype, free, sizetype, perc, filename);
}

}

exit(0);
}

thefunc(int argcc, char* argvv[]) {

int x=0;
char* items[9999];
char* items2[9999];
struct mntent *Info; // mntent used to read fstab and mtab
FILE *thefile; // set up pointer to fstab

thefile=setmntent(_PATH_MOUNTED, "r");

// for -a flag. do not list dummy filesystems
while ((Info=getmntent(thefile))) {
	if (a!=1) {
		if (strcmp(Info->mnt_fsname, "proc")==0 ||
                    strcmp(Info->mnt_fsname, "rpc_pipefs")==0 ||  
		    strcmp(Info->mnt_fsname, "none")==0 ||  
		    strcmp(Info->mnt_fsname, "sysfs")==0 ||   
	     	    strcmp(Info->mnt_fsname, "devpts")==0 ||   
	  	    strcmp(Info->mnt_fsname, "tmpfs")==0 ||   
		    strcmp(Info->mnt_fsname, "debugfs")==0 ||
	 	    strcmp(Info->mnt_fsname, "cgroup")==0 ||
		    strcmp(Info->mnt_fsname, "pstore")==0 ||
	            strcmp(Info->mnt_fsname, "mqueue")==0 ||
		    strcmp(Info->mnt_fsname, "udev")==0 ||
		    strcmp(Info->mnt_fsname, "securityfs")==0 ||
	            strcmp(Info->mnt_fsname, "gvsd-fuse")==0 ||
	            strcmp(Info->mnt_fsname, "binfmt_misc")==0 ||
		    strcmp(Info->mnt_fsname, "hugetlbfs")==0 ||
		    strcmp(Info->mnt_fsname, "systemd-1")==0 ||
		    strcmp(Info->mnt_fsname, "fusectl")==0 ||
		    strcmp(Info->mnt_fsname, "gvfsd-fuse")==0 
		   )
			{
                		continue; // skips over listings
                	}
	}
        
items[x]=strdup(Info->mnt_dir); // pull these items out of fstab/mtab
items2[x]=strdup(Info->mnt_fsname);
//printf("%s - %s\n", items[x], items2[x]);
x++;
}

endmntent(thefile); // close fstab pointer

if (h!=1) {
printf("1-K blocks      Used     Avail Use%% Filesystem     Mounted on\n");
}

if (h==1) {
printf("Size  Used  Avail  Use%%  Filesystem               Mounted on\n");
}

int y=0;
char* filename_array[9999];
char* mountpoint_array[9999];

while (items[y]!=NULL) {
	filename_array[y]=strdup(items[y]);
	mountpoint_array[y]=strdup(items2[y]);
	//printf("%d: %s - %s\n", y, filename_array[y], mountpoint_array[y]);
	y++;
}

int qq=0;
char* filename;

while (filename_array[qq]!=NULL) {

filename=strdup(filename_array[qq]);
struct statvfs Diskinfo;
statvfs(filename, &Diskinfo);

//capacity of disk
unsigned long result=0;
unsigned long result2=0;
char sizetype;
unsigned long resfinal=0;
unsigned long free=0;
unsigned long used=0;
unsigned long perc=0;

if (h!=1) {

// no flag, 1-k blocks

result2=((unsigned long)Diskinfo.f_bsize*Diskinfo.f_blocks/(1024));

if (result2==0) {  // handles dummy filesystems
	used=0;
	free=0;
	perc=0;
}
else {
	free=(Diskinfo.f_bsize*Diskinfo.f_bfree/(1024));
	used=result2-free;
	perc=100*used/result2;
}

printf("%10d %9ld %9ld %3ld%% %-12s   %s\n", result2, used, free, perc, filename, mountpoint_array[qq]);

}

if (h==1) {
result=((unsigned long)Diskinfo.f_bsize*Diskinfo.f_blocks/(1024*1024));

if (result==0) {
	resfinal=0;
	sizetype='0';
	used=0;
	free=0;
	perc=0;
}
else if (result>1000) {
	resfinal=((double)result/1000);
	free=(Diskinfo.f_bsize*Diskinfo.f_bfree/(1024*1024)/1000);
	used=resfinal-free;
	perc=100*used/resfinal;
	sizetype='G';
}
else if (result<1000) {
	resfinal=(double)result;
	free=(Diskinfo.f_bsize*Diskinfo.f_bfree/(1024*1024));
	used=resfinal-free;
	perc=100*used/resfinal;
	sizetype='M';
}

printf("%3ld%c %4ld%c %5ld%c %4ld%%  %-24s %s\n", resfinal, sizetype, used, sizetype, free, sizetype, perc, filename, mountpoint_array[qq]);

}

qq++;
}

exit(0);
}

static int flag_help;

static struct option const long_options[] = 
{
    {"all", no_argument, 0, 'a'}, 
    {"humanreadable", no_argument, 0, 'h'},
    {"help", no_argument, &flag_help, 1},
    {0, 0, 0, 0}			
};

usage() {
printf("usage: 'df -[a][h]'\n");
exit(1);
}

main (int argc, char* argv[]) {

int optc;
int index;

while ((optc = getopt_long (argc, argv, "ah", long_options, (int *) 0)) !=EOF) {
	switch (optc) {
	    case 'a':
	        a=1;
		break;
	    case 'h':
	        h=1;
		break;
	    case '?':
	    default:	
		usage();
		break;
	}
}

if (flag_help) {
	usage();
}

if (argc==1) {
	noargs=1;
	thefunc(argc, argv);
}

else if (argc>1) {
//re-position arg vectors after removing flags
for (index=optind; index<argc; index++) {
	;
}

argc -= optind;
argv += optind;
leftover=index-optind;

if (leftover>=1) {
	newfunc(argc, argv);
}

	thefunc(argc, argv);
}

}
