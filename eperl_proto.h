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
**  statements by expanding the Perl 5 code. It can operate both as a 
**  stand-alone CGI/1.1 compliant program or as a integrated API module
**  for the Apache webserver. The resulting data is pure HTML markup code.
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
**  eperl_proto.h -- ePerl prototypes
*/
#ifndef EPERL_PROTO_H
#define EPERL_PROTO_H 1

/*_BEGIN_PROTO_*/
#if __STDC__ || defined(__cplusplus)
#define P_(s) s
#else
#define P_(s) ()
#endif


/* eperl_main.c */
extern int mode;
extern char *begin_delimiter;
extern char *end_delimiter;
extern char *allowed_file_ext[];
extern char *allowed_caller_uid[];
extern void PrintError P_((int mode, char *scripturl, char *scriptfile, char *logfile, char *str, ...));
extern void give_version P_((void));
extern void give_version_extended P_((void));
extern void give_readme P_((void));
extern void give_license P_((void));
extern void give_img_logo P_((void));
extern void give_usage P_((char *name));
extern char **set_variable P_((char **env, char *str));
extern char *remembered_perl_scalars[100];
extern void remember_perl_scalar P_((char *str));
extern void set_perl_scalars P_((void));
extern void mysighandler P_((int rc));
extern void myinit P_((void));
extern void myexit P_((int rc));
extern int main P_((int argc, char **argv, char **env));

/* eperl_perl5.c */
extern void ePerl_xs_init P_((void));
extern void ePerl_SetScalar P_((char *pname, char *vname, char *vvalue));
extern void ePerl_ForceUnbufferedStdout P_((void));

/* eperl_parse.c */
extern void ePerl_SetError P_((char *str, ...));
extern char *ePerl_GetError P_((void));
extern char *ePerl_fprintf P_((char *cpOut, char *str, ...));
extern char *ePerl_fwrite P_((char *cpBuf, int nBuf, int cNum, char *cpOut));
extern char *ePerl_Efwrite P_((char *cpBuf, int nBuf, int cNum, char *cpOut));
extern char *ePerl_ReadSourceFile P_((char *filename, char **cpBufC, int *nBufC));
extern char *ePerl_ReadErrorFile P_((char *filename, char *scriptfile, char *scripturl));
extern char *ePerl_Bristled2Perl P_((char *cpBuf));
extern int ePerl_IsHeaderLine P_((char *cp1, char *cp2));
extern int ePerl_HeadersExists P_((char *cpBuf));
extern void ePerl_StripHTTPHeaders P_((char **cpBuf, int *nBuf));

/* eperl_sys.c */
extern char **mysetenv P_((char **env, char *var, char *str, ...));
extern void IO_redirect_stdin P_((FILE *fp));
extern void IO_redirect_stdout P_((FILE *fp));
extern void IO_redirect_stderr P_((FILE *fp));
extern int IO_is_stdin_redirected P_((void));
extern int IO_is_stdout_redirected P_((void));
extern int IO_is_stderr_redirected P_((void));
extern void IO_restore_stdin P_((void));
extern void IO_restore_stdout P_((void));
extern void IO_restore_stderr P_((void));
extern char *mytmpfile P_((char *id));
extern void remove_mytmpfiles P_((void));
extern char *strnchr P_((char *buf, char chr, int n));
extern char *strnstr P_((char *buf, char *str, int n));
extern char *strndup P_((char *buf, int n));
extern char *isotime P_((time_t *t));

/* eperl_http.c */
extern void PrintHTTPResponse P_((void));
extern char *WebTime P_((void));

/* eperl_getopt.c */
extern int opterr;
extern int optind;
extern int optopt;
extern int optbad;
extern int optchar;
extern int optneed;
extern int optmaybe;
extern int opterrfd;
extern char *optarg;
extern char *optstart;
extern int egetopt P_((int nargc, char **nargv, char *ostr));

/* eperl_debug.c */
extern int fDebug;
extern char *cpDebugFile;
extern void Debug P_((char *str, ...));

/* eperl_version.c */
extern char ePerl_Version[];
extern char ePerl_Hello[];
extern char ePerl_GNUVersion[];
extern char ePerl_WhatID[];
extern char ePerl_RCSIdentID[];
extern char ePerl_WebID[];

/* eperl_readme.c */
extern char *ePerl_README;

/* eperl_license.c */
extern char *ePerl_LICENSE;

/* eperl_logo.c */
extern int ePerl_LOGO_size;
extern char ePerl_LOGO_data[];

#undef P_
/*_END_PROTO_*/

#endif /* EPERL_PROTO_H */
/*EOF*/
