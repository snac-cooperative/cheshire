#include <stdio.h>
#include "wn.h"
#include "wnconsts.h"

main(argc, argv)
int argc;
char *argv[];
{
  char *newword;
  char *word, *morphstr();
  int pos, new_pos;

  if (argc != 3) {
    fprintf(stderr, "usage:  %s #pos word -- where #pos is 0-3", argv[0]); 
    exit (1);
  }  
  pos = atoi(argv[1]);
  word = argv[argc-1];

 
  wninit();

  morphinit();

  newword = morphstr(word,pos);

  if (newword == NULL) {
    if (new_pos = in_wn(word,pos)) newword = word;
  }

  printf ("morphed word is : %s  pos is %d\n",newword, new_pos);

    
}
