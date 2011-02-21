#!/usr/bin/perl
use threads;
use threads::shared;

# This program calculates the Hessian matrix
# We use it to get the errors for the paramters

# We take the best-fit parameters in on the command line
# We vary each of them by +/- h (which is currently fixed), and get a chi-squared
# We use this to construct each Hij

# The fixed values of hi and hj
my $numparameters :shared;

$numparameters = $#ARGV-1;
$lambda = $ARGV[$#ARGV];
# Get the parameter values
foreach $argnum (0 .. $#ARGV) {
	chomp $ARGV[$argnum];
	$parameter[$argnum] = $ARGV[$argnum];

	# Put in the h value for distance, and the scale factor
	if($argnum == 0) {
		$hi[$argnum] = 0.1*$lambda;
		$hj[$argnum] = 0.1*$lambda;
	} elsif($argnum == 1) {
		$hi[$argnum] = 0.01*$lambda;
		$hj[$argnum] = 0.01*$lambda;
	} elsif($argnum == 2) {
		$hi[$argnum] = 0.1*$lambda;
		$hj[$argnum] = 0.1*$lambda;
	} elsif($argnum == 3) {
		$hi[$argnum] = 0.1*$lambda;
		$hj[$argnum] = 0.1*$lambda;
	}	
}

# Finding H1
for($i=0;$i<=$numparameters;$i++) {
	for($j=0;$j<=$i;$j++) {
		$parameter[$i] = $parameter[$i] + $hi[$i];
		$parameter[$j] = $parameter[$j] + $hj[$j];

		$cmd1 = "./func.pl ";
	
		# Build the command
		for($k=0;$k<=3;$k++) {
			$cmd1 = $cmd1 . $parameter[$k] . " ";
		}
		$cmd1 = $cmd1 . "$i+$j.1";

		# Execute the commands
		$val1 = `$cmd1`;

		chomp $val1;

		$H1[$i][$j] = $val1;

		# Restore the parameters to their old values
		$parameter[$i] = $parameter[$i] - $hi[$i];
		$parameter[$j] = $parameter[$j] - $hj[$j];
	}
}

# Finding H2
for($i=0;$i<=$numparameters;$i++) {
	for($j=0;$j<=$i;$j++) {
		$parameter[$i] = $parameter[$i] + $hi[$i];
		$parameter[$j] = $parameter[$j] - $hj[$j];

		$cmd1 = "./func.pl ";
	
		# Build the command
		for($k=0;$k<=3;$k++) {
			$cmd1 = $cmd1 . $parameter[$k] . " ";
		}
		$cmd1 = $cmd1 . "$i+$j.1";

		# Execute the commands
		$val1 = `$cmd1`;

		chomp $val1;

		$H2[$i][$j] = $val1;

		# Restore the parameters to their old values
		$parameter[$i] = $parameter[$i] - $hi[$i];
		$parameter[$j] = $parameter[$j] + $hj[$j];
	}
}

# Finding H3
for($i=0;$i<=$numparameters;$i++) {
	for($j=0;$j<=$i;$j++) {
		$parameter[$i] = $parameter[$i] - $hi[$i];
		$parameter[$j] = $parameter[$j] + $hj[$j];

		$cmd1 = "./func.pl ";
	
		# Build the command
		for($k=0;$k<=3;$k++) {
			$cmd1 = $cmd1 . $parameter[$k] . " ";
		}
		$cmd1 = $cmd1 . "$i+$j.1";

		# Execute the commands
		$val1 = `$cmd1`;

		chomp $val1;

		$H3[$i][$j] = $val1; 

		# Restore the parameters to their old values
		$parameter[$i] = $parameter[$i] + $hi[$i];
		$parameter[$j] = $parameter[$j] - $hj[$j];
	}
}

# Finding H4
for($i=0;$i<=$numparameters;$i++) {
	for($j=0;$j<=$i;$j++) {
		$parameter[$i] = $parameter[$i] - $hi[$i];
		$parameter[$j] = $parameter[$j] - $hj[$j];

		$cmd1 = "./func.pl ";
	
		# Build the command
		for($k=0;$k<=3;$k++) {
			$cmd1 = $cmd1 . $parameter[$k] . " ";
		}
		$cmd1 = $cmd1 . "$i+$j.1";

		# Execute the commands
		$val1 = `$cmd1`;

		chomp $val1;

		$H4[$i][$j] = $val1;

		# Restore the parameters to their old values
		$parameter[$i] = $parameter[$i] + $hi[$i];
		$parameter[$j] = $parameter[$j] + $hj[$j];
	}
}


# Construct the final Hessian matrix

for($i=0;$i<=$numparameters;$i++) {
	for($j=0;$j<=$i;$j++) {
		$H[$i][$j] = ($H1[$i][$j] - $H2[$i][$j] - $H3[$i][$j] + $H4[$i][$j]) / (4*$hi[$i]*$hj[$j]);
		$H[$j][$i] = $H[$i][$j];
	}
}

# Use the symmetry to get the rest of the elements
#for($i=0;$i<=$numparameters;$i++) {
#	for($j=$i;$j<=$numparameters;$j++) {
#		$H[$j][$i] = $H[$i][$j];
#	}
#}

# Now, we have to invert this matrix
# We get this from Numerical Recipies p. 28
# This has been tested on its own, see invmat.pl

$N = $numparameters;
$NP = $numparameters + 1;

# Print it out
print "BEFORE INVERTING\n\n";

for($i=0;$i<=$N;$i++) {
	for($j=0;$j<=$N;$j++) {
		print $H[$i][$j] . "	";
	}
	print "\n";
}

for($j=0;$j<=$N;$j++) {
	$ipiv[$j] = 0;
}

for($i=0;$i<=$N;$i++) {
	$big = 0;
	for($j=0;$j<=$N;$j++) {
		if($ipiv[$j] != 1) {
		for($k=0;$k<=$N;$k++) {
			if($ipiv[$k] == 0) { 
				if(abs($H[$j][$k]) >= $big) {
					$big = abs($H[$j][$k]);
					$irow = $j;
					$icol = $k;
				}
			} elsif($ipiv[$k] > 1) {
				print "Singular Hessian Matrix #1\n";
				exit(0);
			}
		}
		}
	}

	$ipiv[$icol] = $ipiv[$icol] + 1;

	if($irow != $icol) {
		for($l=0;$l<=$N;$l++) {
			$dum = $H[$irow][$l];
			$H[$irow][$l]=$H[$icol][$l];
			$H[$icol][$l] = $dum;
		}
	}

	$indxr[$i] = $irow;
	$indxc[$i] = $icol;

	if($H[$icol][$icol] == 0) {
		print "Singular Hessian Matrix #2\n";
		exit(0);
	}

	$pivinv = 1.0/$H[$icol][$icol];
	$H[$icol][$icol] = 1.0;

	for($l=0;$l<=$N;$l++) {
		$H[$icol][$l] = $H[$icol][$l]*$pivinv;
	}
	
	for($ll=0;$ll<=$N;$ll++) {
		if($ll != $icol) {
			$dum = $H[$ll][$icol];
			$H[$ll][$icol] = 0;
			for($l=0;$l<=$N;$l++) {
				$H[$ll][$l] = $H[$ll][$l] - $H[$icol][$l]*$dum;
			}
		}
	}
}

for($l=$N;$l>=0;$l--) {
	if($indxr[$l] != $indxc[$l]) {
		for($k=0;$k<=$N;$k++) {
			$dum = $H[$k][$indxr[$l]];
			$H[$k][$indxr[$l]] = $H[$k][$indxc[$l]];
			$H[$k][$indxc[$l]] = $dum;
		}
	}
}

print "AFTER INVERTING\n\n";

for($i=0;$i<=$N;$i++) {
	for($j=0;$j<=$N;$j++) {
		print $H[$i][$j] . "	";
	}
	print "\n";
}

print "\n";

print "ERRORS\n\n";

for($i=0;$i<=$N;$i++) {
	print "Parameter $i: " . sqrt(2*$H[$i][$i]) . "\n";
}
