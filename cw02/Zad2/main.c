#define _XOPEN_SOURCE 500
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>
#include <inttypes.h>
#include <linux/limits.h>
#include <ftw.h>

char* indicatedDirectory;
char* comparisonSign;
time_t indicatedTime;
char* mode;

char* printFileRights(mode_t buf){
    char* fileRights=malloc(11*sizeof(char));

    fileRights[0]=S_ISDIR(buf)?'d':'-';

    fileRights[1]=buf&S_IRUSR?'r':'-';
    fileRights[2]=buf&S_IWUSR?'w':'-';
    fileRights[3]=buf&S_IXUSR?'x':'-';

    fileRights[4]=buf&S_IRGRP?'r':'-';
    fileRights[5]=buf&S_IWGRP?'w':'-';
    fileRights[6]=buf&S_IXGRP?'x':'-';

    fileRights[7]=buf&S_IROTH?'r':'-';
    fileRights[8]=buf&S_IWOTH?'w':'-';
    fileRights[9]=buf&S_IXOTH?'x':'-';

    fileRights[10]='\0';

    return fileRights;
}

void logArguments(){
    printf("------------\n");
    printf("Indicated directory: \t%s\n",indicatedDirectory);
    printf("Comparison sign: \t%s\n",comparisonSign);
    printf("Date&Time: \t\t%s",ctime(&indicatedTime));
    printf("Mode: \t\t\t%s\n",mode);
    printf("------------\n");
}

int parseArguments(int argc, char* argv[]){

    indicatedDirectory=argv[1];
    comparisonSign=argv[2];
    mode=argv[4];

    if(strcmp(comparisonSign,"<")==0 || strcmp(comparisonSign,"=")==0 || strcmp(comparisonSign,">")==0){

        const char *date=argv[3];
        if(strlen(date)!=19){
            printf("Invalid data argument. Type YYYY-mm-dd HH:MM:SS\n");
            return -2;
        }

        struct tm theTime;
        const char *dateTimeFormat="%Y-%m-%d %H:%M:%S";

        strptime(date,dateTimeFormat,&theTime);
        theTime.tm_isdst=-1;

        indicatedTime=mktime(&theTime);

    }else{
        printf("Invalid comparison argument. Type '<', '=' or '>'.\n");
        return -1;
    }

    return 0;
}

int compareTime(time_t fileTime){
    int tdiff=(int)difftime(fileTime,indicatedTime);

    return (tdiff==0 && strcmp(comparisonSign,"=")==0) 
        || (tdiff<0 && strcmp(comparisonSign,"<")==0) 
        || (tdiff>0 && strcmp(comparisonSign,">")==0);
}

void logFileInfo(const char* fpath,const struct stat *stats){

    printf("\n%s\n", realpath(fpath,NULL));
    printf("\tLast modification: %19s",ctime(&(stats->st_mtime)));
    printf("\tSize in bytes: %lld\n", (long long) stats->st_size);
    printf("\tRights: %s\n", printFileRights(stats->st_mode));
}

int iterateThroughDirectories(char* directory){

    DIR *dir=opendir(directory);
    if(dir==NULL){
        return -1;
    }

    struct dirent *sd;
    
    while((sd=readdir(dir))!=NULL){
        char path[PATH_MAX+1];
        strcpy(path,directory);
        strcat(path,"/");
        strcat(path,sd->d_name);

        struct stat fileStat;
        stat(path,&fileStat);

        if(S_ISDIR(fileStat.st_mode) || S_ISLNK(fileStat.st_mode) || !S_ISREG(fileStat.st_mode)){
            if(strcmp(sd->d_name,".")==0 || strcmp(sd->d_name,"..")==0){
                //sd=readdir(dir);
                continue;
            }else{
                iterateThroughDirectories(path);
            }
        }

        //log file date if possible
        if(!S_ISDIR(fileStat.st_mode) && compareTime(fileStat.st_mtime)){
            logFileInfo(path,&fileStat);  
        }

        //sd=readdir(dir);
    }

    if(closedir(dir)==-1){
        printf("error");
        return -1;
    }

    return 0;
}

int displayInfo(const char *fpath, const struct stat *stats, int fileFlags, struct FTW * ftwPtr){

    if(S_ISDIR(stats->st_mode) || S_ISLNK(stats->st_mode) || !S_ISREG(stats->st_mode)) return 0;

    if(compareTime(stats->st_mtime)) logFileInfo(fpath,stats);

    return 0;
}

int main(int argc, char* argv[]){

    if(argc!=5 || parseArguments(argc,argv)<0){
        printf("Invalid argument. Type %s path '<'|'='|'>' 'YYYY-mm-dd HH:MM:SS' dir|nftw\n",argv[0]);
        return -1;
    }

    logArguments();

    if(strcmp(mode,"dir")==0){
        iterateThroughDirectories(indicatedDirectory);
    }else if(strcmp(mode,"nftw")==0){
        nftw(indicatedDirectory,displayInfo,5,FTW_PHYS);
    }else{
        printf("Invalid argument. Type dir or nftw.\n");
    }

    
    return 0;
}