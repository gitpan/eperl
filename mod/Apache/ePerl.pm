##        ____           _ 
##    ___|  _ \ ___ _ __| |
##   / _ \ |_) / _ \ '__| |
##  |  __/  __/  __/ |  | |
##   \___|_|   \___|_|  |_|
## 
##  ePerl -- Embedded Perl 5 Language
##
##  ePerl interprets an ASCII file bristled with Perl 5 program statements
##  by evaluating the Perl 5 code while passing through the plain ASCII
##  data. It can operate both as a standard Unix filter for general file
##  generation tasks and as a powerful Webserver scripting language for
##  dynamic HTML page programming. 
##
##  ======================================================================
##
##  Copyright (c) 1996,1997 Ralf S. Engelschall, All rights reserved.
##
##  This program is free software; it may be redistributed and/or modified
##  only under the terms of either the Artistic License or the GNU General
##  Public License, which may be found in the ePerl source distribution.
##  Look at the files ARTISTIC and COPYING or run ``eperl -l'' to receive
##  a built-in copy of both license files.
##
##  This program is distributed in the hope that it will be useful, but
##  WITHOUT ANY WARRANTY; without even the implied warranty of
##  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See either the
##  Artistic License or the GNU General Public License for more details.
##
##  ======================================================================
##
##  ePerl.pm -- Apache/mod_perl handler for fast emulated ePerl facility
##

package Apache::ePerl;


#   requirements and runtime behaviour
require 5.00325;
use strict;
use vars qw($VERSION);
use vars qw($nDone $nOk $nFail $Cache $Config);

#   imports
use Carp;
use Apache;
use Apache::Debug;
use Apache::Constants;
use FileHandle;
use File::Basename qw(dirname);
use Parse::ePerl;

#   private version number
$VERSION = "2.2.4";

#   globals
$nDone  = 0;
$nOk    = 0;
$nFail  = 0;
$Cache  = {};

#   configuration
$Config = {
    'BeginDelimiter'  => '<?',
    'EndDelimiter'    => '!>',
    'CaseDelimiters'  => 0,
    'ConvertEntities' => 1
};

#
#   send HTML error page
#
sub send_errorpage {
    my ($r, $e) = @_;

    $r->content_type("text/html");
    $r->send_http_header;
    $r->print(
        "<html>\n" .
        "<head>\n" .
        "<title>Apache::ePerl ERROR</title>\n" .
        "</head>\n" .
        "<body>\n" .
        "<h1>Apache::ePerl ERROR</h1>\n" .
        "<pre>$e</pre>\n" .
        "</body>\n" .
        "</html>\n"
    );
    $r->log_reason("Apache::ePerl: $e", $r->filename);
}

#
#   the mod_perl handler
#
sub handler {
    my ($r) = @_;
    my ($filename, $data, $error, $fh);
    my (%env, $rc, $mtime, $header, $key, $value);

    $nDone++;
    
    #   import filename from Apache API
    $filename = $r->filename;

    #   check for invalid filename
    if (-d $filename) {
        $r->log_reason("Apache::ePerl: Attempt to invoke directory as ePerl script", $filename);
        return FORBIDDEN;
    }
    if (not (-f _ and -s _)) {
        $r->log_reason("Apache::ePerl: File not exists, not readable or empty", $filename);
        return NOT_FOUND;
    }

    #   check if we are allowed to use ePerl
    if (not ($r->allow_options & OPT_EXECCGI)) {
        $r->log_reason("Apache::ePerl: Option ExecCGI is off in this directory", $filename);
        return FORBIDDEN;
    }

    #   check cache for existing P-code
    $mtime = -M _;
    if (not ($Cache->{$filename} and
             $Cache->{$filename}->{CODE} and
             $Cache->{$filename}->{MTIME} == $mtime)) {
        #   read script
        local ($/) = undef;
        $fh = new FileHandle $filename;
        $data = <$fh>;
        $fh->close;

        #   run the preprocessor over the script
        if (not Parse::ePerl::Preprocess({
            Script => $data,
            Result => \$data,
        })) {
            &send_errorpage($r, "Error on preprocessing script");
            $nFail++;
            return OK;
        }

        #   translate the script from bristled 
        #   ePerl format to plain Perl format
        if (not Parse::ePerl::Translate({
            Script          => $data,
            BeginDelimiter  => $Config->{'BeginDelimiter'},
            EndDelimiter    => $Config->{'EndDelimiter'},
            CaseDelimiters  => $Config->{'CaseDelimiters'},
            ConvertEntities => $Config->{'ConvertEntities'},
            Result          => \$data,
        })) {
            &send_errorpage($r, "Error on translating script from bristled to plain format");
            $nFail++;
            return OK;
        }

        #   precompile the source into P-code
        $error = "";
        if (not Parse::ePerl::Precompile({
            Script => $data,
            Name   => $filename, 
            Cwd    => dirname($filename),
            Result => \$data,
            Error  => \$error,
        })) {
            &send_errorpage($r, "Error on precompiling script from plain format to P-code:\n$error");
            $nFail++;
            return OK;
        }

        #   set the new results
        $Cache->{$filename} = {};
        $Cache->{$filename}->{MTIME} = $mtime;
        $Cache->{$filename}->{CODE}  = $data;
    }
  
    $data = $Cache->{$filename}->{CODE};
    %env = $r->cgi_env;
    if (not Parse::ePerl::Evaluate({
        Script  => $data,
        Name    => $filename, 
        Cwd     => dirname($filename),
        ENV     => \%env,
        Result  => \$data,
        Error   => \$error,
    })) {
        &send_errorpage($r, "Error on evaluating script from P-code:\n$error");
        $nFail++;
        return OK;
    }

    #   generate headers
    if ($data =~ m|^([A-Za-z0-9-]+:\s.+?)\n\n(.+)$|s) {
        ($header, $data) = ($1, $2);

        $r->content_type("text/html");
        $r->cgi_header_out("Content-Length", length($data));
        
        while ($header =~ m|^([A-Za-z0-9-]+):\s+(.+?)\s*\n(.+)$|) {
            ($key, $value, $header) = ($1, $2, $3);
            $r->cgi_header_out($key, $value);
        }
    }
    else {
        $r->content_type("text/html");
        $r->cgi_header_out("Content-Length", length($data));
    }

    #   send resulting page
    $r->send_http_header;
    $r->print($data) if (not $r->header_only);

    #   statistic
    $nOk++;

    #   make Apache API happy ;_)
    return OK;
}


#
#   optional Apache::Status information
#
Apache::Status->menu_item(
    'ePerl' => "ePerl status",
    sub {
        my ($r,$q) = @_;
        my (@s);
        push(@s, "<b>Information about Apache::ePerl</b><br>");
        push(@s, "Interpreted documents: <b>$nDone</b> ($nOk ok, $nFail failed)<br>");
        push(@s, "Versions: Apache::ePerl <b>$VERSION</b>, Parse::ePerl <b>$Parse::ePerl::VERSION</b>");
        return \@s;
    }
) if Apache->module("Apache::Status");


1;
##EOF##
__END__

=head1 NAME

Apache::ePerl - mod_perl handler for fast emulated ePerl facility

=head1 SYNOPSIS

   #   Apache's httpd.conf
   #   mandatory:
   PerlModule Apache::ePerl
   <Files ~ "/root/of/webmaster/area/.+\.iphtml$">
       Options     +ExecCGI
       SetHandler  perl-script
       PerlHandler Apache::ePerl
   </Files>
   #   optional:
   <Perl>
   $Apache::ePerl::Config->{'BeginDelimiter'}  = '<?';
   $Apache::ePerl::Config->{'EndDelimiter'}    = '!>';
   $Apache::ePerl::Config->{'CaseDelimiters'}  = 0;
   $Apache::ePerl::Config->{'ConvertEntities'} = 1;
   </Perl>

=head1 DESCRIPTION

This packages provides a handler function for Apache/mod_perl which can be
used to emulate the stand-alone Server-Side-Scripting-Language I<ePerl> (see
eperl(3) for more details) in a very fast way. This is not a real 100%
replacement for F<nph-eperl> because of reduced functionality, principal
runtime restrictions and speedup decisions. For instance this variant does not
(and cannot) provide the SetUID feature of ePerl nor does it check for allowed
filename extensions (speedup), etc. Instead it uses further features like
object caching which ePerl does not use. 

But the accepted bristled source file format is exactly the same as with the
regular ePerl facility, because Apache::ePerl uses the Parse::ePerl package
which provides the original ePerl parser and translator. So, any valid ePerl
which works under F<nph-eperl> can also be used under Apache::ePerl.

The intent is to use this special variant of ePerl for scripts which are
directly under control of the webmaster. In this situation no real security
problems exists for him, because all risk is at his own hands. For the average
user you should B<not> use Apache::ePerl. Instead install the regular
stand-alone ePerl facility (F<nph-eperl>) for your users.

So, the advantage of Apache::ePerl against the regular F<nph-eperl> is better
performance and nothing else. Actually scripts executed under Apache::ePerl
are at least twice as fast as under F<nph-eperl>. The reason its not that
ePerl itself is faster. The reason is the runtime in-core environment of
Apache/mod_perl which does not use forking.

=head2 Installation and Configuration

First you have to install Apache::ePerl so that Apache/mod_perl can find it.
This is usually done via configuring the ePerl distribution via the same Perl
interpreter as was used when building Apache/mod_perl.

Second, you have to add the following config snippet to Apache's F<httpd.conf>
file:

   PerlModule Apache::ePerl
   <Files ~ "/root/of/webmaster/area/.+\.iphtml$">
       Options     +ExecCGI
       SetHandler  perl-script
       PerlHandler Apache::ePerl
   </Files>

Third, when you want to change the defaults of the ePerl parser, you also can
add something like this to the end of the snippet above.

   <Perl>
   $Apache::ePerl::Config->{'BeginDelimiter'}  = '<?';
   $Apache::ePerl::Config->{'EndDelimiter'}    = '!>';
   $Apache::ePerl::Config->{'CaseDelimiters'}  = 0;
   $Apache::ePerl::Config->{'ConvertEntities'} = 1;
   </Perl>

Fourth you can additionally enable the mod_perl runtime status for
Apache::ePerl by using the following snippet.

   <Location /perl-status>
   SetHandler perl-script
   PerlHandler Apache::Status
   </Location>

This enables the URL C</perl-status> in general and the URL
C</perl-status?ePerl> in special. Use it to see how much scripts where run and
which version of ePerl you are currently using.

=head1 AUTHOR

 Ralf S. Engelschall
 rse@engelschall.com
 www.engelschall.com

=head1 HISTORY

Apache::ePerl was first implemented by Mark Imbriaco E<lt>mark@itribe.netE<gt>
in December 1996 as a plain Perl module after he has seen the original ePerl
from Ralf S. Engelschall. It implemented the ePerl idea, but was not
compatible to the original ePerl. In May 1997 Hanno Mueller
E<lt>hmueller@kabel.deE<gt> has taken over the maintainance from Mark I. and
enhanced Apache::ePerl by adding caching for P-Code, adding the missing chdir
stuff, etc. 

Nearly at the same time Ralf S. Engelschall was unhappy of the old
Apache::ePerl from Mark I. and already started to write this version (the one
you are current reading its POD). He has rewritten the complete module from
scratch, but incorporated the P-Code caching idea and the Apache::Status usage
from Hanno M.'s version. The big difference between this one and Mark I.'s or
Hanno M.'s versions are that this version makes use of the new Parse::ePerl
module which itself incorporates the original ePerl parser.  So this version
is more compliant to the original ePerl facility.

=head1 SEE ALSO

Parse::ePerl(3)

Web-References:

  Perl:     perl(1),     http://www.perl.com/perl/
  ePerl:    eperl(1),    http://www.engelschall.com/sw/eperl/
  Apache:   httpd(7),    http://www.apache.org/
  mod_perl: mod_perl(1), http://perl.apache.org/

=cut

##EOF##
