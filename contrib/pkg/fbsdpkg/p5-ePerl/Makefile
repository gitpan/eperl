# New ports collection makefile for:	p5-ePerl
# Version required:			2.2.0
# Date created:				Tue Jul 15 12:11:11 MET DST 1997
# Whom:	      				Ralf S. Engelschall <rse@engelschall.com>
#
# $Id: Makefile,v 1.6 1997/05/24 13:17:10 rse Exp $
#

DISTNAME=	eperl-2.2.0
PKGNAME=	p5-ePerl-2.2.0
CATEGORIES=	lang www perl5
MASTER_SITES=	${MASTER_SITE_PERL_CPAN}
MASTER_SITE_SUBDIR=	Parse

MAINTAINER=	rse@engelschall.com

USE_PERL5=	YES

MAN3=		Parse::ePerl.3 Apache::ePerl.3
MANPREFIX=	${PREFIX}/lib/perl5

do-configure:
		@ cd ${WRKSRC}; ${SETENV} ${MAKE_ENV} ${PREFIX}/bin/perl Makefile.PL

.include <bsd.port.mk>
