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
**  eperl_perl5.c -- ePerl Perl5 related stuff
*/

#include "eperl_global.h"
#include "eperl_proto.h"

#include <EXTERN.h>
#include <perl.h>                 

#ifdef HAVE_PERL_DYNALOADER

extern void boot_DynaLoader _((CV* cv));

/*
 *  the Perl XS init function for dynamic library loading
 */
void ePerl_xs_init(void)
{
   /* dXSUB_SYS; */
   char *file = __FILE__;

   /* dummy = 0; */ /* make gcc -Wall happy ;-) */
   newXS("DynaLoader::boot_DynaLoader", boot_DynaLoader, file);
}
#endif

/*
 *  sets a Perl scalar variable
 */
void ePerl_SetScalar(char *pname, char *vname, char *vvalue)
{
    char ca[1024];

    sprintf(ca, "%s::%s", pname, vname);
    sv_setpv(perl_get_sv(ca, TRUE), vvalue);
    return;
}

/*
 *  sets a Perl scalar variable
 */
void ePerl_ForceUnbufferedStdout(void)
{
    perl_eval_sv(newSVpv("$| = 1;",0), G_DISCARD);
    return;
}

/*EOF*/
