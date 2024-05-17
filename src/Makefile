CC=gcc
CFLAGS=-Wall -Wextra
DEPS = multi_set.h test_program.h
OBJ_MAIN = main.o single_thread_test.o more_reads_test.o more_writes_test.o equal_reads_writes_test.o custom_test.o
OBJ_COARSE = coarse_grained.o $(OBJ_MAIN)
OBJ_FINE = fine_grained.o $(OBJ_MAIN)
OBJ_OPT = optimistic_sync.o $(OBJ_MAIN)
OBJ_LAZY = lazy_sync.o $(OBJ_MAIN)
EXE_COARSE = coarse_grained
EXE_FINE = fine_grained
EXE_OPT = opt_sync
EXE_LAZY = lazy_sync

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

$(EXE_COARSE): $(OBJ_COARSE)
	$(CC) -o $@ $^ $(CFLAGS)

$(EXE_FINE): $(OBJ_FINE)
	$(CC) -o $@ $^ $(CFLAGS)

$(EXE_OPT): $(OBJ_OPT)
	$(CC) -o $@ $^ $(CFLAGS)

$(EXE_LAZY): $(OBJ_LAZY)
	$(CC) -o $@ $^ $(CFLAGS)

.PHONY: all clean run_coarse run_fine run_opt run_lazy

all: $(EXE_COARSE) $(EXE_FINE) $(EXE_OPT) $(EXE_LAZY)

clean:
	rm -f *.o $(EXE_COARSE) $(EXE_FINE) $(EXE_OPT) $(EXE_LAZY)

run_coarse: $(EXE_COARSE)
	./$(EXE_COARSE)

run_fine: $(EXE_FINE)
	./$(EXE_FINE)

run_opt: $(EXE_OPT)
	./$(EXE_OPT)

run_lazy: $(EXE_LAZY)
	./$(EXE_LAZY)