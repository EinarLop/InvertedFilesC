def: files
	gcc -o wr word_retrieval.c
	./wr

files: 
	gcc -o ex inverted_index.c
	./ex