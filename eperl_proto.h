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
**  Artistic License or the GNU General Public License for more details.
**
**  ======================================================================
**
**  eperl_proto.h -- ePerl ANSI C prototypes
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
extern char *allowed_file_ext[];
extern char *allowed_caller_uid[];
extern void PrintError P_((int mode, char *scripturl, char *scriptfile, char *logfile, char *str, ...));
extern void give_version P_((void));
extern void give_version_extended P_((void));
extern void give_readme P_((void));
extern void give_license P_((void));
extern void give_img_logo P_((void));
extern void give_img_powered P_((void));
extern void give_usage P_((char *name));
extern void mysighandler P_((int rc));
extern void myinit P_((void));
extern void myexit P_((int rc));
extern int main P_((int argc, char **argv, char **env));

/* eperl_perl5.c */
extern void Perl5_XSInit P_((void));
extern void Perl5_ForceUnbufferedStdout P_((void));
extern void Perl5_EvalCmd P_((char *cmd));
extern char **Perl5_SetEnvVar P_((char **env, char *str));
extern void Perl5_SetScalar P_((char *pname, char *vname, char *vvalue));
extern char *Perl5_RememberedScalars[1024];
extern void Perl5_RememberScalar P_((char *str));
extern void Perl5_SetRememberedScalars P_((void));
extern char *Perl5_RememberedINC[1024];
extern void Perl5_RememberINC P_((char *str));
extern void Perl5_SetRememberedINC P_((void));

/* eperl_parse.c */
extern char *ePerl_begin_delimiter;
extern char *ePerl_end_delimiter;
extern int ePerl_case_sensitive_delimiters;
extern int ePerl_convert_entities;
extern int ePerl_line_continuation;
extern void ePerl_SetError P_((char *str, ...));
extern char *ePerl_GetError P_((void));
extern char *ePerl_fprintf P_((char *cpOut, char *str, ...));
extern char *ePerl_fwrite P_((char *cpBuf, int nBuf, int cNum, char *cpOut));
extern char *ePerl_Efwrite P_((char *cpBuf, int nBuf, int cNum, char *cpOut));
extern char *ePerl_Cfwrite P_((char *cpBuf, int nBuf, int cNum, char *cpOut));
extern char *strnchr P_((char *buf, char chr, int n));
extern char *strnstr P_((char *buf, char *str, int n));
extern char *strncasestr P_((char *buf, char *str, int n));
extern char *strndup P_((char *buf, int n));
extern char *ePerl_Bristled2Plain P_((char *cpBuf));

/* eperl_pp.c */
extern void ePerl_PP_SetError P_((char *str, ...));
extern char *ePerl_PP_GetError P_((void));
extern char *ePerl_PP_Process P_((char *cpInput, char **cppINC, int mode));
extern char *ePerl_PP P_((char *cpBuf, char **cppINC));

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
extern char *isotime P_((time_t *t));
extern char *ePerl_ReadSourceFile P_((char *filename, char **cpBufC, int *nBufC));
extern char *ePerl_ReadErrorFile P_((char *filename, char *scriptfile, char *scripturl));
extern char *filename P_((char *path));
extern char *dirname P_((char *path));
extern char *abspath P_((char *path));

/* eperl_http.c */
extern void HTTP_PrintResponseHeaders P_((void));
extern void HTTP_StripResponseHeaders P_((char **cpBuf, int *nBuf));
extern int HTTP_IsHeaderLine P_((char *cp1, char *cp2));
extern int HTTP_HeadersExists P_((char *cpBuf));
extern char *WebTime P_((void));
extern FILE *HTTP_openURLasFP P_((char *url));

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
extern char ePerl_PlainID[];

/* eperl_readme.c */
extern char *ePerl_README;

/* eperl_license.c */
extern char *ePerl_LICENSE;

/* eperl_logo.c */
extern int ePerl_LOGO_size;
extern char ePerl_LOGO_data[];

/* eperl_powered.c */
extern int ePerl_POWERED_size;
extern char ePerl_POWERED_data[];

#undef P_
/*_END_PROTO_*/

#endif /* EPERL_PROTO_H */
/*EOF*/
