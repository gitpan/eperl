/*
**        ____           _ 
**    ___|  _ \ ___ _ __| |
**   / _ \ |_) / _ \ '__| |
**  |  __/  __/  __/ |  | |
**   \___|_|   \___|_|  |_|
** 
**  ePerl -- Embedded Perl 5 Language
**
**  ePerl interprets an ASCII file bristled with Perl 5 program statements
**  by evaluating the Perl 5 code while passing through the plain ASCII
**  data. It can operate both as a standard Unix filter for general file
**  generation tasks and as a powerful Webserver scripting language for
**  dynamic HTML page programming. 
**
**  ======================================================================
**
**  Copyright (c) 1996,1997 Ralf S. Engelschall, All rights reserved.
**
**  This program is free software; it may be redistributed and/or modified
**  only under the terms of either the Artistic License or the GNU General
**  Public License, which may be found in the ePerl source distribution.
**  Look at the files ARTISTIC and COPYING or run ``eperl -l'' to receive
**  a builtin copy of both license files.
**
**  This program is distributed in the hope that it will be useful, but
**  WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See either the
**  GNU General Public License or the Artistic License for more details.
**
**  ======================================================================
**
**  eperl_sys.c -- ePerl system functions
*/


#include "eperl_global.h"
#include "eperl_proto.h"


/*
**
**  own setenv function which works with Perl
**
*/
char **mysetenv(char **env, char *var, char *str, ...)
{
    va_list ap;
    char ca[1024];
    char ca2[1024];
    char *cp;
    int i;
    char **envN;
    extern char **environ;
    static stillcalled = FALSE;

    /*  create the key=val string  */
    va_start(ap, str);
    vsprintf(ca, str, ap);
    sprintf(ca2, "%s=%s", var, ca);
    cp = strdup(ca2);

    /*  now duplicate the old structure  */
    for (i = 0; env[i] != NULL; i++)
        ;
    envN = (char **)malloc(sizeof(char *) * (i+2));
    for (i = 0; env[i] != NULL; i++)
        envN[i] = env[i];

    /*  and add the new entry  */
    envN[i++] = cp;
    envN[i++] = NULL;

    /* set the libc/exec variable which Perl uses */
    if (stillcalled) 
        free(environ);
    stillcalled = TRUE;
    environ = envN;

    va_end(ap);
    return envN;
}


/*
**
**  I/O handle redirection
**
*/

#define HANDLE_STDIN  0
#define HANDLE_STDOUT 1
#define HANDLE_STDERR 2
#define HANDLE_STORE_STDIN  10
#define HANDLE_STORE_STDOUT 11
#define HANDLE_STORE_STDERR 12

static int IO_redirected_stdin  = FALSE;
static int IO_redirected_stdout = FALSE;
static int IO_redirected_stderr = FALSE;

void IO_redirect_stdin(FILE *fp)
{
    if (IO_redirected_stdin)
        IO_restore_stdin();

    /* first copy the current stdin to the store handle */
    (void)dup2(HANDLE_STDIN, HANDLE_STORE_STDIN);

    /* then copy the new handle to stdin */
    (void)dup2(fileno(fp), HANDLE_STDIN);

    /* and remember the fact */
    IO_redirected_stdin = TRUE;
}


void IO_redirect_stdout(FILE *fp)
{
    if (IO_redirected_stdout)
        IO_restore_stdout();

    /* first copy the current stdout to the store handle */
    (void)dup2(HANDLE_STDOUT, HANDLE_STORE_STDOUT);

    /* then copy the new handle to stdout */
    (void)dup2(fileno(fp), HANDLE_STDOUT);

    /* and remember the fact */
    IO_redirected_stdout = TRUE;
}

void IO_redirect_stderr(FILE *fp)
{
    if (IO_redirected_stderr)
        IO_restore_stderr();

    /* first copy the current stderr to the store handle */
    (void)dup2(HANDLE_STDERR, HANDLE_STORE_STDERR);

    /* then copy the new handle to stderr */
    (void)dup2(fileno(fp), HANDLE_STDERR);

    /* and remember the fact */
    IO_redirected_stderr = TRUE;
}

int IO_is_stdin_redirected(void)
{
    return IO_redirected_stdin;
}

int IO_is_stdout_redirected(void)
{
    return IO_redirected_stdout;
}

int IO_is_stderr_redirected(void)
{
    return IO_redirected_stderr;
}

void IO_restore_stdin(void)
{
    if (IO_redirected_stdin) {
        dup2(HANDLE_STORE_STDIN, HANDLE_STDIN); 
        IO_redirected_stdin = FALSE;
    }
}

void IO_restore_stdout(void)
{
    if (IO_redirected_stdout) {
        dup2(HANDLE_STORE_STDOUT, HANDLE_STDOUT); 
        IO_redirected_stdout = FALSE;
    }
}

void IO_restore_stderr(void)
{
    if (IO_redirected_stderr) {
        dup2(HANDLE_STORE_STDERR, HANDLE_STDERR); 
        IO_redirected_stderr = FALSE;
    }
}


/*
**
**  Temporary filename support  
**
*/

static char *mytmpfiles[100] = { NULL };
static int mytmpfilecnt = 0;

char *mytmpfile(char *id)
{
    char ca[1024];
    char *cp;
    int i;

    sprintf(ca, "/tmp/%s.%d.tmp%d", id, (int)getpid(), mytmpfilecnt++);
    cp = strdup(ca);
    for (i = 0; mytmpfiles[i] != NULL; i++)
        ;
    mytmpfiles[i++] = cp;
    mytmpfiles[i] = NULL;
    return cp;
}

void remove_mytmpfiles(void)
{
    int i;

    for (i = 0; mytmpfiles[i] != NULL; i++) {
        unlink(mytmpfiles[i]);
    }
}

/*
**
**
*/

char *strnchr(char *buf, char chr, int n)
{
    char *cp;
    char *cpe;

    for (cp = buf, cpe = buf+n-1; cp <= cpe; cp++) {
        if (*cp == chr)
            return cp;
    }
    return NULL;
}

char *strnstr(char *buf, char *str, int n)
{
    char *cp;
    char *cpe;
    int len;
    
    len = strlen(str);
    for (cp = buf, cpe = buf+n-len; cp <= cpe; cp++) {
        if (strncmp(cp, str, len) == 0)
            return cp;
    }
    return NULL;
}

char *strndup(char *buf, int n)
{
    char *cp;

    cp = (char *)malloc(n+1);
    strncpy(cp, buf, n);
    return cp;
}

/*
**
**  ISO time
**
*/

char *isotime(time_t *t)
{
    struct tm *tm;
    char timestr[128];

    tm = localtime(t);
    sprintf(timestr, "%02d-%02d-19%02d %02d:%02d",
                      tm->tm_mday, tm->tm_mon+1, tm->tm_year,
                      tm->tm_hour, tm->tm_min);
    return strdup(timestr);
}

/*EOF*/
