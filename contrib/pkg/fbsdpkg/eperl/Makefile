# New ports collection makefile for:	eperl
# Version required:	2.2.0
# Date created:		Fri May  2 13:16:31 MET DST 1997
# Whom:			Ralf S. Engelschall <rse@engelschall.com>
#
# $Id: Makefile,v 1.6 1997/05/24 13:17:10 rse Exp $
#

DISTNAME=	eperl-2.2.0
CATEGORIES=	lang www perl5
MASTER_SITES=	http://www.engelschall.com/sw/eperl/distrib/ \
		ftp://ftp.engelschall.com/sw/

MAINTAINER=	rse@engelschall.com

# User-controlled variables list
#
# Server-Side Scripting Language (SSSL) support:
# INSTALL_ALSO_AS_SSSL - install ePerl also as SSSL for Apache
# ENABLE_UID_SWITCHING - allow ePerl to switch the UID/GID to the
#                        owner of the executed script when operating
#                        as a SSSL. Implies usage of SetUID bit!
#
INSTALL_ALSO_AS_SSSL=	yes
#ENABLE_UID_SWITCHING=	yes

BUILD_DEPENDS=	perl5.004:${PORTSDIR}/lang/perl5
.if defined(INSTALL_ALSO_AS_SSSL)
RUN_DEPENDS=	perl5.004:${PORTSDIR}/lang/perl5 \
		${PREFIX}/etc/apache/srm.conf:${PORTSDIR}/www/apache
.else
RUN_DEPENDS=	perl5.004:${PORTSDIR}/lang/perl5
.endif

GNU_CONFIGURE=	yes
CONFIGURE_ARGS+=--with-perl=${PREFIX}/bin/perl

do-install:
	${INSTALL_PROGRAM} ${WRKSRC}/eperl ${PREFIX}/bin/eperl
	${INSTALL_MAN} ${WRKSRC}/eperl.1 ${PREFIX}/man/man1/eperl.1
.if defined(INSTALL_ALSO_AS_SSSL)
	@if [ ! -d ${PREFIX}/www/cgi-bin ]; then \
		${MKDIR} -p ${PREFIX}/www/cgi-bin; \
	fi
.if defined(ENABLE_UID_SWITCHING)
	${INSTALL_PROGRAM} -o root -m u+s ${WRKSRC}/eperl \
		${PREFIX}/www/cgi-bin/nph-eperl
.else
	${INSTALL_PROGRAM} ${WRKSRC}/eperl ${PREFIX}/www/cgi-bin/nph-eperl
.endif
.endif
.if !defined(NOPORTDOCS)
	@if [ ! -d ${PREFIX}/share/examples/eperl ]; then \
		${MKDIR} -p ${PREFIX}/share/examples/eperl; \
	fi
	${INSTALL_DATA} ${WRKSRC}/eg/demo.cgipm.phtml \
		${PREFIX}/share/examples/eperl/demo.cgipm.phtml
	${INSTALL_DATA} ${WRKSRC}/eg/demo.env.phtml \
		${PREFIX}/share/examples/eperl/demo.env.phtml
	${INSTALL_DATA} ${WRKSRC}/eg/demo.errout.phtml \
		${PREFIX}/share/examples/eperl/demo.errout.phtml
	${INSTALL_DATA} ${WRKSRC}/eg/demo.errsyn.phtml \
		${PREFIX}/share/examples/eperl/demo.errsyn.phtml
	${INSTALL_DATA} ${WRKSRC}/eg/demo.func.phtml \
		${PREFIX}/share/examples/eperl/demo.func.phtml
	${INSTALL_DATA} ${WRKSRC}/eg/demo.html.phtml \
		${PREFIX}/share/examples/eperl/demo.html.phtml
	${INSTALL_DATA} ${WRKSRC}/eg/demo.image.phtml \
		${PREFIX}/share/examples/eperl/demo.image.phtml
	${INSTALL_DATA} ${WRKSRC}/eg/demo.lwp.phtml \
		${PREFIX}/share/examples/eperl/demo.lwp.phtml
	${INSTALL_DATA} ${WRKSRC}/eg/demo.net.phtml \
		${PREFIX}/share/examples/eperl/demo.net.phtml
	${INSTALL_DATA} ${WRKSRC}/eg/demo.pp.phtml \
		${PREFIX}/share/examples/eperl/demo.pp.phtml
	${INSTALL_DATA} ${WRKSRC}/eg/demo.table.phtml \
		${PREFIX}/share/examples/eperl/demo.table.phtml
.endif

.if defined(INSTALL_ALSO_AS_SSSL)
	@if [ ".`grep x-httpd-eperl ${PREFIX}/etc/apache/srm.conf`" = . ]; then \
		${ECHO} "AddType  application/x-httpd-eperl  .phtml .eperl .epl" \
			>>${PREFIX}/etc/apache/srm.conf; \
		${ECHO} "Action   application/x-httpd-eperl  /cgi-bin/nph-eperl" \
			>>${PREFIX}/etc/apache/srm.conf; \
		${ECHO} "Added knowledge about ePerl to Apache's srm.conf"; \
	else \
		${ECHO} "Apache srm.conf is already aware of ePerl"; \
	fi
.endif

.include <bsd.port.mk>
