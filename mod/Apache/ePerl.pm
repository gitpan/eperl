##
##  ePerl.pm -- mod_perl handler for fast emulated ePerl facility
##  Copyright (c) 1997 Ralf S. Engelschall, All Rights Reserved. 
##

package Apache::ePerl;

#   requirements and runtime behaviour
require 5.00325;
use strict;
use Carp;
use vars qw($VERSION @ISA @EXPORT @EXPORT_OK $AUTOLOAD);
use vars qw($nDone $nOk $nFail $Cache);

#   private version number
$VERSION = "2.2.0";

#   globals
$nDone = 0;
$nOk   = 0;
$nFail = 0;
$Cache = {};

#   import of used modules
use Apache;
use Apache::Debug;
use Apache::Constants;
use FileHandle;
use File::Basename qw(dirname);
use Parse::ePerl;

sub send_errorpage {
    my ($r, $e) = @_;

    $r->content_type("text/html");
    $r->send_http_header;
    $r->print(
        "<html>\n" .
        "<head>\n" .
        "<title>\n" .
        "Apache::ePerl ERROR\n" .
        "</title>\n" .
        "</head>\n" .
        "<body>\n" .
        "<h1>\n" .
        "Apache::ePerl ERROR\n" .
        "</h1>\n" .
        "<pre>$e</pre>\n" .
        "</body>\n" .
        "</html>\n"
    );
    $r->log_reason("Apache::ePerl: $e", $r->filename);
}

#   the mod_perl handler
sub handler {
    my ($r) = @_;
    my ($filename, $data, $error, $fh);
    my (%env, $rc, $mtime);

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
            Script         => $data,
            BeginDelimiter => '<?',
            EndDelimiter   => '!>',
            Result         => \$data,
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

    #   send resulting page
    $r->content_type("text/html");
    $r->send_http_header;
    $r->print($data);

    #   statistic
    $nOk++;

    #   make Apache API happy ;_)
    return OK;
}

#   when Apache::Status is loaded, add
#   information about us
Apache::Status->menu_item(
    'ePerl' => "ePerl status",
    sub {
        my ($r,$q) = @_;
        my (@s);
        push(@s, "<b>Information about Apache::ePerl</b><br>");
        push(@s, "Interpreted documents: <b>$nDone</b> ($nOk ok, $nFail failed)<br>");
        push(@s, "Version: <b>$VERSION</b>");
        return \@s;
    }
) if Apache->module("Apache::Status");

1;
__END__

=head1 NAME

Apache::ePerl - mod_perl handler for fast emulated ePerl facility

=head1 SYNOPSIS

   #   httpd.conf
   PerlModule Apache::ePerl

   <Files ~ "/root/of/webmaster/area/.+\.iphtml$">
   Options     +ExecCGI
   SetHandler  perl-script
   PerlHandler Apache::ePerl::handler
   </Files>

=head1 DESCRIPTION

This packages provides a handler function for Apache/mod_perl which can be
used to emulate the stand-alone Server-Side-Scripting-Language ePerl (see
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

=head1 ADVANTAGE

The advantage of Apache::ePerl against the regular F<nph-eperl> is better
performance. Actually scripts executed under Apache::ePerl are at least twice
as fast as under F<nph-eperl>. The reason its not that ePerl itself is faster.
The reason are the runtime in-core environment of Apache+mod_perl which does
not use forking.

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

Parse::ePerl(3), eperl(1), http://www.engelschall.com/sw/eperl/.

=cut

##EOF##
