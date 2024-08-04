CC=/usr/bin/gcc
CFLAGS= -g3 -Wall -Werror -Wextra `pkg-config fuse --cflags --libs`
INSTALL= sudo apt install
OBJ = program-10.o
PROGRAM = program-10
FS=fs

%.o: %.c %.h $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS) $(LIBRARIES)

$(PROGRAM): $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBRARIES) 

.PHONY: fuse clean

install-fuse:
	$(INSTALL) libfuse2 libfuse-dev

run-fuse:
	@mkdir -p $(FS)
	@./$(PROGRAM) -f $(FS)

generate-files:
	@echo "fuse" > $(FS)/example.txt;
	@echo `date +%s%N` > $(FS)/test.log;
	@ls -ltrh $(FS)

clean:
	rm $(PROGRAM) *.o
	rm fs -r
