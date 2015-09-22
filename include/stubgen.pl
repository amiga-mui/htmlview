#!/usr/bin/perl

########################################################################
#
# cvinclude
#
# Copyright 2002-2005 by Emmanuel Lesueur and Sigbjørn Skjæret
#
# $Id: stubgen.pl,v 1.1 2007/01/05 11:09:35 itix Exp $
#
# Include conversion and generation tool.
#
# Modified to generate 68k style varargs for Ambient
#

require 5.004;

use Getopt::Long;

########################################################################
#
# Output file types descriptions.
#
# For each output file type, there must be a variable declaration
# and a line in the array below. The meanings of the columns are:
#
# 1) name of the option to output this file format
# 2) boolean: does generating this output requires a fd file ?
# 3) boolean: does generating this output requires a clib file ?
# 4) a reference to the associated variable
# 5) a reference to the function to use to generate this output.
# 6) boolean: shall the output file be opened and set as stdout
#    before entering the generation function ?
#
# The variable will hold the filename given by the user as an
# argument the matching option.
#
# Example: if "--inlines=somefile" is given in the command line,
# the variable $inline will be set to "somefile", and the function
# gen_inlines will be called to generate this file. It will need
# both the fd and the clib files.
#

my $proto;
my $inlines;
my $vbccinlines;
my $asmstubs;
my $brelasmstubs;
my $gatestubs;
my $outclib;
my $gluelib;
my $brelgluelib;
my $biggluelib;
my $bigbrelgluelib;
my $breltrampolines;
my $sysvtagstubs;

$tmpdestdir = "/tmp/cvinc.$$";

@output_descr =
(
	[ "proto",		1, 0,	\$proto,		\&gen_proto,		1 ],
	[ "inlines",		1, 1,	\$inlines,		\&gen_inlines,		1 ],
	[ "vbccinlines",	1, 1,	\$vbccinlines,		\&gen_vbccinlines,	1 ],
	[ "asmstubs",		0, 1,	\$asmstubs,		\&gen_asmstubs,		1 ],
	[ "brelasmstubs",	1, 1,	\$brelasmstubs,		\&gen_brelasmstubs,	1 ],
	[ "gatestubs",		1, 1,	\$gatestubs,		\&gen_gatestubs,	1 ],
	[ "outclib",		0, 1,	\$outclib,		\&gen_clib,		1 ],
	[ "gluelib",		0, 1,	\$gluelib,		\&gen_gluelib,		0 ],
	[ "brelgluelib",	1, 1,	\$brelgluelib,		\&gen_brelgluelib,	0 ],
	[ "biggluelib",		1, 1,	\$biggluelib,		\&gen_biggluelib,	0 ],
	[ "bigbrelgluelib",	1, 1,	\$bigbrelgluelib,	\&gen_bigbrelgluelib,	0 ],
	[ "breltrampolines",	1, 1,	\$breltrampolines,	\&gen_breltrampolines,	1 ],
	[ "sysvtagstubs",	1, 1,	\$sysvtagstubs,		\&gen_sysvtagstubs,	1 ],
);

########################################################################
#
# Other options.
#

local $help;
local $fd;		# name of the fd file
local $clib;		# name of the clib file
local $libprefix;	# prefix to add to gate stubs
local $root;		# root name for the library
local $fddir;		# fd files directory
local $clibdir;		# clib directory
local $srcdir;		# source include directory
local $destdir;		# dest directory for generated includes
local $destlibdir;	# dest directory for generated libraries
local $destlib;		# generate a big library for all glues
local $aboxlib;		# generate libabox.a
local $breldestlibdir;	# dest directory for generated libraries
local $breldestlib;	# generate a big library for all glues
local $brelaboxlib;	# generate libabox.a
local $quiet;		# be silent
local $verbose;		# be verbose

@options =
(
	"h|help+"		=> \$help,
	"fd=s"			=> \$fd,
	"clib=s"		=> \$clib,
	"r|root=s"		=> \$root,
	"f|fddir=s"		=> \$fddir,
	"c|clibdir=s"		=> \$clibdir,
	"s|srcdir=s"		=> \$srcdir,
	"d|destdir=s"		=> \$destdir,
	"destlibdir=s"		=> \$destlibdir,
	"breldestlibdir=s"	=> \$breldestlibdir,
	"destlib=s"		=> \$destlib,
	"breldestlib=s"		=> \$breldestlib,
	"aboxlib=s"		=> \$aboxlib,
	"amigalib=s"		=> \$aboxlib,
	"brelaboxlib=s"		=> \$brelaboxlib,
	"brelamigalib=s"	=> \$brelaboxlib,
	"libprefix=s"		=> \$libprefix,
	"q|quiet+"		=> \$quiet,
	"v|verbose+"		=> \$verbose,
);

########################################################################
#
# Usage info
#

$usage = <<EOF;

Usage: cvinclude.pl [options]
Options:

Converting whole directories:

	--srcdir=...			input directory
	--fddir=...			input fd directory (if not srcdir/fd)
	--clibdir=...			input clib directory (if not srcdir/clib)
	--destdir=...			destination directory
	--destlibdir=...		destination directory for stubs libraries
	--breldestlibdir=...		destination directory for base relative stubs libraries
	--destlib=libfoo.a		library containing all glues
	--breldestlib=libfoo.a		library containing all baserelative glues
	--aboxlib=libabox.a		put system libraries glues in libabox.a
	--brelaboxlib=libabox.a		put base relative system libraries glues in libabox.a

Converting files:

	--fd=fd/foo_lib.fd		input FD file
	--clib=clib/foo_protos.h	input clib file
	--root=foo			root of the library name (usually
					not needed)

	--outclib=clib/foo_protos.h	convert the clib file
	--proto=proto/foo.h		generate a proto file
	--inlines=ppcinline/foo.h	generate an inlines file
	--vbccinlines=inline/foo_protos.h	generate a VBCC inlines file
	--asmstubs=foo.s		generate asm stub functions
	--brelasmstubs=foo.s		generate base relative asm stubs
	--gatestubs=foogates.c		generate gates
	--gluelib=libfoo.a		generate a link library with glues.
	--brelgluelib=libfoo.a		generate a link library with base relative glues.
	--breltrampolines=footrpln.s	generate trampolines setting the smalldata base
	--sysvtagstubs=tagstubs.s	generate stubs for sysv tag functions

	--libprefix=LIB_		prefix to add to gate functions or tag stubs

	--quiet				be silent
	--verbose			extra output
EOF

########################################################################
#
# Libraries for which glues belong to libabox.a
#

#%syslibs =
#(
#	"amigaguide"		=>	1,
#	"asl"			=>	1,
#	"battclock"		=>	1,
#	"battmem"		=>	1,
#	"bullet"		=>	1,
#	"cardres"		=>	1,
#	"cia"			=>	1,
#	"colorwheel"		=>	1,
#	"commodities"		=>	1,
#	"console"		=>	1,
##	"cybergraphics"		=>	1,
#	"datatypes"		=>	1,
#	"diskfont"		=>	1,
#	"disk"			=>	1,
#	"dos"			=>	1,
#	"dtclass"		=>	1,
#	"exec"			=>	1,
#	"expansion"		=>	1,
#	"gadtools"		=>	1,
#	"graphics"		=>	1,
#	"icon"			=>	1,
#	"iffparse"		=>	1,
#	"input"			=>	1,
#	"intuition"		=>	1,
#	"keymap"		=>	1,
#	"layers"		=>	1,
#	"locale"		=>	1,
#	"mathffp"		=>	1,
#	"mathieeedoubbas"	=>	1,
#	"mathieeedoubtrans"	=>	1,
#	"mathieeesingbas"	=>	1,
#	"mathieeesingtrans"	=>	1,
#	"mathtrans"		=>	1,
#	"misc"			=>	1,
#	"poseidon"		=>	1,
#	"potgo"			=>	1,
#	"ramdrive"		=>	1,
#	"rexxsyslib"		=>	1,
#	"timer"			=>	1,
#	"utility"		=>	1,
#	"wb"			=>	1,
#);

########################################################################
#
# Here starts the real work
#

# Add output options to the option array.

foreach $output (@output_descr)
{
	push @options, ("@{$output}[0]=s" => @{$output}[3]);
}

# Parse the options.

unless (GetOptions(@options) && !$help)
{
	print $usage;
	exit(0);
}

if (!mkdir("$tmpdestdir", 0700))
{
	print "Unable to create tmp dir\n";
	exit(1);
}

# Do some Amiga -> Unix paths conversion.

$fddir =~ s%^(.*):%/$1/%;
$clibdir =~ s%^(.*):%/$1/%;
$srcdir =~ s%^(.*):%/$1/%;
$destdir =~ s%^(.*):%/$1/%;
$destlibdir =~ s%^(.*):%/$1/%;
$destlib =~ s%^(.*):%/$1/%;
$aboxlib =~ s%^(.*):%/$1/%;
$breldestlibdir =~ s%^(.*):%/$1/%;
$breldestlib =~ s%^(.*):%/$1/%;
$brelaboxlib =~ s%^(.*):%/$1/%;

# Find the command to use as 'as'.
if (defined($ENV{"AS"}))
{
	$AS = $ENV{"AS"};
}
else
{
	$AS = "ppc-morphos-as";
}

# Find the command to use as 'ar'.
if (defined($ENV{"AR"}))
{
	$AR = $ENV{"AR"};
}
else
{
	$AR = "ppc-morphos-ar";
}

# If a srcdir argument was given, convert includes in it.
if (defined($srcdir))
{
	# If no clibdir argument was given, it defaults to srcdir/clib
	if (!defined($clibdir) && -e "$srcdir/clib")
	{
		$clibdir = "$srcdir/clib";
	}

	# If no fddir argument was given, it defaults to srcdir/fd
	if (!defined($fddir) && -e "$srcdir/fd")
	{
		$fddir = "$srcdir/fd";
	}

	# If we have a destdir, generate includes there.
	if ($destdir)
	{
		scandir($srcdir, $destdir);
	}
}

# If we have a clibdir and a fddir, generate proto/inlines/glues from them.
if (defined($clibdir) && defined($fddir))
{
	undef $asmstubs;
	undef $gatestubs;

	# We may have to create the ppcinline and proto directories.
	if (defined($destdir))
	{
		if (!-e "$destdir/ppcinline")
		{
			mkdir("$destdir/ppcinline", 0755);
		}
		if (!-e "$destdir/inline")
		{
			mkdir("$destdir/inline", 0755);
		}
		if (!-e "$destdir/proto")
		{
			mkdir("$destdir/proto", 0755);
		}
	}

	# If we want to create libbrelabox.a, make a temporary directory
	# for the objects.
	if (defined($brelaboxlib))
	{
		unlink($brelaboxlib);
		mkdir("$tmpdestdir/brelabox", 0700);
	}

	# If we want to create libabox.a, make a temporary directory
	# for the objects.
	if (defined($aboxlib))
	{
		unlink($aboxlib);
		mkdir("$tmpdestdir/libaboxtmp", 0700);
	}

	# If we want to create a big glue library, make a temporary directory
	# for the objects.
	if (defined($destlib))
	{
		# Remove anything that could remain from a previous interrupted
		# execution and create $tmpdir.
		unlink($destlib);
		mkdir("$tmpdestdir/cvincludetmp", 0700);
	}
	if (defined($breldestlib))
	{
		# Remove anything that could remain from a previous interrupted
		# execution and create $tmpdir.
		unlink($breldestlib);
		mkdir("$tmpdestdir/cvincludebreltmp", 0700);
	}

	# Get all the fd files.
	my @fdfiles = glob("$fddir/*_lib.fd");

	# Find the matching clib files and convert.
	foreach $fd (@fdfiles)
	{
		undef $gluelib;
		undef $brelgluelib;
		undef $biggluelib;
		undef $bigbrelgluelib;
		undef $proto;
		undef $inlines;
		undef $vbccinlines;

		$fd =~ /(\w+)_lib.fd/;
		$root = $1;
		$clib = "$clibdir/$1_protos.h";

		if (defined($destdir))
		{
			$vbccinlines = "$destdir/inline/${root}_protos.h";
			$inlines = "$destdir/ppcinline/$root.h";
			$proto = "$destdir/proto/$root.h";
		}
		if (defined($aboxlib))
		{
			$biggluelib = "$tmpdestdir/libaboxtmp";
		}
		else
		{
			if (defined($destlibdir))
			{
				$gluelib = "$destlibdir/lib$root.a";
			}
			if (defined($destlib))
			{
				$biggluelib = "$tmpdestdir/cvincludetmp";
			}
		}
		if (defined($brelaboxlib))
		{
			$bigbrelgluelib = "$tmpdestdir/brelabox";
		}
		else
		{
			if (defined($breldestlibdir))
			{
				$brelgluelib = "$breldestlibdir/lib${root}b32.a";
			}
			if (defined($breldestlib))
			{
				$bigbrelgluelib = "$tmpdestdir/cvincludebreltmp";
			}
		}

		fd2inline();
	}

	# Create libabox.a if needed
	if ($aboxlib)
	{
		my $cmd = "$AR rs $aboxlib ".join(" ", glob("$tmpdestdir/libaboxtmp/*.o"));
		if (!$quiet)
		{
			print "Executing \"$cmd\".\n";
		}
		system($cmd);

		# If that worked, delete $tmpdir.
		if ($? == 0)
		{
			unlink(glob("$tmpdestdir/libaboxtmp/*"));
			rmdir("$tmpdestdir/libaboxtmp");
		}
		else
		{
			print STDERR "Can't create library $aboxlib\n";
		}
	}

	# Create libaboxb32.a if needed
	if ($brelaboxlib)
	{
		my $cmd = "$AR rs $brelaboxlib ".join(" ", glob("$tmpdestdir/brelabox/*.o"));
		if (!$quiet)
		{
			print "Executing \"$cmd\".\n";
		}
		system($cmd);

		# If that worked, delete $tmpdir.
		if ($? == 0)
		{
			unlink(glob("$tmpdestdir/brelabox/*"));
			rmdir("$tmpdestdir/brelabox");
		}
		else
		{
			print STDERR "Can't create library $brelaboxlib\n";
		}
	}

	# Create a big baserel library if needed
	if ($breldestlib)
	{
		my $cmd = "$AR rs $breldestlib ".join(" ", glob("$tmpdestdir/cvincludebtreltmp/*.o"));
		if (!$quiet)
		{
			print "Executing \"$cmd\".\n";
		}
		system($cmd);

		# If that worked, delete $tmpdir.
		if ($? == 0)
		{
			unlink(glob("$tmpdestdir/cvincludebreltmp/*"));
			rmdir("$tmpdestdir/cvincludebreltmp");
		}
		else
		{
			print STDERR "Can't create library $breldestlib\n";
		}
	}

	# Create a big library if needed
	if ($destlib)
	{
		my $cmd = "$AR rs $destlib ".join(" ", glob("$tmpdestdir/cvincludetmp/*.o"));
		if (!$quiet)
		{
			print "Executing \"$cmd\".\n";
		}
		system($cmd);

		# If that worked, delete $tmpdir.
		if ($? == 0)
		{
			unlink(glob("$tmpdestdir/cvincludetmp/*"));
			rmdir("$tmpdestdir/cvincludetmp");
		}
		else
		{
			print STDERR "Can't create library $destlib\n";
		}
	}
}
else
{
	# Single file mode.
	if (defined($fd) && !defined($root))
	{
		$fd =~ /(\w+)_lib.fd/;
		$root = $1;
	}
	fd2inline();
}

rmdir("$tmpdestdir");

exit(0);

########################################################################
#
# Scan an include directory and convert .h files in it.
#

sub scandir
{
	my $dir = shift;
	my $dest = shift;
	my @entries;
	my $entry;

	# Create the dest directory if needed.
	if (!-e $dest)
	{
		mkdir($dest, 0755);
	}

	# Get the list of entries in the directory.
	if (opendir(DIR, $dir))
	{
		@entries = readdir(DIR);
		closedir(DIR);
	}

	foreach $entry (@entries)
	{
		if (!$quiet)
		{
			print "Handling $dir/$entry\n";
		}

		# Ignore . and .. directories to avoid loops.
		if ($entry eq "." || $entry eq "..")
		{
		}
		elsif (-d "$dir/$entry")
		{
			# Convert the clib directory.
			if ($entry eq "clib")
			{
				my @files;
				my $file;

				if (!-e "$dest/$entry")
				{
					mkdir("$dest/$entry", 0755);
				}

				if (opendir(DIR, "$dir/$entry"))
				{
					@files = readdir(DIR);
					closedir(DIR);
				}

				foreach $file (@files)
				{
					if ($file ne "CVS")
					{
						$clib = "$dir/$entry/$file";
						if (open(FILE, ">$dest/$entry/$file"))
						{
							my $oldstdout = select(FILE);
							gen_clib();
							select($oldstdout);
							close(FILE);
						}
					}
				}

			}
			# Copy the lvo directory as-is.
			elsif ($entry eq "lvo")
			{
				my @files;
				my $file;

				if (!-e "$dest/$entry")
				{
					mkdir("$dest/$entry", 0755);
				}

				if (opendir(DIR, "$dir/$entry"))
				{
					@files = readdir(DIR);
					closedir(DIR);
				}

				foreach $file (@files)
				{
					if ($file ne "CVS")
					{
						copy("$dir/$entry/$file", "$dest/$entry/$file");
					}
				}
			}
			# Ignore some directories, convert the others.
			elsif ($entry ne "inline" &&
			    $entry ne "ppcinline" && $entry ne "pragmas" &&
			    $entry ne "ppcpragmas" && $entry ne "ppcproto" &&
			    $entry ne "pragma" && $entry ne "proto" && $entry ne "CVS")
			{

				scandir("$dir/$entry", "$dest/$entry");
			}
		}
		elsif ($entry =~ /\.h$/i)
		{
			# Convert .h files.
			convert_include("$dir/$entry", "$dest/$entry");
		}
	}
}


########################################################################
#
# Copy a file
#

sub copy
{
	my $src = shift;
	my $dest = shift;

	if (open(FILE, "<$src"))
	{
		my $old = $/;
		undef $/;
		$src = <FILE>;
		close(FILE);

		if (open(FILE, ">$dest"))
		{
			print FILE $src;
			close(FILE);
		}

		$/ = $old;
	}
}


########################################################################
#
# Convert an include file for use with ppc
#

sub convert_include
{
	my $src = shift;
	my $dest = shift;

	if (open(FILE, "<$src"))
	{
		my $old = $/;
		undef $/;
		$src = <FILE>;
		close(FILE);

		if (open(FILE, ">$dest"))
		{
			# Remove C style comments
			$src =~ s%/\*.*?\*/%%gs;

			# Remove C++ style comments
			$src =~ s%//.*?\n%\n%g;

			# Add #pragma pack
			$src =~ s/#include(.*?)\n/#pragma pack()\n#include$1\n#pragma pack(2)\n/g;
			print FILE "#pragma pack(2)\n";
			print FILE $src;
			print FILE "\n#pragma pack()\n";

			close(FILE);
		}

		$/ = $old;
	}
}


########################################################################
#
# Convert an fd and a clib file to other formats.
#

sub fd2inline
{
	# If we have a fd file, parse it.

	if (defined($fd))
	{
		%fds = parse_fd($fd);
	}

	# If we have a clib file, parse it.

	if (defined($clib))
	{
		%protos = parse_clib($clib);
	}

	$basetype = "Library";

	$uroot = uc($root);

	foreach $output (@output_descr)
	{
		my $opt = @{$output}[0];
		local $filename;
		my $func = @{$output}[4];

		if (!defined(${@{$output}[3]}))
		{
			next;
		}

		$filename = ${@{$output}[3]};
		$filename =~ s%^(.*):%/$1/%;

		if (@{$output}[1] && !defined($fd))
		{
			print STDERR "The --$opt option requires a fd file.\n";
			next;
		}

		if (@{$output}[2] && !defined($clib))
		{
			print STDERR "The --$opt option requires a clib file.\n";
			next;
		}

		if (!@{$output}[5])
		{
			if (defined($filename))
			{
				&$func();
			}
		}
		elsif (defined($filename))
		{
			if (!$quiet)
			{
				print STDERR "Generating $filename\n";
			}

			if (open(FILE, ">$filename"))
			{
				my $oldstdout = select(FILE);

				&$func();

				select($oldstdout);
				close(FILE);
			}
			else
			{
				print STDERR "Can't create file \"$filename\".\n";
			}
		}
	}
}


########################################################################
#
# Parse a fd file.
#

sub parse_fd
{
	my $filename = shift;
	my %function = ();

	if (open(FILE, "<$filename"))
	{
		my $line = 1;
		my $bias = 0;
		my $public = 1;
		$base = "LibBase";
		$private_file = 0;

		while (<FILE>)
		{
			#if (/^(\w+)\((.*?)\)\((.*?)\)\s*$/)
			if (/^(\w+)\((.*?)\)\((.*?)\)\s*$/)
			{
				#if ($public)
				{
					my $name = $1;
					my $regstr = lc($3);

					$regstr =~ s/\s//g;
					$regstr =~ s/\//,/g;

					if ($verbose)
					{
						print STDERR "Parsing FD: $name at $bias\n";
					}

					$function{$name}=[$bias, $regstr, $public];

					#if ($regstr =~ /\bsysv\b/)
					#{
					#	$function{$name}=[$bias];
					#}
					#else
					#{
					#	my @regs = split(/[,\/]/, $regstr);
					#	$function{$name}=[$bias, \@regs];
					#}
				}

				$bias += 6;
			}
			elsif (/^##/)
			{
				if (/^##\s*bias\s+(\d+)/)
				{
					$bias = $1;
				}
				elsif (/^##\s*public/)
				{
					$public = 1;
				}
				elsif (/^##\s*private/)
				{
					$public = 0;
				}
				elsif (/^##\s*base\s+_(\w+)/)
				{
					$base = $1;
				}
				elsif (/^##\s*end/)
				{
					last;
				}
				else
				{
					print STDERR "Ignoring command line $line.\n";
				}
			}
			elsif (/^\*/ || /^\s*$/)
			{
				# comment or blank line, ignore.
				if (/PRIVATE FILE/)
				{
					$private_file = 1;
				}
			}
			else
			{
				print STDERR "Ignoring bad line $line.\n";
			}

			++$line;
		}

		close(FILE);
	}
	else
	{
		print STDERR "Can't open fd file \"$filename\".\n";
	}

	return %function;
}


########################################################################
#
# Parse a declaration.
#

sub parse_declaration
{
	my $str = shift;
	my $type = "";
	my $post = "";

	#print "parse_declaration($str)\n";

	# Remove leading spaces.
	$str =~ s/^\s+//;

	# Skip extern

	$str =~ s/^extern\s+//;

	# Skip cv-qualifiers.
	while ($str =~ /^(const|volatile)\b\s*(.*)$/i)
	{
		$type = "$type$1 ";
		$str = $2;
	}

	# If we have a struct, skip it.
	if ($str =~ /^(struct|union|class|enum)\s+(.*)$/)
	{
		$type = "$type$1 ";
		$str = $2;
	}

	# We must have some type identifier now.
	$str =~ /^(\w+)\s*(.*)$/;
	$type = "$type$1 ";
	$str = $2;

	# Handle basic types with several tokens ('unsigned long', ...)
	while ($str =~ /^(int|short|char|long)\s+(.*)$/)
	{
		$type = "$type$1 ";
		$str = $2;
	}

	# Parse the declarator.
	while (1)
	{
		#print "type [$type] str [$str] post [$post]\n";
		my $change;

		do
		{
			$change = 0;

			# Skip cv-qualifiers.
			while ($str =~ /^(const|volatile)\b\s*(.*)$/i)
			{
				$type = "$type$1 ";
				$str = $2;
				$change = 1;
			}

			# Skip '*' and '&'.
			while ($str =~ /^([\*&]+)\s*(.*)$/)
			{
				$type = $type.$1;
				$str = $2;
				$change = 1;
			}
		}
		while ($change);

		# If we don't have a parenthesis now, we have finished splitting
		# the declarator.
		if ($str !~ /^\(/)
		{
			last;
		}

		# Skip it.
		$str = substr($str, 1);
		$type = "$type(";

		# Change parenthesis into <>.
		$str =~ tr/()/<>/;

		# Repeatedly convert inner <> pairs into ().
		while ($str =~ s/<([^<>]*)>/\($1\)/g) {}

		# Now, only one '>' should be left: the one matching the
		# parenthesis we removed. Things on the right go in $post,
		# the rest must be parsed.
		$str =~ /(.*)>(.*)/;
		$str = $1;
		$post = ")$2$post";
	}
	#print "endloop: type [$type] str [$str] post [$post]\n";

	# If we stopped splitting at an alphanumeric char, it must be
	# the identifier declared. Otherwise, there is no identifier.
	if ($str =~ /^(\w+)\s*(.*)/)
	{
		$str = $1;
		$post = $2.$post;
	}
	else
	{
		$post = $str.$post;
		$str = undef;
	}

#	print "type=[$type]\n";
#	print "post=[$post]\n";
#	print "decl=[$str]\n";

	return ($type, $post, $str);
}


########################################################################
#
# Parse a clib file.
#

sub parse_clib
{
	my $filename = shift;
	my $source = "";
	my %function = ();
	my ($argstr, $i);

	# small helper function used below
	sub convert
	{
		my $str = shift;
		$str =~ tr/;/,/;
		return "($str)";
	}

	sub rem_block
	{
		my $str = shift;
		if ($str =~ s/^\s*{/ /)
		{
			while ($str =~ s/{([^{}]*)}/«$1»/g) {}
			$str =~ s/}/ /g;
			$str =~ tr/«»/{}/;
		}
		return $str;
	}

	my $old = $/;
	undef $/;

	if (open(FILE, "<$filename"))
	{
		$source = <FILE>;
		close(FILE);
	}
	else
	{
		print STDERR "Can't open clib file \"$filename\".\n";
	}

	# CRLF -> LF
	$source =~ s/\r\n/\n/g;
	$source =~ s/\r/\n/g;

	# Remove C style comments
	$source =~ s%/\*.*?\*/%%gs;

	# Remove C++ style comments
	$source =~ s%//.*?\n%\n%g;

	# Remove preprocessor directives
	$/ = '\n';
	while ($source =~ s%^\s*#.*?$%%gm) {}

	# Remove newlines
	$source =~ s%\n% %g;

	# Remove 'extern "C" {'
	while ($source =~ s%extern\s*"\w+"\s*(.*)%rem_block($1)%eg) {}

	@protos = split(/\s*;\s*/, $source);

	foreach (@protos)
	{
		my ($type, $post, $name) = parse_declaration($_);

		if (defined($name) && $post =~ /^\(/)
		{
			$post = substr($post, 1);
			$post =~ tr/()/<>/;
			while ($post =~ s/<([^<>]*)>/($1)/g) {}

			$post =~ /(.*)>(.*)/;
			$argstr = $1;
			$post = $2;

			# The parameters are separated by commas, but there
			# can be commas *not* separating arguments too, in case
			# an argument is a function pointer.
			#
			# To split the arguments, we do:
			#  1) replace '(' by '<', ) by '>', and ',' by ';'
			$argstr =~ tr/(),/<>;/;

			#  2) replace inner <...> by (...), converting ';' to ',' inside
			while ($argstr =~ s/<([^<>]*)>/convert($1)/eg) {}

			#  3) split at the remaining semi-colons.
			my @args;
			my $is_vararg = 0;
			if ($argstr !~ /^\s*void\s*$/i && $argstr !~ /^\s*$/)
			{
				@args = split(/;/, $argstr);

				for ($i = 0; $i < scalar(@args); ++$i)
				{
					if ($args[$i] =~ /\.\.\./)
					{
						$is_vararg = 1;
						$args[$i] = ["void*", ""];
					}
					else
					{
						my ($type1, $post1, $decl1) = parse_declaration($args[$i]);

						# Convert 'blah xx[]' to 'blah* xx'.

						while ($post1 =~ /(.*)\[[^\[\]]*\]\s*$/)
						{
							$post1 = $1;
							$type1 = $type1."*";
						}

						$args[$i] = [$type1, $post1];
					}
				}
			}
			else
			{
				@args = ();
			}

			$function{$name} = [$type, $post, \@args, $is_vararg];
		}
	}

	$/ = $old;

	return %function;
}

########################################################################
#
# Iterators, for use in output generation functions.
#

sub for_each_proto_with_fd
{
	my $func = shift;
	my $f;
	local $funcname;

	while (($funcname, $f) = each(%protos))
	{
		local $vfuncname = $funcname;
		my $ok = 0;

		#if (exists($fds{$vfuncname}))
		{
			$ok = 1;
		}

		if ($ok)
		{
			local $is_varargs_function = @{$f}[3];
			local ($bias, $regstr, $public) = @{$fds{$vfuncname}};
			local $is_ppc_function = 0;
			local @regs;
			local $ret_pretype = @{$f}[0];
			local $ret_posttype = @{$f}[1];
			local @args = @{@{$f}[2]};
			local $numargs = scalar(@args);
			local  $is_void_function = ($ret_pretype =~ /^\s*void\s*$/i &&
						length($ret_posttype) == 0);
			if ($regstr =~ /\bsysv\b/)
			{
				$is_ppc_function = 1;
			}
			else
			{
				@regs = split(/,/, $regstr);
			}

			&$func();
		}
	}
}

sub for_params
{
	my $first = shift;
	my $last = shift;
	my $func = shift;
	local $param_num;

	for ($param_num = $first; $param_num < $last; ++$param_num)
	{
		#local $arg_pretype = $args[$param_num][0];
		#local $arg_posttype = $args[$param_num][1];
		#local $register = @{$regs}[$param_num];
		local $register = $regs[$param_num];
		local $param_name = "__p$param_num";
		local $is_varargs_param = ($is_varargs_function && $param_num == $numargs - 1);
		local $is_varargs_next = ($is_varargs_function && $param_num == $numargs - 2);

		&$func();
	}
}

sub for_each_param
{
	my $func = shift;
	for_params(0, $numargs, $func);
}

sub for_param
{
	my $n = shift;
	my $func = shift;
	for_params($n, $n+1, $func);
}

########################################################################
#
# Check if a type is a floating point one, and if so, return its size.
#

sub is_float
{
	my $type = shift;

	if ($type =~ /^\s*(\w+)\s*$/)
	{
		$type = $1;
		if ($type =~ /float/i)
		{
			return 4;
		}
		elsif ($type =~ /double/i)
		{
			return 8;
		}
	}
	return 0;
}


########################################################################
########################################################################
###                                                                  ###
###                    Output generation functions                   ###
###                                                                  ###
########################################################################
########################################################################
########################################################################
#
# Generate MorphOS stubs, as an asm file.
#

sub gen_asmstubs
{
	local $breg = "0";
	local $ha = "ha";
	local $l = "l";

	for_each_proto_with_fd sub
	{
		gen_asmstub();
	}
}

sub gen_brelasmstubs
{
	local $breg = "13";
	local $ha = "drelha";
	local $l = "drell";

	for_each_proto_with_fd sub
	{
		gen_asmstub();
	}
}

sub gen_asmstub
{
	local $success = 1;

#-----------------------------------
	print <<EOF;

	.align	2
	.globl	$funcname
	.type	$funcname,\@function
$funcname:
EOF
#-----------------------------------

	#if ($is_varargs_function)
	{
		if ($numargs > 8)
		{
			print STDERR "Varargs functions with more than 8 non-varargs arguments not supported (yet?).\n";
			return 0;
		}
		my $num_varregs = 9 - $numargs;
		my $varregs_space = 4 * $num_varregs;
		my $framesize = ($varregs_space + 16 + 15) & ~15;
		my $vararg_reg = 2 + $numargs;
		my $varregs_offset = $framesize - $varregs_space + 8;
		my $vararg_reg_offset;

		for_param $numargs-1, sub
		{
			if ($register =~ /d(\d)/)
			{
				$vararg_reg_offset = $1;
			}
			elsif ($register =~ /a(\d)/)
			{
				$vararg_reg_offset = 8 + $1;
			}
		};
		$vararg_reg_offset *= 4;

#-----------------------------------
		print <<EOF;
	lwz	12,0(1)
	mflr	0
	stwu	1,-$framesize(1)
	stw	12,16(1)
	stw	0,20(1)
EOF
#-----------------------------------

		my $r = $param_num + 3;

		for_params 0, $numargs-1, sub
		{
			$r = $param_num + 4;
		};

		my $k;
		my $offset = $varregs_offset;
		for ($k = $vararg_reg; $k < 11; ++$k)
		{
			print "\tstw\t$k,$offset(1)\n";
			$offset += 4;
		}

		#$r = $r + 1;

#-----------------------------------
		print <<EOF;
	addi	$r,1,$varregs_offset
	bl $funcname\A
	lwz	0,20(1)
	lwz	11,16(1)
	mtlr	0
	stwu	11,$framesize(1)
	blr
EOF
#-----------------------------------

	}

	$success;
}


########################################################################
#
# Generate a MorphOS link library containing glues.
#

sub gen_gluelib
{
	local $breg = "0";
	local $ha = "ha";
	local $l = "l";
	build_gluelib();
}

sub gen_brelgluelib
{
	local $breg = "13";
	local $ha = "drelha";
	local $l = "drell";
	build_gluelib();
}

sub build_gluelib
{
	my $tmpdir = "$tmpdestdir/tmpgluelib";

	# Remove anything that could remain from a previous interrupted
	# execution and create $tmpdir.
	unlink($filename);
	if (-e $tmpdir)
	{
		unlink(glob("$tmpdir/*"));
	}
	else
	{
		mkdir($tmpdir, 0700);
	}

	# Create object files for the glues. We pipe the generated asm
	# through 'as'.
	for_each_proto_with_fd sub
	{
		if (!$quiet)
		{
			print "Creating $tmpdir/$funcname.o\n";
		}
		if (open(FILE, "| $AS -o $tmpdir/$funcname.o"))
		{
			my $oldfile = select(FILE);
			my $success;

			$success = gen_asmstub();

			select($oldfile);

			close(FILE);

			if (!$success)
			{
				unlink("$tmpdir/$funcname.o");
			}
		}
		else
		{
			print STDERR "Can't create \"$tmpdir/$funcname.o\"";
		}
	};

	# Create the link library from all the objects.
	my $cmd = "$AR rs $filename ".join(" ", glob("$tmpdir/*.o"));
	if (!$quiet)
	{
		print "Executing \"$cmd\".\n";
	}
	system($cmd);

	# If that worked, delete $tmpdir.
	if ($? == 0)
	{
		unlink(glob("$tmpdir/*"));
		rmdir($tmpdir);
	}
	else
	{
		print STDERR "Can't create library $filename\n";
	}
}
