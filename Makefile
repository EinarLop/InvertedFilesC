last: first
	gcc -o wr word_retrieval.c
	./wr

first: 
	gcc -o inv inverted_index.c
	./inv

clean:
	rm archivo1 archivo2 inv wr