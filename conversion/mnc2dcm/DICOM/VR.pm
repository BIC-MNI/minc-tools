# DICOM::VR.pm - Definition of DICOM VR's 
#
# Andrew Crabb (ahc@jhu.edu), May 2002.
# Andrew Janke (a.janke@gmail.com), March 2009
# $Id: VR.pm,v 1.2 2009/03/13 06:46:47 rotor Exp $

package DICOM::VR;

use strict;
use warnings;
use Pod::Usage;

require Exporter;

our @ISA = qw(Exporter);
our @EXPORT = qw(%VR);
our $VERSION = sprintf "%d.%03d", q$Revision: 1.2 $ =~ /: (\d+)\.(\d+)/;

# Value Representations (DICOM Standard PS 3.5 Sect 6.2)
# Code     2 character DICOM VR
# Name     Descriptive name
# MaxSize  Maximum number of bytes per element
#             0 for undefined length
# Fixed     1 is the VR has a fixed size
# Numeric   1 if the VR is numeric only
# ByteSwap  1 if the field needs swapping
# Pack      Perl pack() code to use
#
#  Code     Name           MaxSize Fixed Numeric ByteSwap Pack
our %VR = (
   'AE' => ['Application Entity',         16,  0,  0,  0, 'a' ],
   'AS' => ['Age String',                  4,  1,  0,  0, 'a4'],
   'AT' => ['Attribute Tag',               4,  1,  0,  1, 'v2'],
   'CS' => ['Code String',                16,  0,  0,  0, 'a' ],
   'DA' => ['Date',                        8,  1,  0,  0, 'a8'],
   'DS' => ['Decimal String',             16,  0,  1,  0, 'a' ],
   'DT' => ['Date Time',                  26,  0,  0,  0, 'a' ],
   'FD' => ['Floating Point Double',       8,  1,  1,  1, 'd' ],
   'FL' => ['Floating Point Single',       4,  1,  1,  1, 'f' ],
   'IS' => ['Integer String',             12,  0,  1,  0, 'a' ],
   'LO' => ['Long String',                64,  0,  0,  0, 'a' ],
   'LT' => ['Long Text',               10240,  0,  0,  0, 'a' ],
   'OB' => ['Other Byte String',           0,  0,  0,  0, 'C' ],
   'OF' => ['Other Float String',          0,  0,  0,  1, 'f' ],
   'OW' => ['Other Word String',           0,  0,  0,  1, 'v' ],
#  'OT' => ['UNKNOWN',                     0,  0,  0,  0, ''  ],
   'OX' => ['Binary Stream',               0,  0,  0,  1, ''  ],
   'PN' => ['Person Name',                64,  0,  0,  0, 'a' ],
   'SH' => ['Short String',               16,  0,  0,  0, 'a' ],
   'SL' => ['Signed Long',                 4,  1,  1,  0, 'i' ],
   'SQ' => ['Sequence of Items',           0,  0,  0,  0, ''  ],
   'SS' => ['Signed Short',                2,  1,  1,  1, 's' ],
   'ST' => ['Short Text',               1024,  0,  0,  0, 'a' ],
   'TM' => ['Time',                       16,  0,  0,  0, 'a' ],
   'UI' => ['Unique Identifier UID',      64,  0,  0,  0, 'a' ],
   'UL' => ['Unsigned Long',               4,  1,  1,  1, 'V' ],
   'UN' => ['Unknown',                     0,  0,  0,  0, 'c' ],
   'US' => ['Unsigned Short',              2,  1,  1,  1, 'S' ],
   'UT' => ['Unlimited Text',              0,  0,  0,  0, 'a' ],
   );

1;
 
__END__

=head1 NAME

DICOM::VR - Definition of DICOM Value Representations (VR)s

=head1 SYNOPSIS

   use DICOM::VR qw/%VR/;
   
   my $pack_code = $DICOM::VR::VR{$this->{'code'}}[5];
   my $numeric = $DICOM::VR::VR{$this->{'code'}}[4];
   my $max_len = $DICOM::VR::VR{$this->{'code'}}[1];

=head1 DESCRIPTION

This module is little more than a great big list of all the DICOM VRs 
(Value Representations) that might be used in a DICOM Image

=head1 SEE ALSO

DICOM DICOM::Fields DICOM::Element

=head1 AUTHOR

Andrew Crabb E<lt>ahc@jhu.eduE<gt>,
Jonathan Harlap E<lt>jharlap@bic.mni.mcgill.caE<gt>,
Andrew Janke E<lt>a.janke@gmail.comE<gt>

=head1 COPYRIGHT AND LICENSE

Copyright (C) 2002 by Andrew Crabb
Some parts are Copyright (C) 2003 by Jonathan Harlap
And some Copyright (C) 2009 by Andrew Janke

This library is free software; you can redistribute it and/or modify
it under the same terms as Perl itself, either Perl version 5.6.0 or,
at your option, any later version of Perl 5 you may have available.

=cut
