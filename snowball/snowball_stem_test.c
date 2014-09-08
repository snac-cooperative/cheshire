#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include "libstemmer.h"


int main(int argc,char **argv) 
{

  int i = 0;
  int langcode = 0;
  struct SN_env *env = NULL;
  char *resultword = NULL;
  char *filebuffer = NULL;
  char *inword = NULL;
  FILE *infile = NULL;
  char *next_tok;
  char *keywordletters = " \t\n\r\a\b\v\f`~!@#$%^&*()_=|\\{[]};:'\",.<>?/";
  struct stat filestatus;
  int statresult;



  if (argc < 2 || (langcode = atoi(argv[1])) < 0 || langcode > 13) {
    printf("Usage: %s stemmernumber word_to_stem <filename>\n\
                   where stemmernumber is one of:\n\
         1: Porter Stemmer\n\
         2: English (porter2) Stemmer\n\
         3: French Stemmer\n\
         4: German Stemmer\n\
         5: Dutch Stemmer\n\
         6: Spanish Stemmer\n\
         7: Italian Stemmer\n\
         8: Swedish Stemmer\n\
         9: Portuguese Stemmer\n\
         10: Russian (UTF-8) Stemmer\n\
         11: Danish  Stemmer\n\
         12: Norwegian Stemmer\n\
         13: Russian (KOI-8) Stemmer\n\n\
         If filename is given the file is opened and all words extracted.\n"
	   , argv[0]);
    exit(-1);
  }

  printf("The language code is %d\n", langcode);


  switch (langcode) {
  case 1:
    printf("Using Porter Stemmer\n");
      break;
  case 2:
    printf("Using English (porter2) Stemmer\n");
    break;
  case 3:
    printf("Using French Stemmer\n");
    break;
  case 4:
    printf("Using German Stemmer\n");
  case 5:
    printf("Using Dutch Stemmer\n");
    break;
  case 6:
    printf("Using Spanish Stemmer\n");
    break;
  case 7:
    printf("Using Italian Stemmer\n");
    break;
  case 8:
    printf("Using Swedish Stemmer\n");
    break;
  case 9:
    printf("Using Portuguese Stemmer\n");
    break;
  case 10:
    printf("Using Russian (UTF-8) Stemmer\n");
    break;
  case 11:
    printf("Using Danish  Stemmer\n");
    break;
  case 12:
    printf("Using Norwegian Stemmer\n");
    break;
  case 13:
    printf("Using Russian (KOI-8) Stemmer\n");
    break;
      
  }

  if (argc == 4) {
    /* process from a file... */

    if (stat(argv[3], &filestatus) != 0) {
      printf("stat for input file %s failed\n", argv[3]);
      exit(1);
    }
    
    if ((infile = fopen(argv[3], "r")) == NULL) {
      printf("Unable to open file: %s\n", argv[3]);
      exit(2);
    }
    
    /* How big is it?? */
    filebuffer = malloc(filestatus.st_size + 1);
    fread(filebuffer, 1, filestatus.st_size, infile);
    fclose(infile);
  } 
  else {
    filebuffer = strdup(argv[2]);
  }

  

  inword = strtok_r (filebuffer, keywordletters, &next_tok);

  while (inword != NULL) {

    resultword = snowball_stem(inword, langcode);

    printf("The input word is   : '%s'\n", inword);
    printf("The stemmed word is : '%s'\n", resultword);

    inword = strtok_r (NULL, keywordletters, &next_tok);    

  }

  if (filebuffer != NULL) {
    free(filebuffer);
  }

  exit(0);
  
}

