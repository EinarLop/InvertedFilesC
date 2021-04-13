#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <stdlib.h>
/* INVERTED INDEX GENERATOR*/

typedef struct record1 {
  char word[50];    // palabra o llave
  int ptr;      // posicion de primer record2 en el archivo 2
} record1;

typedef struct record2 {
  int line; // linea en el archivo .txt
  int next; // sig posicion dentro de A2
} record2;

typedef struct {
  record1* array;  // guarda estructuras record1 
  size_t used;
  size_t size;
} Array;

void initArray(Array*, size_t);
void insertArray(Array* , record1);
void freeArray(Array*);

void testArray(){
  Array a;
  initArray(&a, 5);  // initially 5 elements
  for (int i = 0; i < 10; i++) {
    record1* r = (record1 *) malloc(sizeof(record1));
    r->ptr = i;
    insertArray(&a, *r);  // automatically resizes as necessary
  }
  printf("1st elem: %d\n", a.array[0].ptr);
  printf("10th elem: %d\n", a.array[9].ptr);  // print 10th element
  printf("# records: %d\n", (int)a.used);  // print number of elements
  freeArray(&a);
}

int compareUser(const void *v1, const void *v2)
{
  // comparación para ordenamiento de registros 1
  const struct record1* r1 = v1;
  const struct record1* r2 = v2;
  return strcmp(r1->word, r2->word);
}

record1 rec1Arr [4];

int traverseArchivo1(FILE* ar, char* target) {
  // busca secuencialmente el target string dentro de archivo 1
  // devuelve la posición donde se encontró, -1 si no está.
  int i = 0;
  while (fseek(ar, sizeof(record1)*i, SEEK_SET) == 0){
    record1 r1;
    fread(&r1, sizeof(record1), 1, ar);
    if(strcmp(r1.word, target)==0){
      return i;
    }
    if (feof(ar)) {
      // Indica si tratamos de escribir más allá de EOF
      break;
    }
    i++;
  }
  return -1;  // not found
}

/* IMPRIMIR ARCHIVOS */
int imprimirArchivo1(FILE* ar) {
  int i=0;
  
  while (fseek(ar, sizeof(record1)*i, SEEK_SET) == 0){
    record1 r1;
    fread(&r1, sizeof(record1), 1, ar);
    if (feof(ar)) {
      // Indica si tratamos de escribir más allá de EOF
      break;
    }
    printf("R1 Word:%s\n R1 ptr: %d\n",r1.word, r1.ptr);
    i++;
  }
  return 0;
}

int imprimirArchivo2(FILE* ar) {
  int i=0;
  while (fseek(ar, sizeof(record2)*i, SEEK_SET) == 0){
    record2 r2;
    fread(&r2, sizeof(record2), 1, ar);
    if (feof(ar)) {
      // Indica si tratamos de escribir más allá de EOF
      break;
    }
    printf("R2 %d\t Line occurence: %d\n R2 next: %d\n",i, r2.line, r2.next);
    i++;
  }
  return 0;
}

int pruebaSort() {
  printf("Inicia main prueba\n");
  Array a;
  initArray(&a, 6);  // initially 5 elements
  printf("Creando elems\n");
  record1* r = (record1 *) malloc(sizeof(record1));
  r->ptr = 1;
  strcpy(r->word, "wonderland");

  record1* r1 = (record1 *) malloc(sizeof(record1));
  r->ptr = 1;
  strcpy(r1->word, "alice");
  
  record1* r2 = (record1 *) malloc(sizeof(record1));
  r2->ptr = 1;
  strcpy(r2->word, "bunny");
  
  record1* r3 = (record1 *) malloc(sizeof(record1));
  r3->ptr = 1;
  strcpy(r3->word, "end");
  
  record1* r4 = (record1 *) malloc(sizeof(record1));
  r4->ptr = 1;  
  strcpy(r4->word, "hat");
  
  printf("Insertando elems\n");
  insertArray(&a, *r);
  insertArray(&a, *r1);
  insertArray(&a, *r2);
  insertArray(&a, *r3);
  insertArray(&a, *r4);

  printf("Before SORT\n");
   for(int i=0; i<a.used; i++){
       printf("%s\t", a.array[i].word);
   }
  qsort(a.array, a.used, sizeof(record1), compareUser);
  printf("AFTER SORT\n");
  for(int i=0; i<a.used; i++) {
    printf("%s\t", a.array[i].word);
  }
  return 0;
}

/*  MAIN  */
int main(void) {
  FILE * archivo1 = fopen("archivo1", "wb+");
  FILE * archivo2 = fopen("archivo2", "wb+");
  
  FILE *file;
  file = fopen("alice.txt", "r");
  ssize_t getline(char **lineptr, size_t *n, FILE *stream);

  int posA2 = 0;
  Array a;
  initArray(&a, 50);  // initially 50 elements
  
  // LEER LÍNEA POR LÍNEA
  char* line;
  size_t size = sizeof(char)*100;  // numero de bytes
  line = (char *) malloc(size);
  int line_index = 1;
  while( getline(&line, &size, file) != -1 ) {
    const char s[2] = " ";
    char *token;
    /* get the first token */
    token = strtok(line, s);
   /* walk through other tokens */
    while( token != NULL ) {
        // printf( " %s\n", token );
      char clean[50];
      int j = 0;
      for (int i=0; token[i]!='\0'; i++) {
      //  printf("%c\n", token[i]);
          // convertir mayusculas a minusculas
          if(token[i]>='A' && token[i]<='Z'){
            token[i] = token[i]+32;
          }
          // si es una letra, copiala a clean[j]
          if(token[i]>= 'a' && token[i]<='z'){
            clean[j]=token[i];
            j++;
            continue;
          } else {    // dejamos de copiar si ya tenemos al menos 1
            if (j>=1) break;
          }
          // Casos especiales: contracciones I've, I'm, I'll
      }
    
      if (strcmp("", clean)!=0) {
        int positionA1  = traverseArchivo1(archivo1, clean); 
        if (positionA1 == -1) {
        // if string not empty, write to archivo1
          record1 r1;
          strcpy(r1.word, clean);
          r1.ptr = posA2;  
          fwrite(&r1, sizeof(record1), 1, archivo1);
          insertArray(&a, r1);
          // TODO: CREAR PRIMER ENTRADA EN ARCHIVO 2
          // 1. Tomar la línea actual (line_index)
          // 2. Tomar la posA2 actual
          // 3. Escribir en Archivo2 desde posA2 la line_index y -1
          record2 r2;
          r2.line = line_index;
          r2.next = -1;
          fseek(archivo2, sizeof(record2)*posA2, SEEK_SET);
          fwrite(&r2, sizeof(record2), 1, archivo2);
          
          // 4. Incrementar posA2.
          posA2++;
        } else { 
          // Duplicate word is at positionA1
            fseek(archivo1, sizeof(record1)*positionA1, SEEK_SET); // move to positionA1 in archivo 1
            record1 r1;     
            fread(&r1, sizeof(record1), 1, archivo1);
            //printf("R1ptr for %s : %d\n", r1.word, r1.ptr);
            // Then move to r1.ptr in in A2
            fseek(archivo2, sizeof(record2)*r1.ptr, SEEK_SET);
            record2 r2;
            fread(&r2, sizeof(record2), 1, archivo2); // read in r2
            //printf("R2.next: %d\n", r2.next);
            int prevR2 = r1.ptr;      
            while (r2.next != -1) {   // traverse linked list until finish
              prevR2 = r2.next;  // last valid pointer in R2
              //printf("Lista invertida next: %d", r2.next);
              fseek(archivo2, sizeof(record2)*r2.next, SEEK_SET);
              fread(&r2, sizeof(record2), 1, archivo2); // reasignas r2
            }
            // while termina cuando r2.next == -1; y llegamos al último
            r2.next = posA2;
            fseek(archivo2, sizeof(record2)*prevR2, SEEK_SET);
            fwrite(&r2, sizeof(record2), 1, archivo2);
            //printf("R2.next is now %d\n", r2.next);
            record2 newR2;
            newR2.line = line_index;
            newR2.next = -1;
            fseek(archivo2, sizeof(record2)*posA2, SEEK_SET);
            fwrite(&newR2, sizeof(record2), 1, archivo2);
            posA2++;
            // (linea, siguiente) ->fseek(tamañor2*siguiente) (linea, siguiente) -> (linea, -1)
          // Modificar next de A2 cuando topemos con -1
          // Crear nuevo registro 2 al final de A2
          // Incrementar posA2
        }
      }
      memset(clean,'\0',sizeof(clean)); 
      token = strtok(NULL, s); // lee el siguiente token
    }
    line_index++;  // next line counter
  }

  qsort(a.array, a.used, sizeof(record1), compareUser);
  fseek(archivo1, 0, SEEK_SET);
  
  for(int i=0; i<a.used; i++) {
    fwrite(&(a.array[i]), sizeof(record1), 1, archivo1);
  }
  
/*   printf("===============Imprimiendo Archivo 1\n");
  imprimirArchivo1(archivo1);
  printf("===============\n");
  printf("===============Imprimiendo Archivo 2\n");
  imprimirArchivo2(archivo2);
  printf("===============\n"); */

  fclose(archivo1);
  fclose(archivo2);
  freeArray(&a);

  return 0;
}

/* DYNAMIC ARRAY METHODS */
// initialize an array of record1 pointers
void initArray(Array* a, size_t initialSize) {
  a->array = malloc(initialSize * sizeof(record1));
  a->used = 0;
  a->size = initialSize;
}

/* insert a record1 pointer to the array, if number of used elements
   is equal to array size, double the size of the array. */
void insertArray(Array *a, record1 element) {
  // Syntax a->array[a->used++] updates a->used only *after* the array has been accessed.
  if (a->used == a->size) {
    a->size *= 2;
    a->array = realloc(a->array, a->size * sizeof(record1));
  }
  a->array[a->used++] = element;
}

void freeArray(Array *a) {
  free(a->array);
  a->array = NULL;
  a->used = a->size = 0;
}