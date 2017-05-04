
CC       = pgcc
CCFLAGS  = 
ACCFLAGS = -acc -ta=radeon -Minfo=accel
#ACCFLAGS = -acc -ta=nvidia -Minfo=accel
OMPFLAGS = -fast -mp -Minfo
RM       = rm

BIN =  main_omp main_acc

all: $(BIN)

main_acc: main.c cpu_3d_jacobi.c acc_3d_jacobi.c
	$(CC) $(CCFLAGS) $(ACCFLAGS) -o $@ $^

main_omp: main.c cpu_3d_jacobi.c acc_3d_jacobi.c
	$(CC) $(CCFLAGS) $(OMPFLAGS) -o $@ $^

main_cpu: csr_main1.c csr_cpu.c sparse_formats.c common_args.c
	$(CC) $(CCFLAGS) $(OMPFLAGS) -o $@ $^

clean:
	$(RM) $(BIN) *.o