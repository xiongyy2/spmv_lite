
CC       = pgcc
CCFLAGS  = 
ACCFLAGS = -acc -ta=radeon -Minfo=accel
#ACCFLAGS = -acc -ta=nvidia -Minfo=accel
OMPFLAGS = -fast -mp -Minfo
RM       = rm

BIN =  main_omp main_acc main_cpu_gcc main_cpu_pgcc

all: $(BIN)

main_acc: csr_main1.c csr_cpu.c csr_ompacc.c sparse_formats.c ziggurat.c
	$(CC) $(CCFLAGS) $(ACCFLAGS) -o $@ $^

main_omp: csr_main1.c csr_cpu.c csr_ompacc.c allthefunctions.c
	$(CC) $(CCFLAGS) $(OMPFLAGS) -o $@ $^

clean:
	$(RM) $(BIN) *.o