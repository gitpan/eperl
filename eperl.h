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
#ifndef EPL_H
#define EPL_H 1


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

#define DECL_EXRC int rc
#define EXRC rc
#define ZERO 0
#define STMT(stuff) do { stuff } while (ZERO)
#define CU(returncode) STMT( rc = returncode; goto CUS; )
#define VCU STMT( goto CUS; )
#define RETURN_EXRC return (rc)
#define RETURN_NORC return

#ifndef EX_OK
#define EX_OK   0
#endif
#ifndef EX_FAIL
#define EX_FAIL 1
#endif

#endif
