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
**  NPH-CGI/1.1 compliant Unix program and produces pure HTML markup code 
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
**  eperl_nphcgi.c -- ePerl stand-alone NPH-CGI/1.1 program
*/

#include "eperl_global.h"
#include "eperl_proto.h"

#include <EXTERN.h>
#include <perl.h>                 

int mode = MODE_UNKNOWN;

char *begin_delimiter = BEGIN_DELIMITER;
char   *end_delimiter =   END_DELIMITER;

char *allowed_file_ext[] = ALLOWED_FILE_EXT;

/*
 *  Display an error message and a logfile content as a HTML page
 */
void PrintError(int mode, char *scripturl, char *scriptfile, char *logfile, char *str, ...)
{
    va_list ap;
    char ca[1024];
    char *cpBuf;
    char *cp;

    va_start(ap, str);
    vsprintf(ca, str, ap);

    IO_restore_stdout();

    if (mode == MODE_CGI || mode == MODE_NPHCGI) {
        if (mode == MODE_NPHCGI)
            PrintHTTPResponse();
        printf("Content-Type: text/html\n\n");
        printf("<html>\n");
        printf("<head>\n");
        printf("<title>ePerl: ERROR: %s</title>\n", ca);
        printf("</head>\n");
        printf("<body bgcolor=\"#cccccc\">\n");
        printf("<blockquote>\n");
        cp = getenv("SCRIPT_NAME");
        if (cp == NULL)
            cp = "UNKNOWN_IMG_DIR";
        printf("<table cellspacing=0 cellpadding=0 border=0>\n");
		printf("<tr>\n");
        printf("<td><img src=\"%s/logo.gif\" alt=\"Embedded Perl 5 Language\"></td>\n", cp);
		printf("</tr>\n");
		printf("<tr>\n");
        printf("<td align=right><b>Version %s</b></td>\n", ePerl_Version);
		printf("</tr>\n");
		printf("</table>\n");
        printf("<p>\n");
        printf("<table bgcolor=\"#f0d0d0\" cellspacing=0 cellpadding=10 border=0>\n");
        printf("<tr><td bgcolor=\"#d0c0c0\">\n");
        printf("<font face=\"Arial, Helvetica\"><b>ERROR message:</b></font>\n");
        printf("</td></tr>\n");
        printf("<tr><td>\n");
        printf("<h1><font color=\"#cc3333\">%s</font></h1>\n", ca);
        printf("</td></tr>\n");
        printf("</table>\n");
        if (logfile != NULL) {
            if ((cpBuf = ePerl_ReadErrorFile(logfile, scriptfile, scripturl)) != NULL) {
                printf("<p>");
                printf("<table bgcolor=\"#e0e0e0\" cellspacing=0 cellpadding=10 border=0>\n");
                printf("<tr><td bgcolor=\"#d0d0d0\">\n");
                printf("<font face=\"Arial, Helvetica\"><b>Contents of STDERR channel:</b></font>\n");
                printf("</td></tr>\n");
                printf("<tr><td>\n");
                printf("<pre>\n");
                printf("%s", cpBuf);
                printf("</pre>");
                printf("</td></tr>\n");
                printf("</table>\n");
            }
        }
        printf("</blockquote>\n");
        printf("</body>\n");
        printf("</html>\n");
    }
    else {
        printf("ERROR message: %s\n", ca);
        if (logfile != NULL) {
            if ((cpBuf = ePerl_ReadErrorFile(logfile, scriptfile, scripturl)) != NULL) {
                printf("\n");
                printf("---- Contents of STDERR channel: ---------\n");
                printf("%s", cpBuf);
                if (cpBuf[strlen(cpBuf)-1] != '\n')
                    printf("\n");
                printf("------------------------------------------\n");
            }
        }
    }
    fflush(stdout);
    
    va_end(ap);
    return;
}

void give_version(void)
{
    fprintf(stderr, "%s\n", ePerl_Hello);
}

void give_version_extended(void)
{
    fprintf(stderr, "%s\n", ePerl_Hello);
    fprintf(stderr, "\n");
    fprintf(stderr, "Compilation Parameters:\n");
    fprintf(stderr, "  Perl 5 Version    : %s (%s)\n", AC_perlvers, AC_perlprog);
    fprintf(stderr, "  Perl 5 Library    : %s/CORE/libperl.a\n", AC_perlarch);
    fprintf(stderr, "  Perl 5 DynaLoader : %s\n", AC_perldla);
    fprintf(stderr, "  Additional Libs   : %s\n", AC_perllibs);
    fprintf(stderr, "\n");
}

void give_readme(void)
{
    fprintf(stderr, ePerl_README);
}

void give_license(void)
{
    fprintf(stderr, ePerl_LICENSE);
}

void give_img_logo(void)
{
    if (mode == MODE_NPHCGI) {
        PrintHTTPResponse();
    }
    printf("Content-Type: image/gif\n\n");
    fwrite(ePerl_LOGO_data, ePerl_LOGO_size, 1, stdout);
}

void give_usage(char *name)
{
    give_version();
    fprintf(stderr, "Usage: %s [options] [file]\n", name);
    fprintf(stderr, "where options are:\n");
    fprintf(stderr, "   -m f|c|n       force runtime mode to FILTER, CGI or NPH-CGI\n");
    fprintf(stderr, "   -x             force debug mode\n");
    fprintf(stderr, "   -b str         set begin delimiter (default is '%s')\n", BEGIN_DELIMITER);
    fprintf(stderr, "   -e str         set   end delimiter (default is '%s')\n", END_DELIMITER);
    fprintf(stderr, "   -D name=value  define global Perl variable ($main::name)\n");
    fprintf(stderr, "   -E name=value  define environment variable ($ENV{'name'})\n");
    fprintf(stderr, "   -o outputfile  force the output to be send to this file (default is stdout)\n");
    fprintf(stderr, "   -k             keep current working directory\n");
    fprintf(stderr, "   -r             display ePerl README file\n");
    fprintf(stderr, "   -l             display ePerl LICENSE file\n");
    fprintf(stderr, "   -v             display ePerl VERSION id\n");
    fprintf(stderr, "   -V             display ePerl VERSION id & compilation parameters\n");
}

char **set_variable(char **env, char *str) 
{
    char ca[1024];
    char *cp;

    strcpy(ca, str);
    cp = strchr(ca, '=');
    *cp++ = '\0';
    return mysetenv(env, ca, cp);
}

char *remembered_perl_scalars[100] = { NULL };

void remember_perl_scalar(char *str) 
{
    int i;

    for (i = 0; remembered_perl_scalars[i] != NULL; i++)
        ;
    remembered_perl_scalars[i++] = strdup(str);
    remembered_perl_scalars[i++] = NULL;
    return;
}

void set_perl_scalars(void) 
{
    char ca[1024];
    char *cp;
    int i;

    for (i = 0; remembered_perl_scalars[i] != NULL; i++) {
        strcpy(ca, remembered_perl_scalars[i]);
        cp = strchr(ca, '=');
        *cp++ = '\0';
        ePerl_SetScalar("main", ca, cp);
    }
}

void mysighandler(int rc)
{
    /* ignore more signals */
    signal(SIGINT,  SIG_IGN);
    signal(SIGTERM, SIG_IGN);

    /* restore filehandles */
    IO_restore_stdout();
    IO_restore_stderr();

    /* give interrupt information */
    fprintf(stderr, "ePerl: **INTERRUPT**\n");

    /* exit immediately */
    myexit(EX_FAIL);
}

void myinit(void)
{
    /* caught signals */
    signal(SIGINT,  mysighandler);
    signal(SIGTERM, mysighandler);
}

void myexit(int rc)
{
    /* cleanup */
#ifndef DEBUG_ENABLED
    remove_mytmpfiles();
#endif

    /* restore signals */
    signal(SIGINT,  SIG_DFL);
    signal(SIGTERM, SIG_DFL);

#ifdef DEBUG_ENABLED
#ifdef HAVE_DMALLOC
    dmalloc_shutdown();
#endif
#endif

    /* die gracefully */
    exit(rc);
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
    char *cpBuf2 = NULL;
    char perlscript[1024] = "";
    char perlstderr[1024] = "";
    char perlstdout[1024] = "";
    char ca[1024] = "";
    int myargc;
    char *myargv[20];
    char *progname;
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
    struct group *gr;
    int uid, gid;
    int keepcwd = FALSE;
    char c;
    char *cpScript = NULL;
    int allow;
    int i, n, k;
    char *outputfile = NULL;
    char cwd[MAXPATHLEN];


    /*  first step: our process initialisation */
    myinit();

    /*  second step: canonicalize program name */
    progname = argv[0];
    if ((cp = strrchr(progname, '/')) != NULL) {
        progname = cp+1;
    }

    /*  set behaviour for egetopt() function */
    optneed  = (int)'+';    /* use '+' instead of ':' */
    optmaybe = (int)'?';    /* use '?' for optional arguments */
    optbad   = (int)'!';    /* use '!' for bad options */
    optstart = "-+";

    /*  parse the option arguments */
    while ((c = egetopt(argc, argv, "m+b+e+D+E+a+o+kxvVrl")) != EOF) {
        if (optarg == NULL) 
            optarg = "(null)";
        switch (c) {
            case 'm':
                if (strcasecmp(optarg, "f") == 0) {
                    mode = MODE_FILTER;
                }
                else if (strcasecmp(optarg, "c") == 0) {
                    mode = MODE_CGI;
                }
                else if (strcasecmp(optarg, "n") == 0) {
                    mode = MODE_NPHCGI;
                }
                else {
                    PrintError(mode, "", NULL, NULL, "Unknown runtime mode `%s'", optarg);
                    CU(EX_USAGE);
                }
                break;
            case 'b':
                begin_delimiter = strdup(optarg);
                break;
            case 'e':
                end_delimiter = strdup(optarg);
                break;
            case 'D':
                remember_perl_scalar(optarg);
                break;
            case 'E':
                env = set_variable(env, optarg);
                break;
            case 'o':
                outputfile = strdup(optarg);
                break;
            case 'k':
                keepcwd = TRUE;
                break;
            case 'x':
                fDebug = TRUE;
                break;
            case 'v':
                give_version();
                myexit(EX_OK);
            case 'V':
                give_version_extended();
                myexit(EX_OK);
            case 'r':
                give_readme();
                myexit(EX_OK);
            case 'l':
                give_license();
                myexit(EX_OK);
            case '!':
                give_usage(progname);
                myexit(EX_USAGE);
        }
    }

    /* determine source filename and runtime mode */
    if (optind == argc-1) {

        /*  stand-alone filter, source as argument:
            either manually on the console or via shebang */
        source = argv[optind];
        mode   = (mode == MODE_UNKNOWN ? MODE_FILTER : mode);

        /*  provide flexibility by recognizing "-" for stdin */
        if (strcmp(source, "-") == 0) {
            /* store stdin to tmpfile */
            source = mytmpfile("ePerl.stdin");
            if ((fp = fopen(source, "w")) == NULL) {
                PrintError(mode, source, NULL, NULL, "Cannot open tmpfile `%s' for writing", source);
                CU(EX_IOERR);
            }
            while ((c = fgetc(stdin)) != EOF) {
                fputc(c, fp);
            }
            fclose(fp);

            /* stdin script implies keeping of cwd */
            keepcwd = TRUE;
        }
    }
    else if (optind == argc && (cp = getenv("PATH_TRANSLATED")) != NULL) { 

        /*  CGI or NPH-CGI script, source in PATH_TRANSLATED:
            via Webserver request */
        source = cp;

        /*  determine whether pure CGI or NPH-CGI mode */ 
        if ((cp = getenv("SCRIPT_FILENAME")) != NULL) { 
            strcpy(ca, cp);
            if ((cp = strrchr(ca, '/')) != NULL) 
                *cp++ = NUL;
            else 
                cp = ca;
            if (strncasecmp(cp, "nph-", 4) == 0) 
                mode = (mode == MODE_UNKNOWN ? MODE_NPHCGI : mode);
            else
                mode = (mode == MODE_UNKNOWN ? MODE_CGI : mode);
        }
        else {
            mode = (mode == MODE_UNKNOWN ? MODE_CGI : mode);
        }
    }
    else {
        give_usage(progname);
        myexit(EX_USAGE);
    }

    /* the builtin GIF images */
    if ((mode == MODE_CGI || mode == MODE_NPHCGI) && (cp = getenv("PATH_INFO")) != NULL) { 
        if (strcmp(cp, "/logo.gif") == 0) {
            give_img_logo();
            myexit(0);
        }
    }

    /* check for valid source file */
    if ((stat(source, &st)) != 0) {
        PrintError(mode, source, NULL, NULL, "File `%s' not exists", source);
        CU(EX_OK);
    }

    /*
     * Security Checks for the CGI modes
     */
    if (mode == MODE_CGI || mode == MODE_NPHCGI) {

        /* security check: valid user */
        uid = getuid();
        if ((pw = getpwuid(uid)) == NULL) {
            PrintError(mode, source, NULL, NULL, "Invalid UID %d", uid);
            CU(EX_OK);
        }

        /* security check: UID and GID */
        if ((pw = getpwuid(st.st_uid)) == NULL) {
            PrintError(mode, source, NULL, NULL, "Invalid UID %d of file owner", st.st_uid);
            CU(EX_OK);
        }
        uid = pw->pw_uid;
        if ((gr = getgrgid(st.st_gid)) == NULL) {
            PrintError(mode, source, NULL, NULL, "Invalid GID %d of file group", st.st_gid);
            CU(EX_OK);
        }
        gid = gr->gr_gid;

        /* security check: allowed file extension */
        allow = FALSE;
        n = strlen(source);
        for (i = 0; allowed_file_ext[i] != NULL; i++) {
            k = strlen(allowed_file_ext[i]);
            if (strcmp(source+n-k, allowed_file_ext[i]) == 0) 
                allow = TRUE;
        }
        if (!allow) {
            PrintError(mode, source, NULL, NULL, "File `%s' is not allowed to be interpreted by ePerl (wrong extension!)", source);
            CU(EX_OK);
        }

        /* switch to uid/gid if run as a setuid program */
        if (geteuid() == 0 && uid != 0 && gid != 0 /* XXX */) {
            /* XXX */
            if (((setgid(gid)) != 0) || (initgroups(pw->pw_name,gid) != 0)) {
                PrintError(mode, source, NULL, NULL, "Failed to set GID %d", gid);
                CU(EX_OK);
            }
            if ((setuid(uid)) != 0) {
                PrintError(mode, source, NULL, NULL, "Failed to set UID %d", uid);
                CU(EX_OK);
            }
    
            /* eliminate effective uid/gid */
            seteuid(uid);
            setegid(gid);
        }
    }

    /* read source file into internal buffer */
    if ((cpBuf = ePerl_ReadSourceFile(source, &cpBuf, &nBuf)) == NULL) {
        PrintError(mode, source, NULL, NULL, "Cannot open source file `%s' for reading\n%s", source, ePerl_GetError);
        CU(EX_OK);
    }

    /* strip shebang prefix */
    if (strncmp(cpBuf, "#!", 2) == 0) {
        for (cpScript = cpBuf;
             (*cpScript != ' ' && *cpScript != '\t' && *cpScript != '\n') && (cpScript-cpBuf < nBuf);
             cpScript++)
            ;
        /* XXX perhaps add code to again getopt() the stuff */
        for (cpScript = cpBuf;
             *cpScript != '\n' && (cpScript-cpBuf < nBuf);
             cpScript++)
            ;
        cpScript++;
    }
    else
        cpScript = cpBuf;

    /* now set the additional env vars */
    env = mysetenv(env, "SCRIPT_SRC_SIZE", "%d", nBuf);
    stat(source, &st);
    env = mysetenv(env, "SCRIPT_SRC_MTIME", "%d", st.st_mtime);
    pw = getpwuid(st.st_uid);
    env = mysetenv(env, "SCRIPT_SRC_OWNER", "%s", pw->pw_name);
    env = mysetenv(env, "VERSION_INTERPRETER", "%s", ePerl_WebID);
    env = mysetenv(env, "VERSION_LANGUAGE", "Perl/%s", AC_perlvers);

    /* convert bristled source to valid Perl code */
    if ((cpBuf2 = ePerl_Bristled2Perl(cpScript)) == NULL) {
        PrintError(mode, source, NULL, NULL, "Cannot convert bristled code file `%s' to pure HTML", source);
        CU(EX_OK);
    }
    cpScript = cpBuf2;

    /* write buffer to temporary script file */
    strcpy(perlscript, mytmpfile("ePerl.script"));
#ifndef DEBUG_ENABLED
    unlink(perlscript);
#endif
    if ((fp = fopen(perlscript, "w")) == NULL) {
        PrintError(mode, source, NULL, NULL, "Cannot open Perl script file `%s' for writing", perlscript);
        CU(EX_OK);
    }
    fwrite(cpScript, strlen(cpScript), 1, fp);
    fclose(fp);

    /* open a file for Perl's STDOUT channel
       and redirect stdout to the new channel */
    strcpy(perlstdout, mytmpfile("ePerl.stdout"));
#ifndef DEBUG_ENABLED
    unlink(perlstdout);
#endif
    if ((out = fopen(perlstdout, "w")) == NULL) {
        PrintError(mode, source, NULL, NULL, "Cannot open STDOUT file `%s' for writing", perlstdout);
        CU(EX_OK);
    }
    IO_redirect_stdout(out);

    /* open a file for Perl's STDERR channel 
       and redirect stderr to the new channel */
    strcpy(perlstderr, mytmpfile("ePerl.stderr"));
#ifndef DEBUG_ENABLED
    unlink(perlstderr);
#endif
    if ((er = fopen(perlstderr, "w")) == NULL) {
        PrintError(mode, source, NULL, NULL, "Cannot open STDERR file `%s' for writing", perlstderr);
        CU(EX_OK);
    }
    IO_redirect_stderr(er);

    /* now allocate the Perl interpreter */
    my_perl = perl_alloc();   
    perl_construct(my_perl); 
    /* perl_destruct_level = 0; */
    /* perl_init_i18nl10n(1); */

    /* now parse the script
       NOTICE: At this point, the script gets 
               only _parsed_, not evaluated/executed! */
    myargc = 0;
    myargv[myargc++] = progname;
    myargv[myargc++] = perlscript;   
#ifdef HAVE_PERL_DYNALOADER
    rc = perl_parse(my_perl, ePerl_xs_init, myargc, myargv, env);
#else
    rc = perl_parse(my_perl, NULL, myargc, myargv, env);
#endif
    if (rc != 0) { 
        fclose(er);
        PrintError(mode, source, perlscript, perlstderr, "Perl parsing error (interpreter rc=%d)", rc);
        CU(EX_OK);
    }

    /* change to directory of script:
       this actually is not important to us, but really useful 
       for the ePerl source file programmer!! */
    cwd[0] = NUL;
    if (!keepcwd) {
        /* if running as a Unix filter remember the cwd for outputfile */
        if (mode == MODE_FILTER)
            getcwd(cwd, MAXPATHLEN);
        /* determine dir of source file and switch to it */
        strcpy(sourcedir, source);
        for (cp = sourcedir+strlen(sourcedir); cp > sourcedir && *cp != '/'; cp--)
            ;
        *cp = '\0';
        chdir(sourcedir);
    }

    /*  NOW IT IS TIME to evaluate/execute the script!!! */
    /* TAINT; */
    set_perl_scalars();
    ePerl_ForceUnbufferedStdout();
    rc = perl_run(my_perl);
    /* TAINT_NOT; */

    /*  pre-close the handles, to be able to check
        its size and to be able to display the contents */
    fclose(out); out = NULL;
    fclose(er);  er  = NULL;


    /*  when the Perl interpreter failed or there
        is data on stderr, we print a error page */
    if (stat(perlstderr, &st) == 0)
        size = st.st_size;
    else
        size = 0;
    if (rc != 0 || size > 0) {
        PrintError(mode, source, perlscript, perlstderr, "Perl runtime error (interpreter rc=%d)", rc);
        CU(EX_OK);
    }

    /*  else all processing was fine, so
        we read in the stdout contents */
    if ((cpOut = ePerl_ReadSourceFile(perlstdout, &cpOut, &nOut)) == NULL) {
        PrintError(mode, source, NULL, NULL, "Cannot open STDOUT file `%s' for reading", perlstdout);
        CU(EX_OK);
    }
    stat(perlstdout, &st);

    /*  if we are running as a NPH-CGI/1.1 script
        we had to provide the HTTP reponse headers ourself */
    if (mode == MODE_NPHCGI) {
        PrintHTTPResponse();

        /* if there are no HTTP header lines, we print a basic
           Content-Type header which should be ok */
        if (!ePerl_HeadersExists(cpOut)) {
            printf("Content-Type: text/html\n");
            printf("Content-Length: %d\n", nOut);
            printf("\n");
        }
    }
    else if (mode == MODE_CGI) {
        ePerl_StripHTTPHeaders(&cpOut, &nOut);

        /* if there are no HTTP header lines, we print a basic
           Content-Type header which should be ok */
        if (!ePerl_HeadersExists(cpOut)) {
            printf("Content-Type: text/html\n");
            printf("Content-Length: %d\n", nOut);
            printf("\n");
        }
    }
    else if (mode == MODE_FILTER) {
        ePerl_StripHTTPHeaders(&cpOut, &nOut);
    }

    /* ok, now recover the stdout and stderr and
       print out the real contents on stdout or outputfile */
    IO_restore_stdout();
    IO_restore_stderr();

    if (outputfile != NULL && strcmp(outputfile, "-") != 0) {
        /* if we remembered current working dir, restore it now */
        if (mode == MODE_FILTER && cwd[0] != NUL)
            chdir(cwd);
        /* open outputfile and write out the data */
        if ((fp = fopen(outputfile, "w")) == NULL) {
            PrintError(mode, source, NULL, NULL, "Cannot open output file `%s' for writing", outputfile);
            CU(EX_OK);
        }
        fwrite(cpOut, nOut, 1, fp);
        fclose(fp);
    }
    else {
        /* data just goes to stdout */
        fwrite(cpOut, nOut, 1, stdout);
    }

    /* make sure that the data is out before we exit */
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

    /* de-allocate the script buffer */
    if (cpBuf)
        free(cpBuf);
    if (cpBuf2)
        free(cpBuf2);
    if (cpOut)
        free(cpOut);

    myexit(EXRC);
    return EXRC; /* make -Wall happy ;-) */
}

/*EOF*/
