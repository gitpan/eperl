/*
**        ____           _ 
**    ___|  _ \ ___ _ __| |
**   / _ \ |_) / _ \ '__| |
**  |  __/  __/  __/ |  | |
**   \___|_|   \___|_|  |_|
** 
**  ePerl -- Embedded Perl 5 for HTML
**
**  ePerl interprets a HTML markup file bristled with Perl 5 program
**  statements by expanding the Perl 5 code. It operates as a stand-alone 
**  NPH-CGI/1.0 compliant Unix program and produces pure HTML markup code 
**  as the resulting data.
**
**  =====================================================================
**
**  Copyright (c) 1996,1997 Ralf S. Engelschall, All rights reserved.
**  
**  Redistribution and use in source and binary forms, with or without
**  modification, are permitted provided that the following conditions
**  are met:
**  
**  1. Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer. 
**  
**  2. Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**  
**  3. All advertising materials mentioning features or use of this
**     software must display the following acknowledgment:
**         "This product includes software developed by 
**          Ralf S. Engelschall <rse@engelschall.com>."
**  
**  4. Redistributions of any form whatsoever must retain the following
**     acknowledgment:
**         "This product includes software developed by 
**          Ralf S. Engelschall <rse@engelschall.com>."
**  
**  5. The names "ePerl" and "Embedded Perl 5 for HTML" must not be used to
**     endorse or promote products derived from this software without
**     prior written permission.
**  
**  THIS SOFTWARE IS PROVIDED BY RALF S. ENGELSCHALL ``AS IS'' AND ANY
**  EXPRESSED OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
**  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
**  ARE DISCLAIMED.  IN NO EVENT SHALL RALF S. ENGELSCHALL OR HIS CONTRIBUTORS
**  BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
**  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
**  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
**  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
**  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
**  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
**  POSSIBILITY OF SUCH DAMAGE.
**
**  =====================================================================
**
**  eperl.c -- ePerl library code file
*/



/*
**
**  Includes
**
*/

/*  from the Unix system  */
#include <stdio.h>                          
#include <stdlib.h>                          
#include <stdarg.h>                          
#include <string.h>                          
#include <unistd.h>                          

/*  from our own  */
#include "config_ac.h"
#include "config_sc.h"
#include "eperl_lib.h"
#include "eperl_version.h"



/*
**
**  Static Data
**
*/

static char ePerl_ErrorString[1024];



/*
**
**  Functions
**
*/


/*
**  set ePerl error string
*/
void ePerl_SetError(char *str, ...)
{
    va_list ap;

    va_start(ap, str);
    vsprintf(ePerl_ErrorString, str, ap);
    va_end(ap);
    return;
}

/*
**  fprintf for internal buffer 
*/
char *ePerl_fprintf(char *cpOut, char *str, ...)
{
    va_list ap;
	int n;

    va_start(ap, str);
    n = vsprintf(cpOut, str, ap);
    va_end(ap);
    return cpOut+n;
}

/*
**  fwrite for internal buffer 
*/
char *ePerl_fwrite(char *cpBuf, int nBuf, int cNum, char *cpOut)
{
	(void)strncpy(cpOut, cpBuf, nBuf*cNum);
	return (cpOut+(nBuf*cNum));
}

/*
**  fwrite for internal buffer WITH character escaping
*/
char *ePerl_Efwrite(char *cpBuf, int nBuf, int cNum, char *cpOut)
{
    char *cpI;
    char *cpO;

    for (cpI = cpBuf, cpO = cpOut; cpI < (cpBuf+(nBuf*cNum)); ) {
        switch (*cpI) {
            case '"':  *cpO++ = '\\'; *cpO++ = *cpI++;     break;
            case '@':  *cpO++ = '\\'; *cpO++ = *cpI++;     break;
            case '$':  *cpO++ = '\\'; *cpO++ = *cpI++;     break;
            case '\\': *cpO++ = '\\'; *cpO++ = *cpI++;     break;
            case '\t': *cpO++ = '\\'; *cpO++ = 't'; cpI++; break;
            case '\n': *cpO++ = '\\'; *cpO++ = 'n'; cpI++; break;
            default: *cpO++ = *cpI++;
        }
    }
	*cpO = NUL;
    return cpO;
}

/*
**  read source file into internal buffer 
*/
char *ePerl_ReadSourceFile(char *filename, char **cpBufC, int *nBufC)
{
	char *rc;
    FILE *fp = NULL;
    char *cpBuf = NULL;
    int nBuf;

    if ((fp = fopen(filename, "r")) == NULL) {
        ePerl_SetError("Cannot open source file %s for reading", filename);
        CU(NULL);
    }
    fseek(fp, 0, SEEK_END);
    nBuf = ftell(fp);
    if ((cpBuf = (char *)malloc(sizeof(char) * nBuf)) == NULL) {
        ePerl_SetError("Cannot allocate %d bytes of memory", nBuf);
        CU(NULL);
    }
    fseek(fp, 0, SEEK_SET);
    if (fread(cpBuf, nBuf, 1, fp) == 0) {
        ePerl_SetError("Cannot read from file %s", filename);
        CU(NULL);
    }
    cpBuf[nBuf] = '\0';
	*cpBufC = cpBuf;
	*nBufC  = nBuf;
    RETURN_WVAL(cpBuf);

    CUS:
    if (cpBuf)
        free(cpBuf);
    if (fp)
        fclose(fp);
    RETURN_EXRC;
}

char *ePerl_ReadErrorFile(char *filename, char *scriptfile, char *scripturl)
{
	char *rc;
    FILE *fp = NULL;
    char *cpBuf = NULL;
    int nBuf;
	char *cp;

    if ((fp = fopen(filename, "r")) == NULL) {
        ePerl_SetError("Cannot open error file %s for reading", filename);
        CU(NULL);
    }
    fseek(fp, 0, SEEK_END);
    nBuf = ftell(fp);
    if ((cpBuf = (char *)malloc(sizeof(char) * nBuf * 2)) == NULL) {
        ePerl_SetError("Cannot allocate %d bytes of memory", nBuf * 2);
        CU(NULL);
    }
    fseek(fp, 0, SEEK_SET);
    if (fread(cpBuf, nBuf, 1, fp) == 0) {
        ePerl_SetError("Cannot read from file %s", filename);
        CU(NULL);
    }
    cpBuf[nBuf] = '\0';
	for (cp = cpBuf; cp < cpBuf+nBuf; ) {
        if ((cp = strstr(cp, scriptfile)) != NULL) {
			(void)memmove(cp+strlen(scripturl), cp+strlen(scriptfile), strlen(cp+strlen(scriptfile))+1);
			(void)memcpy(cp, scripturl, strlen(scripturl));
            cp += strlen(scripturl);
			continue;
        }
		break;
	}
    RETURN_WVAL(cpBuf);

    CUS:
    if (cpBuf)
        free(cpBuf);
    if (fp)
        fclose(fp);
    RETURN_EXRC;
}

char *ePerl_Bristled2Perl(char *cpBuf)
{
    char *rc;
    char *cpOutBuf = NULL;
    char *cpOut = NULL;
    char *cps, *cpe;
    int mode;
    char *cps2, *cpe2;
	int nBuf;

    /* allocate memory for the Perl code */
    if ((cpOutBuf = (char *)malloc(sizeof(char) * strlen(cpBuf)*4)) == NULL) {
        ePerl_SetError("Cannot allocate %d bytes of memory", strlen(cpBuf)*4);
        CU(NULL);
    }
    cpOut = cpOutBuf;
	nBuf = strlen(cpBuf);

    /* start with some important inits */
    cpOut = ePerl_fprintf(cpOut, "$| = 1;");

    /* now step through the file and convert it to legal Perl code.
       This is a bit complicated because we have to make sure that
       we parse the correct delimiters while the delimiter
       characters could also occur inside the Perl code! */
    cps = cpBuf;
    while (cps < cpBuf+nBuf) {

        if ((cpe = strstr(cps, BEGIN_DELIMITER)) == NULL) {

            /* there are no more ePerl blocks, so
               just encapsulate the remaining contents into
               Perl print constructs */

            if (cps < cpBuf+nBuf) {
                cps2 = cps;
                /* first, do all complete lines */
                while ((cpe2 = strchr(cps2, '\n')) != NULL) {
                    if (cpe2 >= cpBuf+nBuf) 
                        break;
                    cpOut = ePerl_fprintf(cpOut, "print \"");
                    cpOut = ePerl_Efwrite(cps2, cpe2-cps2, 1, cpOut);
                    cpOut = ePerl_fprintf(cpOut, "\\n\";\n");
                    cps2 = cpe2+1;
                }
                /* then do the remainder which is not
                   finished by a newline */
                cpOut = ePerl_fprintf(cpOut, "print \"");
                cpOut = ePerl_Efwrite(cps2, (cpBuf+nBuf)-cps2, 1, cpOut);
                cpOut = ePerl_fprintf(cpOut, "\";");
            }
            break; /* and break the whole processing step */

        }
        else {

            /* Ok, there is at least one more ePerl block */

            /* first, encapsulate the content from current pos
               up to the begin of the ePerl block as print statements */
            if (cps < cpe) {
                cps2 = cps;
                while ((cpe2 = strchr(cps2, '\n')) != NULL) {
                    if (cpe2 >= cpe) 
                        break;
                    cpOut = ePerl_fprintf(cpOut, "print \"");
                    cpOut = ePerl_Efwrite(cps2, cpe2-cps2, 1, cpOut);
                    cpOut = ePerl_fprintf(cpOut, "\\n\";\n");
                    cps2 = cpe2+1;
                }
                cpOut = ePerl_fprintf(cpOut, "print \"");
                cpOut = ePerl_Efwrite(cps2, cpe-cps2, 1, cpOut);
                cpOut = ePerl_fprintf(cpOut, "\";");
            }

            /* skip the start delimiter and all following whitespaces.
               Be careful: we can skip newlines too, but then the
               error output will give wrong line numbers!!! */
            cps = cpe+strlen(BEGIN_DELIMITER);
            while (cps < cpBuf+nBuf) {
                if (*cps != ' ' && *cps != '\t')
                    break;
                cps++;
            }
            cpe = cps;

            /* now we have to determine the _REAL_ end of the ePerl
               block. Be careful: the end delimiter can also occur
               inside the ePerl block!! */
            mode = 0; /* 0 outer, 1 inner "" */
            while (cpe < cpBuf+nBuf) {

                /* if we are outside a ".." and a '"' occur, then we are now inside */
                if (mode == 0 && *(cpe-1) != '\\' && *cpe == '"') {
                    mode = 1;
                    cpe++;
                    continue;
                }
                /* if we are inside a ".." and a '"' occur, then we are now outside */
                if (mode == 1 && *(cpe-1) != '\\' && *cpe == '"') {
                    mode = 0;
                    cpe++;
                    continue;
                }

                if (strncmp(cpe, END_DELIMITER, strlen(END_DELIMITER)) != 0) {
                    cpe++;
                    continue;
                }
                break;
            }
            
            /* pass through the ePerl block without changes! */
            cpOut = ePerl_fwrite(cps, cpe-cps, 1, cpOut);

            /* and adjust the current position to the first character
               after the end delimiter */
            cps = cpe+strlen(END_DELIMITER);
        }
    }
    RETURN_WVAL(cpOutBuf);

    CUS:
    if (cpBuf) 
        free(cpBuf);
    RETURN_EXRC;
}

int ePerl_IsHeaderLine(char *cp1, char *cp2)
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

int ePerl_HeadersExists(char *cpBuf)
{
	char *cp1;
	char *cp2;
	char *cp3;

	if ((cp2 = strstr(cpBuf, "\n\n")) != NULL) {
		for (cp1 = cpBuf; cp1 < cp2; ) {
			cp3 = strchr(cp1, '\n');
			if (!ePerl_IsHeaderLine(cp1, cp3))
				return 0;
			cp1 = cp3+1;
		}
		return 1;
	}
	return 0;
}


/*EOF*/
