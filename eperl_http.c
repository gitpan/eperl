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
**  a built-in copy of both license files.
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
void HTTP_PrintResponseHeaders(void)
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
**  strip standard HTTP reponse header lines
**
*/
void HTTP_StripResponseHeaders(char **cpBuf, int *nBuf)
{
    return;
}

/*
**  
**  check if the line is a valid HTTP header line
**
*/
int HTTP_IsHeaderLine(char *cp1, char *cp2)
{
    char *cp3;
    char *cp4;
    char ca[1024];

    strncpy(ca, cp1, cp2-cp1);
    if ((cp3 = strchr(ca, ':')) == NULL)
        return 0;
    for (cp4 = ca; cp4 < cp3; cp4++) {
        if (! ((*cp4 >= 'A' && *cp4 <= 'Z') ||
               (*cp4 >= 'a' && *cp4 <= 'z') ||
               (*cp4 >= '0' && *cp4 <= '9') ||
               (*cp4 == '-' || *cp4 == '_')   ))
            return 0;
    }
    return 1;
}

/*
**  
**  check if there is a valid HTTP header
**
*/
int HTTP_HeadersExists(char *cpBuf)
{
    char *cp1;
    char *cp2;
    char *cp3;

    if ((cp2 = strstr(cpBuf, "\n\n")) != NULL) {
        for (cp1 = cpBuf; cp1 < cp2; ) {
            cp3 = strchr(cp1, '\n');
            if (!HTTP_IsHeaderLine(cp1, cp3))
                return 0;
            cp1 = cp3+1;
        }
        return 1;
    }
    return 0;
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


/* 
**  extracts the host name from an url 
*/
static char *HTTP_HostOfURL(char *url)
{
    static char host[1024];
    char *cps;
    char *cpe;

    cps = strstr(url, "//");
    cps += 2;
    for (cpe = cps; *cpe != '/' && *cpe != ':' && *cpe != NUL; cpe++)
        ;  
    strncpy(host, cps, cpe-cps);
    host[cpe-cps] = NUL;
    return host;
}

/* 
**  extracts the port from an url 
*/
static char *HTTP_PortOfURL(char *url)
{
    static char port[128];
    char *cps;
    char *cpe;

    cps = strstr(url, "//");
    cps += 2;
    for ( ; *cps != '/' && *cps != ':' && *cps != NUL; cps++)
        ;
    if (*cps == ':') {
        cps++;
        for (cpe = cps; *cpe != '/' && *cpe != NUL; cpe++)
            ;
        strncpy(port, cps, cpe-cps);
        port[cpe-cps] = NUL;
    }
    else 
        strcpy(port, "80");
    return port;
}


/*
**  extracts a file name from a url
*/
static char *HTTP_FileOfURL(char *url)
{
    static char file[2048];
    char *cps;

    cps = strstr(url, "//");
    cps = strstr(cps+2, "/");
    if (cps == NUL) 
        strcpy(file, "/");
    else 
        strcpy(file, cps);
    return file;
}

/* 
**  open an URL as a file descriptor 
*/
FILE *HTTP_openURLasFP(char *url)
{
    struct hostent *he;
    struct sockaddr_in sar;
    struct protoent *pe;
    char cmd[1024];
    char buf[1024];
    char *host;
    char *port;
    char *file;
    int s;  
    FILE *fp;

    /* parse URL */
    host = HTTP_HostOfURL(url);
    port = HTTP_PortOfURL(url);
    file = HTTP_FileOfURL(url);

    /* get the host name */
    if ((he = gethostbyname(host)) == NULL)
        return NULL;

    /* get TCP protocol information */
    if ((pe = getprotobyname("tcp")) == NULL)
        return NULL;

    /* open the socket */
    if ((s = socket(AF_INET, SOCK_STREAM, pe->p_proto)) == -1)
        return NULL;
    
    /* fill in the socket information */
    sar.sin_family      = AF_INET;
    sar.sin_addr.s_addr = *((u_long *)(he->h_addr_list[0]));
    sar.sin_port        = htons(atoi(port));
    sar.sin_len         = 0;

    /* actually connect */
    if (connect(s, (struct sockaddr *)&sar, sizeof(sar)) == -1)
        return NULL;

    /* form the HTTP/1.0 request */
    sprintf(cmd, "GET %s HTTP/1.0\n", file);
    sprintf(cmd+strlen(cmd), "Host: %s:%s\n", host, port);
    sprintf(cmd+strlen(cmd), "User-Agent: %s\n", ePerl_WebID);
    sprintf(cmd+strlen(cmd), "\n");

    /* send the request */
    write(s, cmd, strlen(cmd));

    /* convert the file descriptor to a FILE pointer */
    fp = fdopen(s, "r");

    /* now read until a blank line, i.e. skip HTTP/1.0 headers */ 
    while (fgets(buf, sizeof(buf), fp) != NULL) {
        if ((*buf == '\n' && *(buf+1) == NUL) ||
            (*buf == '\n' && *(buf+1) == '\r' && *(buf+2) == NUL) ||
            (*buf == '\r' && *(buf+1) == '\n' && *(buf+2) == NUL))
            break;
    }

    /* return the (still open) FILE pointer */
    return fp;
}


/*EOF*/
