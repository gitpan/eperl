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
**  eperl_http.c -- ePerl HTTP stuff
*/

#include "eperl_global.h"
#include "eperl_proto.h"

/*
**  
**  print a standard HTTP reponse of header lines
**
*/
void PrintHTTPResponse(void)
{
    char *cp;

    if ((cp = getenv("SERVER_PROTOCOL")) == NULL)
        cp = "HTTP/1.0";
    printf("%s 200 OK\n", cp);

    if ((cp = getenv("SERVER_SOFTWARE")) == NULL)
        cp = "unknown-server/0.0";
    printf("Server: %s %s Perl/%s\n", cp, ePerl_WebID, AC_perlvers);

    printf("Date: %s\n", WebTime());
    printf("Connection: close\n");
    return;
}


/*
**
**  Give back acceptable HTTP time format string
**
*/

char *WebTime(void)
{
    time_t t;
    struct tm *tm;
    char *cp;

    t = time(&t);
    tm = localtime(&t);
    cp = ctime(&t);
    cp[strlen(cp)-1] = NUL;
    return cp;
}

/*EOF*/
