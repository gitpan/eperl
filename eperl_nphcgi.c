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
**  cgi_eperl.c -- ePerl stand-alone CGI/1.0 program
*/


/* from the Unix system */
#include <stdio.h>                          
#include <stdarg.h>                          
#include <stdlib.h>                          
#include <unistd.h>                          
#include <time.h>                          
#include <sys/types.h>
#include <sys/stat.h>
#include <pwd.h>                          

/* from the Perl interpreter distribution */
#include <EXTERN.h>
#include <perl.h>                 

/* from our own */
#include "config_ac.h"
#include "config_sc.h"
#include "eperl_lib.h"
#include "eperl_version.h"

int stdout_is_redirected = 0;
#define stdout_redirect_channel 3

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

void PrintHTTPResponse(void)
{
	char *cp;

	if ((cp = getenv("SERVER_PROTOCOL")) == NULL)
		cp = "HTTP/1.0";
    printf("%s 200 OK\n", cp);

	if ((cp = getenv("SERVER_SOFTWARE")) == NULL)
		cp = "unkown-server/0.0";
    printf("Server: %s %s Perl/%s\n", cp, ePerl_WebID, perlvers);

    printf("Date: %s\n", WebTime());
    printf("Connection: close\n");
	return;
}



/*
 *  Display an error message as a HTML page
 */
void PrintErrorHTMLPage(char *str, ...)
{
    va_list ap;
    char ca[1024];

    va_start(ap, str);
    vsprintf(ca, str, ap);

	if (stdout_is_redirected) {
       dup2(3, 1); 
	   stdout_is_redirected = 0;
	}

	PrintHTTPResponse();

    printf("Content-Type: text/html\n\n");
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
void PrintErrorHTMLPageWithLogFile(char *scripturl, char *scriptfile, char *logfile, char *str, ...)
{
    va_list ap;
    char ca[1024];
    char *cpBuf;

    va_start(ap, str);
    vsprintf(ca, str, ap);

	if (stdout_is_redirected) {
       dup2(3, 1); 
	   stdout_is_redirected = 0;
	}

	PrintHTTPResponse();

    printf("Content-Type: text/html\n\n");
    printf("<html>\n");
    printf("<body>\n");
    printf("<h1>ePerl ERROR: <font color=\"#d02020\">%s</font></h1>\n", ca);

    if ((cpBuf = ePerl_ReadErrorFile(logfile, scriptfile, scripturl)) != NULL) {
        printf("<p>");
        printf("<table bgcolor=\"#e0e0e0\" cellspacing=0 cellpadding=10 border=0>\n");
        printf("<tr><td bgcolor=\"#d0d0d0\">\n");
		printf("Contents of STDERR channel:\n");
        printf("</td></tr>\n");
        printf("<tr><td>\n");
        printf("<pre>\n");
        printf("%s", cpBuf);
        printf("</pre>");
        printf("</td></tr>\n");
        printf("</table>\n");
    }

    printf("</body>\n");
    printf("</html>\n");
    fflush(stdout);

    va_end(ap);
    return;
}


#ifdef HAVE_PERL_DYNALOADER
/*
 *  the Perl XS init function for dynamic library loading
 */
static void xs_init(void)
{
   void boot_DynaLoader _((CV* cv));
   char *file = __FILE__;

   dXSUB_SYS;
   newXS("DynaLoader::boot_DynaLoader", boot_DynaLoader, file);
}
#endif



char **mysetenv(char **env, char *var, char *str, ...)
{
    va_list ap;
    char ca[1024];
    char ca2[1024];
	char *cp;
	int i;
	char **envN;
    extern char **environ;

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
	environ = envN;

    va_end(ap);
    return envN;
}


/*
 *  main procedure
 */
int main(int argc, char **argv, char **env)
{
    DECL_EXRC;
    FILE *fp = NULL;
    FILE *er = NULL;
    FILE *out = NULL;
    char *cpBuf = NULL;
    char perlscript[1024] = "";
    char perlstderr[1024] = "";
    char perlstdout[1024] = "";
    int nBuf;
    int nOut;
    char *source;
    char sourcedir[2048];
    char *cp;
    static PerlInterpreter *my_perl = NULL; 
    struct stat st;
	char *cpOut = NULL;
    int size;
	struct passwd *pw;

    /* set source filename either from first command
       line argument (if used as a shell interpreter when debugging) or
       or from CGI/1.0 variable PATH_TRANSLATED */
    if (argc == 2)
        source = argv[1];
    else 
        source = getenv("PATH_TRANSLATED");

    /* read source file into internal buffer */
    if ((cpBuf = ePerl_ReadSourceFile(source, &cpBuf, &nBuf)) == NULL) {
        PrintErrorHTMLPage("Cannot open source file <tt>%s</tt> for reading", source);
        CU(EX_OK);
    }

	/* now set the additional env vars */
	env = mysetenv(env, "SCRIPT_SRC_SIZE", "%d", nBuf);
    stat(source, &st);
	env = mysetenv(env, "SCRIPT_SRC_MTIME", "%d", st.st_mtime);
	pw = getpwuid(st.st_uid);
	env = mysetenv(env, "SCRIPT_SRC_OWNER", "%s", pw->pw_name);
	env = mysetenv(env, "VERSION_INTERPRETER", "%s", ePerl_WebID);
	env = mysetenv(env, "VERSION_LANGUAGE", "Perl/%s", perlvers);

    /* convert bristled source to valid Perl code */
    if ((cpBuf = ePerl_Bristled2Perl(cpBuf)) == NULL) {
        PrintErrorHTMLPage("Cannot convert bristled code file <tt>%s</tt> to pure HTML", source);
        CU(EX_OK);
    }

    /* write buffer to temporary script file */
    sprintf(perlscript, "/tmp/eperl.script.%d", (int)getpid());
    unlink(perlscript);
    if ((fp = fopen(perlscript, "w")) == NULL) {
        PrintErrorHTMLPage("Cannot open Perl script file <tt>%s</tt> for writing", perlscript);
        CU(EX_OK);
    }
    fwrite(cpBuf, strlen(cpBuf), 1, fp);
    fclose(fp);

    /* open a file for Perl's STDOUT channel */
    sprintf(perlstdout, "/tmp/eperl.stdout.%d", (int)getpid());
    unlink(perlstdout);
    if ((out = fopen(perlstdout, "w")) == NULL) {
        PrintErrorHTMLPage("Cannot open STDOUT file <tt>%s</tt> for writing", perlstdout);
        CU(EX_OK);
    }
    /* redirect stdout to the new channel, but remember 
	   original stdout on channel 3 */
    dup2(1, 3); stdout_is_redirected = 1;
    dup2(fileno(out), 1);

    /* open a file for Perl's STDERR channel */
    sprintf(perlstderr, "/tmp/eperl.stderr.%d", (int)getpid());
    unlink(perlstderr);
    if ((er = fopen(perlstderr, "w")) == NULL) {
        PrintErrorHTMLPage("Cannot open STDERR file <tt>%s</tt> for writing", perlstderr);
        CU(EX_OK);
    }
    /* redirect stderr to the new channel */
    dup2(fileno(er), 2);

    /* now allocate the Perl interpreter and parse the script
       NOTICE: At this point, the script gets 
               only _parsed_, not evaluated/executed! */
    my_perl = perl_alloc();   
    perl_construct(my_perl); 
	perl_destruct_level = 0;
    argv[1] = perlscript;   
#ifdef HAVE_PERL_DYNALOADER
    rc = perl_parse(my_perl, xs_init, argc, argv, env);
#else
    rc = perl_parse(my_perl, NULL, argc, argv, env);
#endif
    if (rc != 0) { 
        fclose(er);
        PrintErrorHTMLPageWithLogFile(source, perlscript, perlstderr, "Perl parsing error (interpreter rc=%d)", rc);
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

    /*  NOW IT IS TIME to evaluate/execute the script!!! */
    rc = perl_run(my_perl);

    /*  pre-close the handles, to be able to check
        its size and to be able to display the contents */
    fclose(er); er = NULL;
    fclose(out); out = NULL;

    /*  when the Perl interpreter failed or there
        is data on stderr, we print a error page */
    if (stat(perlstderr, &st) == 0)
        size = st.st_size;
    else
        size = 0;
    if (rc != 0 || size > 0) {
        PrintErrorHTMLPageWithLogFile(source, perlscript, perlstderr, "Perl runtime error (interpreter rc=%d)", rc);
        CU(EX_OK);
    }

	/*  else all processing was fine, so
	    we read in the stdout contents */
    if ((cpOut = ePerl_ReadSourceFile(perlstdout, &cpOut, &nOut)) == NULL) {
        PrintErrorHTMLPage("Cannot open STDOUT file <tt>%s</tt> for reading", perlstdout);
        CU(EX_OK);
    }
    stat(perlstdout, &st);

	/*  because we are running as a NPH-CGI/1.0 script
	    we had to provide the HTTP reponse outself */
	PrintHTTPResponse();

	/* if there are no HTTP header lines, we print a basic
	   Content-Type header which should be ok */
	if (!ePerl_HeadersExists(cpOut)) {
        printf("Content-Type: text/html\n");
		printf("Content-Length: %d\n", nOut);
		printf("\n");
	}

	/* ok, now recover the original stdout and
	   print out the real contents on it */
    dup2(3, 1); stdout_is_redirected = 0;
    fwrite(cpOut, nOut, 1, stdout);

	/* make sure that the data is out when we exit */
	fflush(stdout);

    CUS: /* the Clean Up Sequence */

    /* Ok, the script got evaluated. Now we can destroy 
       and de-allocate the Perl interpreter */
    if (my_perl) {
       perl_destruct(my_perl);                                                    
       perl_free(my_perl);
    }

    /* close all still open file handles */
    if (out)
        fclose(out);
    if (er)
        fclose(er);
    if (fp)
        fclose(fp);

    /* remove our temporary files */
    if (*perlscript)
        unlink(perlscript);
    if (*perlstderr)
        unlink(perlstderr);
    if (*perlstdout)
        unlink(perlstdout);

    /* de-allocate the script buffer */
    if (cpBuf)
        free(cpBuf);
    if (cpOut)
        free(cpOut);

    RETURN_EXRC;
}

/*EOF*/
