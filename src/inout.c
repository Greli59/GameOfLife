#include <dirent.h>
#include <sys/types.h>
#include <string.h>
#include "inout.h"

#define DEBUG 1
#define BUF_SIZE 256

int load_seed(const char *file_name, bool universe[HEIGHT][WIDTH]) {
  FILE *fp;
  int w, h;
  char buffer[BUF_SIZE];

  // Opening
  if ((fp = fopen(file_name, "rb")) == NULL) {
#ifdef DEBUG
    printf("[load_seed] Cannot open file %s\n",
           file_name);
#endif
    return 0;
  }
#ifdef DEBUG
  printf("[load_seed] File %s opened\n", file_name);
#endif
  
  // Size
  if (fgets((char *) buffer, BUF_SIZE, fp) == NULL) {
#ifdef DEBUG
    printf("[load_seed] File read error\n");
#endif
    return 0;
  }
  if (sscanf((char *) buffer, "%d %d", &w, &h) == 0) {
#ifdef DEBUG
    printf("[load_seed] Cannot read universe dimensions\n");
#endif
    return 0;
  } else {
#ifdef DEBUG
    printf("[load_seed] Dimensions: w=%d,h=%d\n",w,h);
#endif
  }
  if ((w != WIDTH) || (h != HEIGHT)) {
#ifdef DEBUG
    printf("[load_seed] Seed parameters do no match requirements\n");
    printf("w=%d,h=%d\n", w, h);
    return 0;
#endif
  }

  // Universe
  if (universe == NULL) {
#ifdef DEBUG
    printf("[load_seed] Universe incorrectly allocated\n");
#endif
    return 0;
  }

  /* Read cells */
  for(int i=0; i<HEIGHT; i++) {
    if (fgets((char *) buffer, BUF_SIZE, fp) == NULL ||
	strlen((char*)buffer) <WIDTH) {
#ifdef DEBUG
      printf("[load_seed] Not enough cells\n");
#endif
      return 0;
    }
    for(int j=0; j<WIDTH; j++) {
      if (buffer[j]=='1')
	universe[i][j]=true;
      else if(buffer[j]=='0')
	universe[i][j]=false;
      else {
#ifdef DEBUG
	printf("[load_seed] Incorrect cell value: %d\n", buffer[j]);
#endif
	return 0;
      }
    }
  }
  
  /* Everything ok if we get here */
  fclose(fp);
#ifdef DEBUG
  printf("[load_seed] File closed\n");
#endif
  return 1;
}
