/*
**        ____           _ 
**    ___|  _ \ ___ _ __| |
**   / _ \ |_) / _ \ '__| |
**  |  __/  __/  __/ |  | |
**   \___|_|   \___|_|  |_|
** 
**  ePerl -- Embedded Perl 5 for HTML
**
**  ePerl interprets a HTML markup file bristled with Perl5 program statements
**  by expanding the Perl5 code. It is a CGI/1.0 compliant program which
**  produces pure HTML markup as its result on stdout.
**
**  =====================================================================
**
**  Copyright (c) Ralf S. Engelschall, All rights reserved.
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
**  5. The names "ePerl" and "embedded Perl 5 for HTML" must not be used to
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
*/


/* from the Unix system */
#include <stdio.h>                          
#include <stdarg.h>                          
#include <unistd.h>                          

/* from the Perl interpreter distribution */
#include <EXTERN.h>
#include <perl.h>                 

/* from our own */
#include "eperl.h"


/*
 *  Display an error message as a HTML page
 */
void DoError(char *str, ...)
{
    va_list ap;
    char ca[1024];

    va_start(ap, str);
    vsprintf(ca, str, ap);

    printf("Content-type: text/html\n\n");
    printf("<html>\n");
    printf("<body>\n");
    printf("<h1>ePerl ERROR: <font color=\"#d02020\">%s</font></h1>\n", ca);
    printf("</body>\n");
    printf("</html>\n");
    fflush(stdout);

    va_end(ap);
    return;
}


/*
 *  Display an error message and a logfile content as a HTML page
 */
void DoErrorWithLogFile(char *scripturl, char *scriptfile, char *logfile, char *str, ...)
{
    va_list ap;
    char ca[1024];
    FILE *fp;
    char buf[1024];
    char *cpbuf;
    int n;
    char *cp;

    va_start(ap, str);
    vsprintf(ca, str, ap);

    printf("Content-type: text/html\n\n");
    printf("<html>\n");
    printf("<body>\n");
    printf("<h1>ePerl ERROR: <font color=\"#d02020\">%s</font></h1>\n", ca);

    if ((fp = fopen(logfile, "r")) != NULL) {
        printf("<p><hr><pre>\n");
        while ((n = fread(buf, 1, 1024, fp)) > 0) {
            cpbuf = buf;
            label:
            if ((cp = strstr(cpbuf, scriptfile)) != NULL) {
                *cp = '\0';
                printf("%s%s", cpbuf, scripturl);
                cp += strlen(scriptfile);
                n = n-(cp-cpbuf);
                cpbuf = cp;
                goto label;
            }
            else {
                fwrite(cpbuf, 1, n, stdout);
            }
        }
        printf("</pre><hr><p>\n");
        fclose(fp);
    }

    printf("</body>\n");
    printf("</html>\n");
    fflush(stdout);

    va_end(ap);
    return;
}


/*
 *  special fwrite() variant: escapes characters
 */
void escfwrite(char *cpBuf, int nBuf, int count, FILE *fp)
{
    char *cpIntBuf = NULL;
    char *cpI;
    char *cpO;

    if ((cpIntBuf = (char *)malloc(nBuf*count*2)) == NULL)
        VCU;
    for (cpI = cpBuf, cpO = cpIntBuf; cpI < (cpBuf+(nBuf*count)); ) {
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
    fwrite(cpIntBuf, cpO-cpIntBuf, 1, fp);

    CUS:
    if (cpIntBuf)
        free(cpIntBuf);
    return;
}
              

/*
 *  main procedure
 */
int main(int argc, char **argv, char **env)
{
    DECL_EXRC;
    FILE *fp = NULL;
    FILE *er = NULL;
    char *cpBuf = NULL;
    char perlscript[1024] = "";
    char perlstderr[1024] = "";
    int nBuf;
    char *source;
    char sourcedir[2048];
    char *cp, *cps, *cpe;
    int mode;
    char *cps2, *cpe2;
    static PerlInterpreter *my_perl = NULL; 


    /* set source filename either from first command
       line argument (if used as a shell interpreter when debugging) or
       or from CGI/1.0 variable PATH_TRANSLATED */
    if (argc == 2)
        source = argv[1];
    else 
        source = getenv("PATH_TRANSLATED");


    /* read source file into internal buffer */
    if ((fp = fopen(source, "r")) == NULL) {
        DoError("Cannot open source file <tt>%s</tt> for reading", source);
        CU(EX_OK);
    }
    fseek(fp, 0, SEEK_END);
    nBuf = ftell(fp);
    if ((cpBuf = (char *)malloc(sizeof(char) * nBuf)) == NULL) {
        DoError("Cannot allocate %d bytes of memory", nBuf);
        CU(EX_OK);
    }
    fseek(fp, 0, SEEK_SET);
    if (fread(cpBuf, nBuf, 1, fp) == 0) {
        DoError("Cannot read from file <tt>%s</tt>", source);
        CU(EX_OK);
    }
    cpBuf[nBuf] = '\0';
    fclose(fp); fp = NULL;


    /* if really no ePerl constructs exists just output the source
       file without evaluation and exit immediately */
    if (strstr(cpBuf, "<?") == NULL) {
        printf("Content-type: text/html\n\n");
        fwrite(cpBuf, nBuf, 1, stdout);
        fflush(stdout);
        CU(EX_OK);
    }
    /* else: convert source file into a Perl script and evaluate it 
       via the Perl 5 interpreter library... */


    /* open temporary script file */
    sprintf(perlscript, "/tmp/epl.scr.%d", (int)getpid());
    unlink(perlscript);
    if ((fp = fopen(perlscript, "w")) == NULL) {
        DoError("Cannot open Perl script file <tt>%s</tt> for writing", perlscript);
        CU(EX_OK);
    }


    /* some important inits */
    fprintf(fp, "$| = 1;");

    /* now step through the file and convert it to legal Perl code.
       This is a bit complicated because we habe to make sure that
       we parse the correct delimiters <? ... > while the delimiter
       characters could also occur inside Perl constructs! */
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
                    fprintf(fp, "print \"");
                    escfwrite(cps2, cpe2-cps2, 1, fp);
                    fprintf(fp, "\\n\";\n");
                    cps2 = cpe2+1;
                }
                /* then do the remainder which is not
                   finished by a newline */
                fprintf(fp, "print \"");
                escfwrite(cps2, (cpBuf+nBuf)-cps2, 1, fp);
                fprintf(fp, "\";");
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
                    fprintf(fp, "print \"");
                    escfwrite(cps2, cpe2-cps2, 1, fp);
                    fprintf(fp, "\\n\";\n");
                    cps2 = cpe2+1;
                }
                fprintf(fp, "print \"");
                escfwrite(cps2, cpe-cps2, 1, fp);
                fprintf(fp, "\";");
            }

            /* skip the <? delimiter and all following whitespaces.
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
               block. Be careful: the end delimiter ">" can also occur
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
            fwrite(cps, cpe-cps, 1, fp);

            /* and adjust the current position to the first character
               after the end delimiter */
            cps = cpe+strlen(END_DELIMITER);
        }
    }
    fclose(fp); fp = NULL;


    /* open a file for Perls stderr channel */
    sprintf(perlstderr, "/tmp/epl.err.%d", (int)getpid());
    unlink(perlstderr);
    if ((er = fopen(perlstderr, "w")) == NULL) {
        DoError("Cannot open error log file <tt>%s</tt> for writing", perlstderr);
        CU(EX_OK);
    }
    /* redirect stderr to the new channel */
    dup2(fileno(er), 2);


    /* now allocate the Perl interpreter and parse the script
       NOTICE: At this point, the script gets 
               only _parsed_, not evaluated/executed! */
    my_perl = perl_alloc();   
    perl_construct(my_perl); 
    argv[1] = perlscript;   
    if ((rc = perl_parse(my_perl, NULL, argc, argv, env)) != 0) { 
        fclose(er);
        DoErrorWithLogFile(source, perlscript, perlstderr, "Perl parsing error (interpreter rc=%d)", rc);
        CU(EX_OK);
    }


    /* change to directory of script:
       this actually is not important to us, but really useful 
       for the ePerl source file programmer!! */
    strcpy(sourcedir, source);
    for (cp = sourcedir+strlen(sourcedir); cp > sourcedir && *cp != '/'; cp--)
        ;
    *cp = '\0';
    chdir(sourcedir);


    /* if the ePerl source file doesn't directly begin with a 
       ePerl block, we have to write out the content type before 
       running the script. In other words: If you begin your
       ePerl source file directly with a ePerl block then
       you have to or are able to output the content-type yourself!! */
    if (strncmp(cpBuf, "<?", 2) != 0) 
        printf("Content-type: text/html\n\n");


    /* NOW IT IS TIME to evaluate/execute the script!!! */
    if ((rc = perl_run(my_perl)) != 0) {
        fclose(er); /* pre-close the stderr handle, to be able to display the logfile */
        er = NULL;
        DoErrorWithLogFile(source, perlscript, perlstderr, "Perl runtime error (interpreter rc=%d)", rc);
        CU(EX_OK);
    }


    CUS: /* the Clean Up Sequence */

    /* Ok, the script got evaluated. Now we can destroy 
       and de-allocate the Perl interpreter */
    if (my_perl) {
       perl_destruct(my_perl);                                                    
       perl_free(my_perl);
    }

    /* close all still open file handles */
    if (er)
        fclose(er);
    if (fp)
        fclose(fp);

    /* remove our temporary files */
    if (*perlscript)
        unlink(perlscript);
    if (*perlstderr)
        unlink(perlstderr);

    /* de-allocate the script buffer */
    if (cpBuf)
        free(cpBuf);

    RETURN_EXRC;
}

/*EOF*/
