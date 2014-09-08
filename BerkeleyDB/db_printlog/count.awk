# $Id: count.awk,v 1.1.1.1 2000/08/12 08:29:01 silkworm Exp $
#
# Print out the number of log records for transactions that we
# encountered.

/^\[/{
	if ($5 != 0)
		print $5
}
