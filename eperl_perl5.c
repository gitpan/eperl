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
**  eperl_perl5.c -- ePerl Perl5 related stuff
*/

#include "eperl_global.h"
#include "eperl_proto.h"

#include <EXTERN.h>
#include <perl.h>                 


#ifdef HAVE_PERL_DYNALOADER

extern void boot_DynaLoader _((CV* cv));

/*
**
**  the Perl XS init function for dynamic library loading
**
*/
void Perl5_XSInit(void)
{
   /* dXSUB_SYS; */
   char *file = __FILE__;

   /* dummy = 0; */ /* make gcc -Wall happy ;-) */
   newXS("DynaLoader::boot_DynaLoader", boot_DynaLoader, file);
}
#endif /* HAVE_PERL_DYNALOADER */

/*
**
**  Force Perl to use unbuffered I/O
**
*/
void Perl5_ForceUnbufferedStdout(void)
{
    IoFLAGS(GvIOp(defoutgv)) |= IOf_FLUSH; /* $|=1 */
    return;
}

/*
**
**  eval a Perl command
**
**  We provide an own copy of the new perl_eval_pv() function
**  which was first introduced in Perl 5.003_97e, so its
**  missing in older Perl versions, for instance plain old 5.003.
**  The above code was copied from perl5.004/perl.c
**
*/

SV* Perl5_eval_pv(char* p, I32 croak_on_error)
{
    dSP;
    SV* sv = newSVpv(p, 0);

    PUSHMARK(sp);
    perl_eval_sv(sv, G_SCALAR);
    SvREFCNT_dec(sv);
    SPAGAIN;
    sv = POPs;
    PUTBACK;
    if (croak_on_error && SvTRUE(GvSV(errgv)))
        croak(SvPVx(GvSV(errgv), na));
    return sv;
}

void Perl5_EvalCmd(char *cmd)
{
    (void)Perl5_eval_pv(cmd, FALSE);
    return;
}


/*
**
**  set a Perl environment variable
**
*/
char **Perl5_SetEnvVar(char **env, char *str) 
{
    char ca[1024];
    char *cp;

    strcpy(ca, str);
    cp = strchr(ca, '=');
    *cp++ = '\0';
    return mysetenv(env, ca, cp);
}

/*
**
**  sets a Perl scalar variable
**
*/
void Perl5_SetScalar(char *pname, char *vname, char *vvalue)
{
    char ca[1024];

    sprintf(ca, "%s::%s", pname, vname);
    sv_setpv(perl_get_sv(ca, TRUE), vvalue);
    return;
}

/*
**
**  remember a Perl scalar variable
**  and set it later
**
**  (this is needed because we have to
**   remember the scalars when parsing 
**   the command line, but actually setting
**   them can only be done later when the
**   Perl 5 interpreter is allocated !!)
**
*/

char *Perl5_RememberedScalars[1024] = { NULL };

void Perl5_RememberScalar(char *str) 
{
    int i;

    for (i = 0; Perl5_RememberedScalars[i] != NULL; i++)
        ;
    Perl5_RememberedScalars[i++] = strdup(str);
    Perl5_RememberedScalars[i++] = NULL;
    return;
}

void Perl5_SetRememberedScalars(void) 
{
    char ca[1024];
    char *cp;
    int i;

    for (i = 0; Perl5_RememberedScalars[i] != NULL; i++) {
        strcpy(ca, Perl5_RememberedScalars[i]);
        cp = strchr(ca, '=');
        *cp++ = '\0';
        Perl5_SetScalar("main", ca, cp);
    }
}

/*
**
**  remember a Perl INC entry
**  and set it later
**
*/

char *Perl5_RememberedINC[1024] = { NULL };

void Perl5_RememberINC(char *str) 
{
    int i;

    for (i = 0; Perl5_RememberedINC[i] != NULL; i++)
        ;
    Perl5_RememberedINC[i++] = strdup(str);
    Perl5_RememberedINC[i++] = NULL;
    return;
}

void Perl5_SetRememberedINC(void) 
{
    char ca[1024];
    int i;

    for (i = 0; Perl5_RememberedINC[i] != NULL; i++) {
        sprintf(ca, "push(@INC, '%s');", Perl5_RememberedINC[i]);
        Perl5_EvalCmd(ca);
    }
}


/*EOF*/
