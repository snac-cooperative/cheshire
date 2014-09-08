#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include "libstemmer.h"

struct sb_stemmer *porterstemmer = NULL;
struct sb_stemmer *danishstemmer = NULL;
struct sb_stemmer *dutchstemmer = NULL;
struct sb_stemmer *englishstemmer = NULL;
struct sb_stemmer *finnishstemmer = NULL;
struct sb_stemmer *frenchstemmer = NULL;
struct sb_stemmer *germanstemmer = NULL;
struct sb_stemmer *hungarianstemmer = NULL;
struct sb_stemmer *italianstemmer = NULL;
struct sb_stemmer *norwegianstemmer = NULL;
struct sb_stemmer *portuguesestemmer = NULL;
struct sb_stemmer *romanianstemmer = NULL;
struct sb_stemmer *russianstemmer = NULL;
struct sb_stemmer *spanishstemmer = NULL;
struct sb_stemmer *swedishstemmer = NULL;
struct sb_stemmer *turkishstemmer = NULL;

struct sb_stemmer *porterstemmerutf = NULL;
struct sb_stemmer *danishstemmerutf = NULL;
struct sb_stemmer *dutchstemmerutf = NULL;
struct sb_stemmer *englishstemmerutf = NULL;
struct sb_stemmer *finnishstemmerutf = NULL;
struct sb_stemmer *frenchstemmerutf = NULL;
struct sb_stemmer *germanstemmerutf = NULL;
struct sb_stemmer *hungarianstemmerutf = NULL;
struct sb_stemmer *italianstemmerutf = NULL;
struct sb_stemmer *norwegianstemmerutf = NULL;
struct sb_stemmer *portuguesestemmerutf = NULL;
struct sb_stemmer *romanianstemmerutf = NULL;
struct sb_stemmer *russianstemmerutf = NULL;
struct sb_stemmer *spanishstemmerutf = NULL;
struct sb_stemmer *swedishstemmerutf = NULL;
struct sb_stemmer *turkishstemmerutf = NULL;


char *snowball_stem(char *inword, int langcode) 
{
  struct sb_stemmer * stemmer;
  int i = 0;
  char *resultword;
  const sb_symbol *stem=NULL;

  i = strlen(inword);

  switch (langcode) {
  case 1:
    /* printf("Using Porter Stemmer\n"); */
    if (porterstemmer == NULL)
      porterstemmer = sb_stemmer_new("porter", "ISO_8859_1");
    stem = sb_stemmer_stem(porterstemmer,(sb_symbol *)inword, i);      
    break;
  case 2:
    /* printf("Using English (porter2) Stemmer\n"); */
    if (englishstemmer == NULL)
      englishstemmer = sb_stemmer_new("english", "ISO_8859_1");
    stem = sb_stemmer_stem(englishstemmer,(sb_symbol *)inword, i);
    break;
  case 3:
    /* printf("Using French Stemmer\n"); */

    if (frenchstemmer == NULL)
      frenchstemmer = sb_stemmer_new("french", "ISO_8859_1");
    stem = sb_stemmer_stem(frenchstemmer,(sb_symbol *)inword, i);
    break;
  case 4:
    /* printf("Using German Stemmer\n"); */
    if (germanstemmer == NULL)
      germanstemmer = sb_stemmer_new("german", "ISO_8859_1");
    stem = sb_stemmer_stem(germanstemmer,(sb_symbol *)inword, i);
    break;
  case 5:
    /* printf("Using Dutch Stemmer\n"); */
    if (dutchstemmer == NULL)
      dutchstemmer = sb_stemmer_new("dutch", "ISO_8859_1");
    stem = sb_stemmer_stem(dutchstemmer,(sb_symbol *)inword, i);
    break;
  case 6:
    /*printf("Using Spanish Stemmer\n"); */
    if (spanishstemmer == NULL)
      spanishstemmer = sb_stemmer_new("spanish", "ISO_8859_1");
    stem = sb_stemmer_stem(spanishstemmer,(sb_symbol *)inword, i);
    break;
  case 7:
    /* printf("Using Italian Stemmer\n"); */
    if (italianstemmer == NULL)
      italianstemmer = sb_stemmer_new("italian", "ISO_8859_1");
    stem = sb_stemmer_stem(italianstemmer,(sb_symbol *)inword, i);
    break;
  case 8:
    /* printf("Using Swedish Stemmer\n"); */
    if (swedishstemmer == NULL)
      swedishstemmer = sb_stemmer_new("swedish", "ISO_8859_1");
    stem = sb_stemmer_stem(swedishstemmer,(sb_symbol *)inword, i);
    break;
  case 9:
    /* printf("Using Portuguese Stemmer\n"); */
    if (portuguesestemmer == NULL)
      portuguesestemmer = sb_stemmer_new("portuguese", "ISO_8859_1");
    stem = sb_stemmer_stem(portuguesestemmer,(sb_symbol *)inword, i);
    break;
  case 10:
    /*  printf("Using Russian (UTF-8) Stemmer\n"); */
    /* UTF-8 data == 2 bytes per letter this */
    if (russianstemmerutf == NULL)
      russianstemmerutf = sb_stemmer_new("russian", "UTF_8");
    stem = sb_stemmer_stem(russianstemmerutf,(sb_symbol *)inword, i);
    break;
  case 11:
    /*  printf("Using Danish  Stemmer\n"); */
    if (danishstemmer == NULL)
      danishstemmer = sb_stemmer_new("danish", "ISO_8859_1");
    stem = sb_stemmer_stem(danishstemmer,(sb_symbol *)inword, i);
    break;
  case 12:
    /*   printf("Using Norwegian Stemmer\n"); */
    if (norwegianstemmer == NULL)
      norwegianstemmer = sb_stemmer_new("norwegian", "ISO_8859_1");
    stem = sb_stemmer_stem(englishstemmer,(sb_symbol *)inword, i);
    break;

  case 13:
    /*  printf("Using Russian (KOI-8) Stemmer\n"); */
    /* KOI-8 data == one byte per letter */
    if (russianstemmer == NULL)
      russianstemmer = sb_stemmer_new("russian", "KOI8_R");
    stem = sb_stemmer_stem(russianstemmer,(sb_symbol *)inword, i);
    break;

  case 14:
    /* Using finnish stemmer */
    if (finnishstemmer == NULL)
      finnishstemmer = sb_stemmer_new("finnish", "ISO_8859_1");
    stem = sb_stemmer_stem(finnishstemmer,(sb_symbol *)inword, i);
    break;

  case 15:
    /* Using Hungarian stemmer */
    if (hungarianstemmer == NULL)
      hungarianstemmer = sb_stemmer_new("hungarian", "ISO_8859_1");
    stem = sb_stemmer_stem(hungarianstemmer,(sb_symbol *)inword, i);
    break;

  case 16:
    /* Using Turkish stemmer */
    if (turkishstemmerutf == NULL)
      turkishstemmerutf = sb_stemmer_new("turkish", "UTF_8");
    stem = sb_stemmer_stem(turkishstemmerutf,(sb_symbol *)inword, i);
    break;

  case 17:
    /* Using Romanian stemmer */
    if (romanianstemmer == NULL)
      romanianstemmer = sb_stemmer_new("romanian", "ISO_8859_1");
    stem = sb_stemmer_stem(romanianstemmer,(sb_symbol *)inword, i);
    break;


  case 101:
    /* printf("Using Porter Stemmer\n"); */
    if (porterstemmerutf == NULL)
      porterstemmerutf = sb_stemmer_new("porter", "UTF_8");
    stem = sb_stemmer_stem(porterstemmerutf,(sb_symbol *)inword, i);      
    break;
  case 102:
    /* printf("Using English (porter2) Stemmer\n"); */
    if (englishstemmerutf == NULL)
      englishstemmerutf = sb_stemmer_new("english", "UTF_8");
    stem = sb_stemmer_stem(englishstemmerutf,(sb_symbol *)inword, i);
    break;
  case 103:
    /* printf("Using French Stemmer\n"); */

    if (frenchstemmerutf == NULL)
      frenchstemmerutf = sb_stemmer_new("french", "UTF_8");
    stem = sb_stemmer_stem(frenchstemmerutf,(sb_symbol *)inword, i);
    break;
  case 104:
    /* printf("Using German Stemmer\n"); */
    if (germanstemmerutf == NULL)
      germanstemmerutf = sb_stemmer_new("german", "UTF_8");
    stem = sb_stemmer_stem(germanstemmerutf,(sb_symbol *)inword, i);
    break;
  case 105:
    /* printf("Using Dutch Stemmer\n"); */
    if (dutchstemmerutf == NULL)
      dutchstemmerutf = sb_stemmer_new("dutch", "UTF_8");
    stem = sb_stemmer_stem(dutchstemmerutf,(sb_symbol *)inword, i);
    break;
  case 106:
    /*printf("Using Spanish Stemmer\n"); */
    if (spanishstemmerutf == NULL)
      spanishstemmerutf = sb_stemmer_new("spanish", "UTF_8");
    stem = sb_stemmer_stem(spanishstemmerutf,(sb_symbol *)inword, i);
    break;
  case 107:
    /* printf("Using Italian Stemmer\n"); */
    if (italianstemmerutf == NULL)
      italianstemmerutf = sb_stemmer_new("italian", "UTF_8");
    stem = sb_stemmer_stem(italianstemmerutf,(sb_symbol *)inword, i);
    break;
  case 108:
    /* printf("Using Swedish Stemmer\n"); */
    if (swedishstemmerutf == NULL)
      swedishstemmerutf = sb_stemmer_new("swedish", "UTF_8");
    stem = sb_stemmer_stem(swedishstemmerutf,(sb_symbol *)inword, i);
    break;
  case 109:
    /* printf("Using Portuguese Stemmer\n"); */
    if (portuguesestemmerutf == NULL)
      portuguesestemmerutf = sb_stemmer_new("portuguese", "UTF_8");
    stem = sb_stemmer_stem(portuguesestemmerutf,(sb_symbol *)inword, i);
    break;
  case 110:
    /*  printf("Using Russian (UTF-8) Stemmer\n"); */
    /* UTF-8 data == 2 bytes per letter this */
    if (russianstemmerutf == NULL)
      russianstemmerutf = sb_stemmer_new("russian", "UTF_8");
    stem = sb_stemmer_stem(russianstemmerutf,(sb_symbol *)inword, i);
    break;
  case 111:
    /*  printf("Using Danish  Stemmer\n"); */
    if (danishstemmerutf == NULL)
      danishstemmerutf = sb_stemmer_new("danish", "UTF_8");
    stem = sb_stemmer_stem(danishstemmerutf,(sb_symbol *)inword, i);
    break;
  case 112:
    /*   printf("Using Norwegian Stemmer\n"); */
    if (norwegianstemmerutf == NULL)
      norwegianstemmerutf = sb_stemmer_new("norwegian", "UTF_8");
    stem = sb_stemmer_stem(englishstemmerutf,(sb_symbol *)inword, i);
    break;

  case 113:
    /*  printf("Using Russian (KOI-8) Stemmer\n"); */
    /* KOI-8 data == one byte per letter */
    if (russianstemmerutf == NULL)
      russianstemmerutf = sb_stemmer_new("russian", "KOI8_R");
    stem = sb_stemmer_stem(russianstemmerutf,(sb_symbol *)inword, i);
    break;

  case 114:
    /* Using finnish stemmer */
    if (finnishstemmerutf == NULL)
      finnishstemmerutf = sb_stemmer_new("finnish", "UTF_8");
    stem = sb_stemmer_stem(finnishstemmerutf,(sb_symbol *)inword, i);
    break;

  case 115:
    /* Using Hungarian stemmer */
    if (hungarianstemmerutf == NULL)
      hungarianstemmerutf = sb_stemmer_new("hungarian", "UTF_8");
    stem = sb_stemmer_stem(hungarianstemmerutf,(sb_symbol *)inword, i);
    break;

  case 116:
    /* Using Turkish stemmer */
    if (turkishstemmerutf == NULL)
      turkishstemmerutf = sb_stemmer_new("turkish", "UTF_8");
    stem = sb_stemmer_stem(turkishstemmerutf,(sb_symbol *)inword, i);
    break;

  case 117:
    /* Using Romanian stemmer */
    if (romanianstemmerutf == NULL)
      romanianstemmerutf = sb_stemmer_new("romanian", "UTF_8");
    stem = sb_stemmer_stem(romanianstemmerutf,(sb_symbol *)inword, i);
    break;
    
  }

  if (stem != NULL) {
    resultword = strdup((char *)stem);
    return(resultword);
  }
  else
    return(inword);

}


char *delete_snowball_stemmers() 
{

    /* printf("Using Porter Stemmer\n"); */
    if (porterstemmer != NULL)
    sb_stemmer_delete(porterstemmer);      
  
    /* printf("Using English (porter2) Stemmer\n"); */
    if (englishstemmer != NULL)
    sb_stemmer_delete(englishstemmer);
  
    /* printf("Using French Stemmer\n"); */
    if (frenchstemmer != NULL)
    sb_stemmer_delete(frenchstemmer);
  
    /* printf("Using German Stemmer\n"); */
    if (germanstemmer != NULL)
    sb_stemmer_delete(germanstemmer);
  
    /* printf("Using Dutch Stemmer\n"); */
    if (dutchstemmer != NULL)
    sb_stemmer_delete(dutchstemmer);
  
    /*printf("Using Spanish Stemmer\n"); */
    if (spanishstemmer != NULL)
    sb_stemmer_delete(spanishstemmer);
  
    /* printf("Using Italian Stemmer\n"); */
    if (italianstemmer != NULL)
    sb_stemmer_delete(italianstemmer);
  
    /* printf("Using Swedish Stemmer\n"); */
    if (swedishstemmer != NULL)
    sb_stemmer_delete(swedishstemmer);
  
    /* printf("Using Portuguese Stemmer\n"); */
    if (portuguesestemmer != NULL)
    sb_stemmer_delete(portuguesestemmer);
  
      /*  printf("Using Danish  Stemmer\n"); */
    if (danishstemmer != NULL)
    sb_stemmer_delete(danishstemmer);
  
    /*   printf("Using Norwegian Stemmer\n"); */
    if (norwegianstemmer != NULL)
    sb_stemmer_delete(englishstemmer);

  
    /*  printf("Using Russian (KOI-8) Stemmer\n"); */
    /* KOI-8 data == one byte per letter */
    if (russianstemmer != NULL)
    sb_stemmer_delete(russianstemmer);

  
    /* Using finnish stemmer */
    if (finnishstemmer != NULL)
    sb_stemmer_delete(finnishstemmer);

  
    /* Using Hungarian stemmer */
    if (hungarianstemmer != NULL)
    sb_stemmer_delete(hungarianstemmer);

  
    /* Using Turkish stemmer */
    if (turkishstemmerutf != NULL)
    sb_stemmer_delete(turkishstemmerutf);

  
    /* Using Romanian stemmer */
    if (romanianstemmer != NULL)
    sb_stemmer_delete(romanianstemmer);


    /* printf("Using Porter Stemmer\n"); */
    if (porterstemmerutf != NULL)
    sb_stemmer_delete(porterstemmerutf);      
  
    /* printf("Using English (porter2) Stemmer\n"); */
    if (englishstemmerutf != NULL)
    sb_stemmer_delete(englishstemmerutf);
  
    /* printf("Using French Stemmer\n"); */

    if (frenchstemmerutf != NULL)
    sb_stemmer_delete(frenchstemmerutf);
  
    /* printf("Using German Stemmer\n"); */
    if (germanstemmerutf != NULL)
    sb_stemmer_delete(germanstemmerutf);
  
    /* printf("Using Dutch Stemmer\n"); */
    if (dutchstemmerutf != NULL)
    sb_stemmer_delete(dutchstemmerutf);
  
    /*printf("Using Spanish Stemmer\n"); */
    if (spanishstemmerutf != NULL)
    sb_stemmer_delete(spanishstemmerutf);
  
    /* printf("Using Italian Stemmer\n"); */
    if (italianstemmerutf != NULL)
    sb_stemmer_delete(italianstemmerutf);
  
    /* printf("Using Swedish Stemmer\n"); */
    if (swedishstemmerutf != NULL)
    sb_stemmer_delete(swedishstemmerutf);
  
    /* printf("Using Portuguese Stemmer\n"); */
    if (portuguesestemmerutf != NULL)
    sb_stemmer_delete(portuguesestemmerutf);
  
    /*  printf("Using Russian (UTF-8) Stemmer\n"); */
    /* UTF-8 data == 2 bytes per letter this */
    if (russianstemmerutf != NULL)
    sb_stemmer_delete(russianstemmerutf);
  
    /*  printf("Using Danish  Stemmer\n"); */
    if (danishstemmerutf != NULL)
    sb_stemmer_delete(danishstemmerutf);
  
    /*   printf("Using Norwegian Stemmer\n"); */
    if (norwegianstemmerutf != NULL)
    sb_stemmer_delete(englishstemmerutf);

    /* Using finnish stemmer */
    if (finnishstemmerutf != NULL)
    sb_stemmer_delete(finnishstemmerutf);

  
    /* Using Hungarian stemmer */
    if (hungarianstemmerutf != NULL)
    sb_stemmer_delete(hungarianstemmerutf);

  
    /* Using Turkish stemmer */
    if (turkishstemmerutf != NULL)
    sb_stemmer_delete(turkishstemmerutf);

  
    /* Using Romanian stemmer */
    if (romanianstemmerutf != NULL)
    sb_stemmer_delete(romanianstemmerutf);
    

}

