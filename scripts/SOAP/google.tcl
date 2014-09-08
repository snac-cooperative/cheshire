
package require SOAP
package require base64

SOAP::configure -transport http -proxy wwwcache.liv.ac.uk:5128

source "parseWSDL.tcl"

set key "/9DQzbb+EBJkRcHuZCrJYzcmKiapGZoX"
set start 1
set maxResults 10
set filter "false"
set restrict ""
set safeSearch "false"
set lr ""
set ie "latin1"
set oe "latin1"

array set results [list]
for {set x 0} {$x < $maxResults} {incr x} {
   array set "result$x" [list]
}

proc gsrch {query} {
   global key start maxResults filter restrict safeSearch lr ie oe results
   for {set x 0} {$x < $maxResults} {incr x} {
      global "result$x"
   }

   set res [doGoogleSearch $key $query $start $maxResults $filter $restrict $safeSearch $lr $ie $oe]
   array set results $res

   set resultlist $results(resultElements)
   for {set x 0} {$x < $maxResults} {incr x} {
      array set "result$x" [lindex $resultlist $x]
   }
   
}
   
proc gcache {url} {
   global key
   set res [doGetCachedPage $key $url]
   set html [base64::decode $res]
   return $html
}  

proc gspell {phrase} {
   global key
   set res [doSpellingSuggestion $key $phrase]
   return $res
}
