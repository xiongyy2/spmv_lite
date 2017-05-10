
CC       = pgcc
CCFLAGS  = 
ACCFLAGS = -acc -ta=radeon -Minfo=all -Minline
#ACCFLAGS = -acc -ta=nvidia -Minfo=accel
OMPFLAGS = -fast -mp -Minfo -Mvect=levels:6
RM       = rm

BIN =  main_omp main_csr5_seq main_csr5_omp

all: $(BIN)

#main_acc: csr_main1.c csr_cpu.c csr_ompacc.c allthefunctions.c
	#$(CC) $(CCFLAGS) $(ACCFLAGS) -o $@ $^

main_omp: csr_main1.c csr_cpu.c csr_ompacc.c allthefunctions.c
	$(CC) $(CCFLAGS) $(OMPFLAGS) -o $@ $^

main_csr5_seq: csr_main1.c csr_cpu.c csr5_seq.c allthefunctions.c
	$(CC) $(CCFLAGS) -o $@ $^

main_csr5_omp: csr_main1.c csr_cpu.c csr5_ompacc.c allthefunctions.c
	$(CC) $(CCFLAGS) $(OMPFLAGS) -o $@ $^

#main_csr5_acc: csr_main1.c csr_cpu.c csr5_ompacc.c allthefunctions.c
	#$(CC) $(CCFLAGS) $(ACCFLAGS) -o $@ $^

clean:
	$(RM) $(BIN) *.o