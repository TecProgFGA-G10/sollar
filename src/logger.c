/*
// Library to implement a simple log in programs based on c language
*/

#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

static FILE *log_file ;

static void initialize()
{
    struct status file_status = {0};

    if (stat("log", &file_status) == -1) {
        mkdir("log", 0700);
    }
    else {
        /* nothing to do */
    }
}

// gets the real time of the execution of the log and sends the information to the main function
static char* print_time()
{
    time_t time_in_seconds;
    char *buffer;
    time(&time_in_seconds);

    buffer = (char*)malloc(strnlen(ctime(&time_in_seconds))+ 1);

    snprintf(buffer,strnlen(ctime(&time_in_seconds)),"%s ", ctime(&time_in_seconds));

    return buffer;
}

void print_debug_log(char* filename, int line, char *fmt,...)
{
    initialize();
    va_list list;
    char *p, *r;
    int e;

    log_file = fopen ("log/debug.log","a+");

    fprintf(log_file,"[%s][%s][line: %d] ",print_time(),filename,line);
    va_start( list, fmt );

    for ( p = fmt ; *p ; ++p )
    {
        //If simple string
        if ( *p != '%' ) {
            fputc( *p,log_file );
        }
        else {
            switch ( *++p ) {
                /* string */
            case 's':
            {
                r = va_arg( list, char * );

                fprintf(log_file,"%s", r);
                continue;
            }

            /* integer */
            case 'd':
            {
                e = va_arg( list, int );

                fprintf(log_file,"%d", e);
                continue;
            }

            default:
                fputc( *p, log_file );
            }
        }
    }
    va_end( list );
    fputc( '\n', log_file );
    fclose(log_file);
}

void print_error_log(char* filename, int line, char *fmt,...)
{
    initialize();
    va_list list;
    char *p, *r;
    int e;

    log_file = fopen ("log/error.log","a+");

    fprintf(log_file,"[%s][%s][line: %d] ",print_time(),filename,line);
    va_start( list, fmt );

    for ( p = fmt ; *p ; ++p ) {
        //If simple string
        if ( *p != '%' ) {
            fputc( *p,log_file );
        }
        else {
            switch ( *++p ) {
                /* string */
            case 's':
            {
                r = va_arg( list, char * );

                fprintf(log_file,"%s", r);
                continue;
            }

            /* integer */
            case 'd':
            {
                e = va_arg( list, int );

                fprintf(log_file,"%d", e);
                continue;
            }

            default:
                fputc( *p, log_file );
            }
        }
    }
    va_end( list );
    fputc( '\n', log_file );
    fclose(log_file);
}

void print_verbose_log(char* filename, int line, char *fmt,...)
{
    initialize();
    va_list list;
    char *char_pointer, *new_char;
    int e;

    log_file = fopen ("log/verbose.log","a+");

    fprintf(log_file,"[%s][%s][line: %d] ",print_time(),filename,line);
    va_start( list, fmt );

    for ( char_pointer = fmt ; *char_pointer ; ++char_pointer )
    {
        //If simple string
        if ( *char_pointer != '%' ) {
            fputc( *char_pointer,log_file );
        }
        else {
            switch ( *++char_pointer ) {
                /* string */
            case 's':
            {
                new_char = va_arg( list, char * );

                fprintf(log_file,"%s", new_char);
                continue;
            }

            /* integer */
            case 'd':
            {
                e = va_arg( list, int );

                fprintf(log_file,"%d", e);
                continue;
            }

            default:
                fputc( *char_pointer, log_file );
            }
        }
    }
    va_end( list );
    fputc( '\n', log_file );
    fclose(log_file);
}
