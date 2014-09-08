switch -regexp $attname {
    "\[Bb\]\[Aa\]\[Ss\]\[Ii\]\[Cc\]\[Ss\]\[Ee\]\[Aa\]\[Rr\]\[Cc\]\[Hh\]" { puts " "}
    "\[Ss\]\[Ee\]\[Ll\]\[Ee\]\[Cc\]\[Tt\]" { puts " "}
    "\[Ff\]\[Rr\]\[Oo\]\[Mm\]" { puts " "}
    "\[Ss\]\[Cc\]\[Oo\]\[Pp\]\[Ee\]" { puts " "}
    "\[Ww\]\[Hh\]\[Ee\]\[Rr\]\[Ee\]" { puts " "}
    "\[Aa\]\[Nn\]\[Dd\]" { puts " "}
    "\[Oo\]\[Rr\]" { puts " "}
    "\[Nn\]\[Oo\]\[Tt\]" { puts " "}
    "\[Ll\]\[Tt\]" { puts " "}
    "\[Ll\]\[Tt\]\[Ee\]" { puts " "}
    "\[Gg\]\[Tt\]" { puts " "}
    "\[Gg\]\[Tt\]\[Ee\]" { puts " "}
    "\[Ee\]\[Qq\]" { puts " "}
    "\[Ll\]\[Ii\]\[Tt\]\[Ee\]\[Rr\]\[Aa\]\[Ll\]" { puts " "}
    "\[Ii\]\[Ss\]\[Dd\]\[Ee\]\[Ff\]\[Ii\]\[Nn\]\[Ee\]\[Dd\]" { puts " "}
    "\[Ll\]\[Ii\]\[Kk\]\[Ee\]" { puts " "}
    "\[Cc\]\[Oo\]\[Nn\]\[Tt\]\[Aa\]\[Ii\]\[Nn\]\[Ss\]" { puts " "}
    "\[Oo\]\[Rr\]\[Dd\]\[Ee\]\[Rr\]\[Bb\]y" { puts " "}
    "\[Oo\]\[Rr\]\[Dd\]\[Ee\]\[Rr\]" { puts " "}
    "\[Aa\]\[Ss\]\[Cc\]\[Ee\]\[Nn\]\[Dd\]\[Ii\]\[Nn\]\[Gg\]" { puts " "}
    "\[Dd\]\[Ee\]\[Ss\]\[Cc\]\[Ee\]\[Nn\]\[Dd\]\[Ii\]\[Nn\]\[Gg\]" { puts " "}
    "\[Ll\]\[Ii\]\[Mm\]\[Ii\]\[Tt\]" { puts " "}
    "\[Nn\]\[Rr\]\[Ee\]\[Ss\]\[Uu\]\[Ll\]\[Tt\]\[Ss\]" { puts " "}
default { puts "unmatch item is $tagname"}

}
