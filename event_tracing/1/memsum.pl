#!/usr/bin/perl
#counting total memory usage each time a new process is spawned

open(TP, "<", "/sys/kernel/debug/tracing/trace_pipe") or die "can't open debugfs";
opendir(PROC, "/proc") or die "can't open /proc/";

while(<TP>){
	$_ =~ /(comm=)(\w*)(\s)(pid=)(\d*)/;
	
	$newuid, $newname = $2, $newpid = $5;
	open(my $newstat, "<", "/proc/$newpid/status");
	while(<$newstat>)
	{
		if($_ =~ /(Uid:)(\s*)(\d*)/)
		{
			$newuid = $3;
		}
	}

	if($newuid > 999 && $newuid < 3000)
	{
		printf  "pid: " . $newpid . " - " . $newname . " of uid: " . $newuid . "\n";
		$sum = 0;
		@procs = grep {/(\d+)/} readdir PROC;
		foreach my $proc (@procs)
		{
			open(my $stat, "<", "/proc/$proc/status");
			while(<$stat>)
			{
				if($_ =~ /(VmRSS:)(\s*)(\d*)/)
				{
					$sum = $sum + int($3);
				}
			}

		}
		printf "memory used:%d\n" , $sum;
		rewinddir PROC
	}

}
