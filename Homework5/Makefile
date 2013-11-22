CC=gcc
DEBUG=-g
CFLAGS=$(DEBUG) -Wall -Wshadow -Wunreachable-code -Wredundant-decls -Wmissing-declarations -Wold-style-definition -Wmissing-prototypes -Wdeclaration-after-statement
PROGS=pipeline uniqify

all: $(PROGS)

pipeline: pipeline.o
	$(CC) $(CFLAGS) -o pipeline pipeline.c

pipeline.o: pipeline.c
	$(CC) $(CFLAGS) -c pipeline.c

uniqify: uniqify.o
	$(CC) $(CFLAGS) -o uniqify uniqify.c

uniqify.o: uniqify.c
	$(CC) $(CFLAGS) -c uniqify.c

clean:
	rm -f $(PROGS) *.o *~ *.ar
	rm -f myuniqify_t uniqify_t
	rm -f mypipeline1 mypipeline2 pipeline1 pipeline2

test_pipeline:
	rm -f mypipeline1 mypipeline2 pipeline1 pipeline2
	test/./pipeline.bash -f pipeline2 < test/words.txt > pipeline1
	./pipeline -f mypipeline2 < test/words.txt > mypipeline1
	diff mypipeline1 pipeline1
	diff mypipeline2 pipeline2

test_uniqify:
	rm -f myuniqify_t uniqify_t
	test/./uniquify.bash < test/websters.txt > uniqify_t
	./uniqify -n 5 < test/websters.txt > myuniqify_t
	diff uniqify_t myuniqify_t

test_uniqify_decl:
	rm -f myuniqify_t uniqify_t
	test/./uniquify.bash < test/decl-indep.txt > uniqify_t
	./uniqify -n 5 < test/decl-indep.txt > myuniqify_t
	diff uniqify_t myuniqify_t

test: test_pipeline test_uniqify
