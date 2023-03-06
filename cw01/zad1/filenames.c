#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<math.h>
#include<time.h>


#define UID_RAND_LENGTH 2
#define TIME_DIGITS 12


void _set_uid(char* uid) {
    sprintf(uid, "%ld%d", time(NULL), rand() % (int)pow(10.0, UID_RAND_LENGTH + 1));
}


void set_tmp_filename(char* filename, char* tmp_filename) {
    char tmp_file_uid[TIME_DIGITS + UID_RAND_LENGTH];

    _set_uid(tmp_file_uid);

    strcpy(tmp_filename, filename);
    strcat(tmp_filename, tmp_file_uid);
}


void get_filename(char* filepath, char* filename) {
    static const char delim[] = "/";

    char* path_copy = malloc(strlen(filepath) * sizeof(char));
    strcpy(path_copy, filepath);

    char* last_token;
    char* token = strtok(path_copy, delim);

    while(token) {
        last_token = token;
        token = strtok(NULL, delim);
    }

    strcpy(filename, last_token);
    
    free(path_copy);
}
