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
**  eperl_global.h -- ePerl global header file
*/
#ifndef EPERL_GLOBAL_H
#define EPERL_GLOBAL_H 1

/* from our own */
#include "config_ac.h"
#include "config_sc.h"

/* from the Unix system */
#ifdef HAVE_STDIO_H
#include <stdio.h>
#endif
#ifdef HAVE_STDARG_H
#include <stdarg.h> 
#else
#include <varargs.h>
#endif
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif
#ifdef HAVE_STRING_H
#include <string.h>
#else
#include <strings.h>
#endif
#ifdef HAVE_CTYPE_H
#include <ctype.h>
#endif
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef HAVE_TIME_H
#include <time.h>
#endif
#ifdef HAVE_SIGNAL_H
#include <signal.h>
#endif
#ifdef HAVE_PWD_H
#include <pwd.h>
#endif
#ifdef HAVE_GRP_H
#include <grp.h>
#endif
#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#ifdef HAVE_SYS_STAT_H
#include <sys/stat.h>
#endif
#ifdef HAVE_SYS_PARAM_H
#include <sys/param.h>
#endif

#ifdef SUNOS_LIB_PROTOTYPES
/* Prototypes needed to get a clean compile with gcc -Wall.
 * Believe it or not, these do have to be declared, at least on SunOS,
 * because they aren't mentioned in the relevant system headers.
 * Sun Quality Software.  Gotta love it.
 */

int getopt (int, char **, char *);

int strcasecmp (char *, char *);
int strncasecmp (char *, char *, int);
int toupper(int);
int tolower(int);     
     
int printf (char *, ...);     
int fprintf (FILE *, char *, ...);
int fputs (char *, FILE *);
int fread (char *, int, int, FILE *);     
int fwrite (char *, int, int, FILE *);     
int fflush (FILE *);
int fclose (FILE *);
int ungetc (int, FILE *);
int _filbuf (FILE *);       /* !!! */
int _flsbuf (unsigned char, FILE *); /* !!! */
int sscanf (char *, char *, ...);
void setbuf (FILE *, char *);
void perror (char *);
     
time_t time (time_t *);
int strftime (char *, int, char *, struct tm *);
     
int initgroups (char *, int);     
int wait3 (int *, int, void*);  /* Close enough for us... */
int lstat (const char *, struct stat *);
int stat (const char *, struct stat *);     
int flock (int, int);
#ifndef NO_KILLPG
int killpg(int, int);
#endif
int socket (int, int, int);     
int setsockopt (int, int, int, const char*, int);
int listen (int, int);     
int bind (int, struct sockaddr *, int);     
int connect (int, struct sockaddr *, int);
int accept (int, struct sockaddr *, int *);
int shutdown (int, int);     

int getsockname (int s, struct sockaddr *name, int *namelen);
int getpeername (int s, struct sockaddr *name, int *namelen);
int gethostname (char *name, int namelen);     
void syslog (int, char *, ...);
char *mktemp (char *);
     
long vfprintf (FILE *, char *, va_list);
char *vsprintf (char *, char *, va_list);
     
#endif

/* debugging! */
#ifdef DEBUG_ENABLED
#ifdef HAVE_DMALLOC
#define DMALLOC_FUNC_CHECK 1
#include <dmalloc.h>
#endif
#endif


/*
**
**  OS Return Values
**
*/

#define EX__BASE        64      /* base value for error messages */
#define EX_USAGE        64      /* command line usage error */
#define EX_DATAERR      65      /* data format error */
#define EX_NOINPUT      66      /* cannot open input */
#define EX_NOUSER       67      /* addressee unknown */
#define EX_NOHOST       68      /* host name unknown */
#define EX_UNAVAILABLE  69      /* service unavailable */
#define EX_SOFTWARE     70      /* internal software error */
#define EX_OSERR        71      /* system error (e.g., can't fork) */
#define EX_OSFILE       72      /* critical OS file missing */
#define EX_CANTCREAT    73      /* can't create (user) output file */
#define EX_IOERR        74      /* input/output error */
#define EX_TEMPFAIL     75      /* temp failure; user is invited to retry */
#define EX_PROTOCOL     76      /* remote error in protocol */
#define EX_NOPERM       77      /* permission denied */
#define EX_CONFIG       78      /* configuration error */
#define EX__MAX         78      /* maximum listed value */

/* OK and FAIL exits should ALLWAYS exists */
#ifndef EX_OK
#define EX_OK   0
#endif
#ifndef EX_FAIL
#define EX_FAIL 1
#endif


/*
**
**  CU() -- CleanUp Makro (implemented in a safety way)
**
*/

#define DECL_EXRC int rc
#define EXRC rc
#define ZERO 0
#define STMT(stuff) do { stuff } while (ZERO)
#define CU(returncode) STMT( rc = returncode; goto CUS; )
#define VCU STMT( goto CUS; )
#define RETURN_WVAL(val) return (val)
#define RETURN_EXRC return (rc)
#define RETURN_NORC return


/*
**
**  ASCII Control Codes
**
*/

#define ASC_NUL '\x00'
#define ASC_SOH '\x01'
#define ASC_STX '\x02'
#define ASC_ETX '\x03'
#define ASC_EOT '\x04'
#define ASC_ENQ '\x05'
#define ASC_ACK '\x06'
#define ASC_BEL '\x07'
#define ASC_BS  '\x08'
#define ASC_HT  '\x09'
#define ASC_LF  '\x0a'
#define ASC_VT  '\x0b'
#define ASC_FF  '\x0c'
#define ASC_CR  '\x0d'
#define ASC_SO  '\x0e'
#define ASC_SI  '\x0f'
#define ASC_DLE '\x10'
#define ASC_DC1 '\x11'
#define ASC_DC2 '\x12'
#define ASC_DC3 '\x13'
#define ASC_DC4 '\x14'
#define ASC_NAK '\x15'
#define ASC_SYN '\x16'
#define ASC_ETB '\x17'
#define ASC_CAN '\x18'
#define ASC_EM  '\x19'
#define ASC_SUB '\x1a'
#define ASC_ESC '\x1b'
#define ASC_FS  '\x1c'
#define ASC_GS  '\x1d'
#define ASC_RS  '\x1e'
#define ASC_US  '\x1f'
#define ASC_SP  '\x20'
#define ASC_DEL '\x7f'
#define NUL ASC_NUL

#define ASC_QUOTE '\x22'
#define ASC_NL    ASC_LF
#define NL        ASC_NL


/*
**
**  NULL
**
*/
#ifndef NULL
#define NULL (void *)0
#endif


/*
**
**  Boolean Values -- defined in a general and portable way
**
*/

#undef  TRUE
#define TRUE  (0 || !(0))
#undef  FALSE
#define FALSE (!(TRUE))
/* typedef enum { false = FALSE, true = TRUE } bool; */

/*   some other names for true and false */

#define YES   TRUE
#define NO    FALSE

#define GOOD  TRUE
#define WRONG FALSE

#undef  OK
#define OK    TRUE
#define BAD   FALSE

#define SOME  TRUE
#define NONE  FALSE


/*
**
**  The ePerl block delimiters
**
*/

#define BEGIN_DELIMITER_FILTER "<:"
#define   END_DELIMITER_FILTER ":>"
#define BEGIN_DELIMITER_CGI    "<?"
#define   END_DELIMITER_CGI    "!>"


/*
**
**  Security
**
*/

/* General security for CGI modes */
#define CGI_NEEDS_ALLOWED_FILE_EXT       TRUE

/* SetUID security for CGI modes */
#define SETUID_NEEDS_VALID_CALLER_UID    TRUE
#define SETUID_NEEDS_ALLOWED_CALLER_UID  TRUE
#define SETUID_NEEDS_VALID_OWNER_UID     TRUE
#define SETUID_NEEDS_VALID_OWNER_GID     TRUE
#define SETUID_NEEDS_BELOW_OWNER_HOME    TRUE

#define LIST_OF_ALLOWED_FILE_EXT         { ".html", ".phtml", ".ephtml", ".epl", ".pl", ".cgi", NULL };
#define LIST_OF_ALLOWED_CALLER_UID       { "nobody", "root", NULL }

/*
**
**  The ePerl runtime mode
**
*/

#define MODE_UNKNOWN    1
#define MODE_FILTER     2
#define MODE_CGI        4
#define MODE_NPHCGI     8


#endif /* EPERL_GLOBAL_H */
/*EOF*/
