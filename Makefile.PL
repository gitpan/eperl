##
##  faked Perl 5 MakeMaker Makefile
##  Copyright (c) 1997 Ralf S. Engelschall, All Rights Reserved. 
##

require 5.00325;
use Config; 

$perl = $Config{bin} . "/perl";
$args = join(' ', @ARGV);

unlink("Makefile");
unlink("Makefile.stand");

open(MK, ">Makefile");
print MK <<"EOT";

all:
	\@if [ ! -f Makefile.stand ]; then \\
	    cp Makefile Makefile.perl; \\
		echo "./configure"; \\
	    ./configure --with-perl=$perl; \\
	    mv Makefile Makefile.stand; \\
	    cp Makefile.perl Makefile; \\
		rm Makefile.perl; \\
	fi
	@\$(MAKE) -f Makefile.stand libeperl.a
	\@if [ ! -f mod/Makefile ]; then \\
	    cd mod && $perl Makefile.PL $args; \\
		sed -e '/^\trm -f */d' <Makefile >Makefile.n && mv Makefile.n Makefile; \\
	fi
	cd mod && \$(MAKE) \$(MFLAGS)

test: all
	cd mod && \$(MAKE) \$(MFLAGS) test

install: all
	\@if [ "x\$(UNINST)" = x1 ]; then \\
	    echo "cd mod && \$(MAKE) \$(MFLAGS) install UNINST=1"; \\
	    cd mod && \$(MAKE) \$(MFLAGS) install UNINST=1; \\
	else \\
	    echo "cd mod && \$(MAKE) \$(MFLAGS) install"; \\
	    cd mod && \$(MAKE) \$(MFLAGS) install; \\
	fi

clean:
	cd mod && rm -rf blib *.o *.c *.bs pm_to_blib
	\$(MAKE) -f Makefile.stand clean

distclean:
	cd mod && \$(MAKE) \$(MFLAGS) distclean
	\$(MAKE) -f Makefile.stand distclean
	rm -f Makefile.stand
	rm -f Makefile

EOT
close(MK);

##EOF##
