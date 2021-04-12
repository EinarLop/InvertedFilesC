#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Word Retrieval 
1. Console Menu reads a word given by the user
-> Run binary search across file 1 for (word)
-> Read ptr attribute
-> Read from 2 file considering ptr and follow linked list until next is -1
-> While following linked list print attribute record2.line

3. Asks for user input as many times as necessary until user quits.
*/

typedef struct record1 {
  char word[50];    // palabra o llave
  int ptr;      // posicion de primer record2 en el archivo 2
} record1;

typedef struct record2 {
  int line; // linea en el archivo .txt
  int next; // sig posicion dentro de A2
} record2;  

int search(char* word, int len, FILE * archivo1) {
  record1 r1;
  fread(&r1, sizeof(record1), 1, archivo1);
  len = 2530; 
  int hi = len-1;
  int lo = 0;
  int mid;
  while (lo <= hi) {
    mid = lo + (hi-lo)/2;
    record1* r1 = (record1 *) malloc(sizeof(record1));
    fseek(archivo1, sizeof(record1)*mid, SEEK_SET);
    fread(r1, sizeof(record1), 1, archivo1);
    if(strcmp(r1->word, word) == 0){
      return r1->ptr;
    }
    else if(strcmp(r1->word, word)>0){
      hi = mid-1;
    }
     else {
      lo = mid+1;
    }
    free(r1); 
  }
  return -1;  // not found
}

void positions (int r1ptr, FILE * archivo2){
  record2 r2;
  // Initialize cursor position ar r1ptr
  fseek(archivo2, sizeof(record2)*r1ptr, SEEK_SET);
  fread(&r2, sizeof(record2), 1, archivo2);
  printf("Line occurrences: %d", r2.line);
  int prevLine;
  while(r2.next != -1){
    prevLine = r2.line;
    fseek(archivo2, sizeof(record2)*r2.next, SEEK_SET);
    fread(&r2, sizeof(record2), 1, archivo2);
    if (r2.line != prevLine) {
      printf("\t%d", r2.line);
    }
  }
  putchar('\n');
}

//2530
int validateWord(char * word) {
  for (int i=0; word[i] != '\0'; i++) {
    if (word[i] >= 'A' && word[i] <= 'Z') {
      word[i] += 32;
    } else if (word[i] <'a' || word[i] >'z') {
      return -1;    // invalid word
    }
  }
  return 0;
}

int main() {
  // Open our inverted index files.
  FILE * archivo1 = fopen("archivo1", "rb");
  FILE * archivo2 = fopen("archivo2", "rb");
  char word[50];
  int user = 1;
  while (user == 1) {
    printf("Type the word to search: ");
    scanf("%s", word);
    if (validateWord(word)==0){
      printf("Your word: >%s<\n", word);
      int start = search(word, 2530, archivo1);
      if (start != -1) {
        positions(start, archivo2);
      } else {
        printf("No occurrences \n");
      }
    } else {
    printf("Invalid word. Try again\n");
    }
    printf("Try another word? 1 for yes, 0 for no\n");
    scanf("%d", &user);
  }

  printf("\n BYE"); 
  return 0;
}