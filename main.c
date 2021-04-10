#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <stdlib.h>

typedef struct record1 {
  char word[50];    // key
  int ptr;      // posicion de primer record2 en el archivo 2
} record1;

typedef struct record2 {
  int line; // linea en el archivo Alice
  int next; // sig posicion dentro de A2
} record2;


int traverseArchivo1(FILE* ar, char* target) {
  // busca secuencialmente la palabra target en archivo 1
  int i=0;
  while (fseek(ar, sizeof(record1)*i, SEEK_SET) == 0){
    record1 r1;
    fread(&r1, sizeof(record1), 1, ar);
    if(strcmp(r1.word, target)==0){
     // printf("Duplicated %s\n", target);
      return i; //Repetido entonces no escribimos
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


/*  MAIN  */
int main(void) {
  FILE * archivo1 = fopen("archivo1", "wb+");
  FILE * archivo2 = fopen("archivo2", "wb+");
  
  FILE *file;
  file = fopen("alice.txt", "r");
  ssize_t getline(char **lineptr, size_t *n, FILE *stream);

  int posA2 = 0;
  
  // LEER LÍNEA POR LÍNEA
  char* line;
  size_t size = sizeof(char)*100;  // numero de bytes
  line = (char *) malloc(size);
  int line_index = 1;
  while( getline(&line, &size, file) != -1 ) {
    //printf("Line: %d", line_index);
    //printf("%s\n", line);
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
  imprimirArchivo1(archivo1);
  printf("===============\n");
  imprimirArchivo2(archivo2);
  fclose(archivo1);

  // for(int i= 0; i<20; i++){
  //   record2 r2;
  //   r2.line = i*10;
  //   r2.next =  50;
  //   fwrite(&r2, sizeof(record2),1, archivo1);
  // }
/*
  fseek(ar, sizeof(record2)*5, SEEK_SET);
  record2 r2;
  fread(&r2, sizeof(record2), 1, ar);
  printf("line %d , next %d\n", r2.line, r2.next);
  printf("Tamaño record2 %zu", sizeof(record2));
  fclose(ar);
*/
  return 0;
}

/*/

size_t fpread(void *buffer, size_t size, size_t mitems, size_t offset, FILE *fp)
{
     if (fseek(fp, offset, SEEK_SET) != 0)
         return 0;
     return fread(buffer, size, nitems, fp);
}
/*/