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
**  stand-alone CGI/1.0 compliant program or as a integrated API module
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
**  eperl.h -- ePerl library header file
*/
#ifndef EPERL_H
#define EERPL_H 1


/* only for back-ward compatibility with older versions of ePerl */
/* WARNING: these make problems when '>' occur in the Perl block!! */
#if 0
#define BEGIN_DELIMITER "<?" 
#define   END_DELIMITER ">"  
#endif

/* standard delimiters (RSE 1) */
#if 1
#define BEGIN_DELIMITER "<?"
#define   END_DELIMITER "!>"
#endif

/* alternative delimiters (RSE 2) */
#if 0
#define BEGIN_DELIMITER "<!"
#define   END_DELIMITER "!>"
#endif

/* new delimiters (RSE 3) */
#if 0
#define BEGIN_DELIMITER "<:"
#define   END_DELIMITER ":>"
#endif

/* alternative delimiters (JavaScript style) */
#if 0
#define BEGIN_DELIMITER "<script language='eperl'>"
#define   END_DELIMITER "</script>"
#endif

/* alternative delimiters (NeoScript style) */
#if 0
#define BEGIN_DELIMITER "<!--#eperl code='"
#define   END_DELIMITER "' -->"
#endif

#define EX__BASE        64      /* base value for error messages */
#define EX_USAGE        64      /* command line usage error */
#define EX_DATAERR      65      /* data format error */
#define EX_NOINPUT      66      /* cannot open input */
#define EX_NOUSER       67      /* addressee unknown */
#define EX_NOHOST       68      /* host name unknown */
#define EX_UNAVAILABLE  69      /* service unavailable */
#define EX_SOFTWARE     70      /* internal software error */
#define EX_OSERR        71      /* system error (e.g., can't fork) */
#define EX_OSFILE       72      /* critical OS file missing */
#define EX_CANTCREAT    73      /* can't create (user) output file */
#define EX_IOERR        74      /* input/output error */
#define EX_TEMPFAIL     75      /* temp failure; user is invited to retry */
#define EX_PROTOCOL     76      /* remote error in protocol */
#define EX_NOPERM       77      /* permission denied */
#define EX_CONFIG       78      /* configuration error */
#define EX__MAX         78      /* maximum listed value */

/* OK and FAIL exits should ALLWAYS exists */
#ifndef EX_OK
#define EX_OK   0
#endif
#ifndef EX_FAIL
#define EX_FAIL 1
#endif


/*
 *  CU() -- CleanUp Makro
 *  implemented in a safety way
 */
#define DECL_EXRC int rc
#define EXRC rc
#define ZERO 0
#define STMT(stuff) do { stuff } while (ZERO)
#define CU(returncode) STMT( rc = returncode; goto CUS; )
#define VCU STMT( goto CUS; )
#define RETURN_WVAL(val) return (val)
#define RETURN_EXRC return (rc)
#define RETURN_NORC return

/*
 *  Yeah, here it comes...all ASCII control codes.
 *  Defined as their correct acronyms.
 */
#define ASC_NUL '\x00'
#define ASC_SOH '\x01'
#define ASC_STX '\x02'
#define ASC_ETX '\x03'
#define ASC_EOT '\x04'
#define ASC_ENQ '\x05'
#define ASC_ACK '\x06'
#define ASC_BEL '\x07'
#define ASC_BS  '\x08'
#define ASC_HT  '\x09'
#define ASC_LF  '\x0a'
#define ASC_VT  '\x0b'
#define ASC_FF  '\x0c'
#define ASC_CR  '\x0d'
#define ASC_SO  '\x0e'
#define ASC_SI  '\x0f'
#define ASC_DLE '\x10'
#define ASC_DC1 '\x11'
#define ASC_DC2 '\x12'
#define ASC_DC3 '\x13'
#define ASC_DC4 '\x14'
#define ASC_NAK '\x15'
#define ASC_SYN '\x16'
#define ASC_ETB '\x17'
#define ASC_CAN '\x18'
#define ASC_EM  '\x19'
#define ASC_SUB '\x1a'
#define ASC_ESC '\x1b'
#define ASC_FS  '\x1c'
#define ASC_GS  '\x1d'
#define ASC_RS  '\x1e'
#define ASC_US  '\x1f'
#define ASC_SP  '\x20'
#define ASC_DEL '\x7f'
#define NUL ASC_NUL

#define ASC_QUOTE '\x22'
#define ASC_NL    ASC_LF
#define NL        ASC_NL

/*   define the boolean values in a general and
 *   portable way.
 */
#undef  TRUE
#define TRUE  (0 || !(0))
#undef  FALSE
#define FALSE (!(TRUE))
/* typedef enum { false = FALSE, true = TRUE } bool; */

/*   some other names for true and false */
#define YES   TRUE
#define NO    FALSE

#define GOOD  TRUE
#define WRONG FALSE

#undef  OK
#define OK    TRUE
#define BAD   FALSE

#define SOME  TRUE
#define NONE  FALSE

/* forward declarations */
void DoError(char *str, ...);
void DoErrorWithLogFile(char *scripturl, char *scriptfile, char *logfile, char *str, ...);
void escfwrite(char *cpBuf, int nBuf, int count, FILE *fp);
char *ePerl_ReadSourceFile(char *filename, char **buf, int *num);
char *ePerl_ReadErrorFile(char *filename, char *scriptfile, char *scripturl);
char *ePerl_Bristled2Perl(char *buffer);
int ePerl_HeadersExists(char *cpBuf);

#endif /* EPERL_H */
/*EOF*/
