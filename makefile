CC = c++
CF = -c -I /usr/local/include/ -o

model: serotiny_model
docs: serotiny_model_user_guide.pdf

#--------------------
#      MODEL
#--------------------
serotiny_model : lib/main.o lib/landscape.o lib/params.o lib/global.o lib/io.o lib/stats.o lib/iterator.o
	$(CC) -o serotiny_model -L /usr/local/lib/ -lgsl -lgslcblas lib/main.o lib/landscape.o lib/params.o lib/global.o lib/io.o lib/stats.o lib/iterator.o
	
lib/main.o : src/main.cpp hdr/main.h hdr/landscape.h hdr/params.h hdr/iterator.h hdr/global.h hdr/stats.h
	$(CC) $(CF) lib/main.o src/main.cpp
	
lib/landscape.o : src/landscape.cpp hdr/landscape.h hdr/params.h hdr/stats.h
	mkdir -p lib
	$(CC) $(CF) lib/landscape.o src/landscape.cpp
	
lib/params.o : src/params.cpp hdr/params.h hdr/global.h hdr/io.h
	mkdir -p lib
	$(CC) $(CF) lib/params.o src/params.cpp

lib/global.o : src/global.cpp hdr/global.h
	mkdir -p lib
	$(CC) $(CF) lib/global.o src/global.cpp

lib/io.o : src/io.cpp hdr/io.h
	mkdir -p lib
	$(CC) $(CF) lib/io.o src/io.cpp
	
lib/stats.o : src/stats.cpp hdr/stats.h
	mkdir -p lib
	$(CC) $(CF) lib/stats.o src/stats.cpp
	
lib/iterator.o : src/iterator.cpp hdr/iterator.h hdr/params.h hdr/io.h hdr/global.h hdr/stats.h
	mkdir -p lib
	$(CC) $(CF) lib/iterator.o src/iterator.cpp

serotiny_model_user_guide.pdf: doc/serotiny_model_user_guide.tex doc/user_guide_bib.bib
	cd doc; pdflatex serotiny_model_user_guide; bibtex serotiny_model_user_guide; pdflatex serotiny_model_user_guide; pdflatex serotiny_model_user_guide
	mv doc/serotiny_model_user_guide.pdf serotiny_model_user_guide.pdf