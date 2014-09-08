# $Id: commit.awk,v 1.1.1.1 2000/08/12 08:29:01 silkworm Exp $
#
# Output tid of committed transactions.

/txn_regop/ {
	print $5
}
