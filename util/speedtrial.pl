#!/usr/bin/env perl

=head1 NAME

speedtrial.pl - Compare speed of multiple Wren files

=head1 SYNOPSIS

  speedtrial.pl STEM

Runs all C<< <STEM>*.wren >> tests and reports the results.

=cut

use 5.006;
use strict;
use warnings;
use Benchmark qw(cmpthese timethese);
use Config;
use File::Spec;
use FindBin;
use Pod::Usage qw(pod2usage);

exit main(@ARGV);

### Tweak the benchmark to take child time into account #####################

# By default, Benchmark runs until _this process_ has taken 0.1 sec. of CPU
# time for each test.  Since the vast majority of the runtime is in the
# _child processes_ for these tests, the default benchmarks take a very
# long time to run!
#
# To fix that, wrap Benchmark::timeit() to count child-process time
# as part of this process.

BEGIN {
  my $orig_timeit = \&Benchmark::timeit;
  no warnings 'redefine';
  *Benchmark::timeit = sub {
    my $times = $orig_timeit->(@_);
    #my($r, $pu, $ps, $cu, $cs, $n) = @$times;

    $times->[1] += $times->[3]; # Move user time from children to parent
    $times->[3] = 0;
    $times->[2] += $times->[4]; # Move system time from children to parent
    $times->[4] = 0;

    return $times;
  }
}

### main ####################################################################

sub main {
  pod2usage(2) unless @_ == 1;
  my $stem = shift;
  my @tests = <$stem*.wren>;
  die "Couldn't find any tests for $stem" unless @tests;

  # Find wren_test
  my $wren_test = find_wren_test();

  print join "\n", "Running using $wren_test:", @tests, '';

  my $starttime = time();
  $| = 1;

  my $results = timethese(-10,  # -10 => at least 10 sec. CPU time per test
    +{
      map { (File::Spec->splitpath($_))[2] => "print '.'; die 'test failed' if system q($wren_test), q($_)" } @tests
    }
  );
  my $duration = time() - $starttime;

  print("Ran for $duration seconds using $wren_test\n");
  cmpthese($results);
  return 0;
} #main()

sub find_wren_test {
  my ($vol, $dirs, $file) = File::Spec->splitpath($FindBin::Bin);
  my @dirs = File::Spec->splitdir($dirs);
  pop @dirs;
  push @dirs, 'bin';
  $dirs = File::Spec->catdir(@dirs);
  my $path = File::Spec->catpath($vol, $dirs, "wren_test$Config{exe_ext}");
  unless(-f $path && -x $path) {
    die "Wren test program $path not found or not executable.\nPlease build Wren and try again.\n"
  }
  return $path;
} #find_wren_test()

### Docs ####################################################################

=head1 AUTHOR

Christopher White (C<< cxwembedded@gmail.com >>)

=head1 LICENSE

MIT License

Copyright (c) 2021 Christopher White

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

=cut
