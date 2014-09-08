
/* This does a conversion from UTF-8 to ISO8859-1, (NO case folding) */
#include <stdlib.h>
#include <string.h>
/* #include "header.h" not needed? */

/*proc utf8-iso8859-1 str {
 *    binary scan $str c* bytes
 *   set res ""
 *   foreach byte $bytes {
 *
 *       if {$byte<0} {
 *            set byte [expr $byte & 0xFF]
 *            if {$byte >=192} {
 *                set hi [expr ($byte&0x1F)<<6]
 *                continue
 *            } else {
 *                set byte [expr ($byte&0x3F)|$hi]
 *            }
 *        }
 *        lappend res $byte
 *        #puts "BYTE $byte"
 *    }
 *
 *    #puts "RES = $res"
 *    return [binary format c* $res]
 *}
 */


char *UTF8toISO8859(unsigned char *instring) {
  int i;
  int c;
  int ch;
  int hi;
  int j = 0;
  unsigned char *outstring;
  
  outstring = calloc(strlen(instring)+1, 1);
  
  /* UTF-8 is two bytes per character */
  
  for (c=0; c < strlen(instring); c++) {
    
    ch = (int)instring[c];
    
    if (ch >= 128) {
      if (ch >= 192) {
	hi = (ch & 0x1F)<<6;
	continue;
      } 
      else {
	ch = (ch & 0x3F)|hi;
      }
    }
    outstring[j++] = (unsigned char)ch;
  }
  
  return(outstring);
  
}

char *ISO8859toUTF8(unsigned char *instring) {
  int i;
  int c;
  int ch;
  int j = 0;
  unsigned char *outstring;

  outstring = calloc((strlen(instring)*2)+2, 1);

  for (c=0; c < strlen(instring); c++) {
    ch = (int)instring[c];

    if (ch <= 127) {
      outstring[j++] = (unsigned char)ch;
    } else {
      if (ch < 192) {
	outstring[j++] = (unsigned char)0xC2;
	outstring[j++] = (unsigned char)ch;
      }
      else {
	outstring[j++] = (unsigned char)0xC3;
	outstring[j++] = (unsigned char)ch-64;
      }
    }
  }
  return(outstring);
}

