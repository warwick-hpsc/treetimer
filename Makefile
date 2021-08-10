
# ===== Third party =====

# Directories
SQLITE_INC=${SQLITE_DIR}/include

# Library Name
LIB_NAME = tt

# ===== Do not edit below this line =====

LIBDIR = -L
LIB = -l
INC = -I

# Compilers:
ifeq ($(COMPILER),gnu)
	CC  = gcc
	FC  = gfortran
	AR  = ar
	MPIFC  = mpif90
	MPICC  = mpicc
	MPICXX = mpic++
else ifeq ($(COMPILER),intel)
	CC  = icc
	FC  = ifort
	AR  = xiar
	MPIFC = mpiifort
	MPICC = mpiicc
	MPICXX = mpiicpc
else ifeq ($(COMPILER),clang)
    CC = clang
    FC = gfortran ## LLVM-Fortran far from ready (pre-Alpha)
    AR = ar
    MPIFC = mpif90 ## LLVM-Fortran far from ready (pre-Alpha)
    MPICC = mpicc -cc=clang
    MPICXX = mpicxx -cxx=clang++
endif

# Build options:
opt           = -O2
debug         = -g -O2 -DDEBUG

# Include Paths
TREETIMER_INC=./include

INCLUDE=${INC}${TREETIMER_INC} ${INC}${SQLITE_INC}

# Library Paths
SQLITE_LIB=${LIBDIR}${SQLITE_DIR}/lib -lsqlite3

# Library Links
LINK = ${SQLITE_LIB} -lm -lc

# Fortran flags:
FFLAGS        := $(debug)
FSTDLIBS      :=
F2C_NAMING    = F2C_UNDERSCORE
ifeq ($(COMPILER),intel)
	FFLAGS += -heap-arrays 64
	FSTDLIBS      = $(LIB)ifcore $(LIB)ifport $(LIB)irc
endif

# C flags:
CFLAGS        = $(debug) -std=gnu99
CDEF          = -D

# C++ flags:
CXXFLAGS      = $(debug)

LD = $(MPIFC)

CFLAGS += -D${F2C_NAMING}
CXXFLAGS += -D${F2C_NAMING}

# Enable interception of PMPI calls:
#CFLAGS += -DTT_PMPI
#CXXFLAGS += -DTT_PMPI
# TODO: auto-detect whether PMPI interception is necessary

OBJECTS = \
		  src/third-party/jsoncpp/jsoncpp.o \
		  src/data_structures/tree/tt_tree_node.o \
		  src/data_structures/tree/tt_tree.o \
		  src/data_structures/linked_list/tt_linkedlist_node.o \
		  src/data_structures/linked_list/tt_linkedlist.o \
		  src/database/sqlite3/tt_sqlite3_db_access.o \
		  src/database/sqlite3/tt_sqlite3_db_aggregate_parameters.o \
		  src/database/sqlite3/tt_sqlite3_db_aggregate_timers.o \
		  src/database/sqlite3/tt_sqlite3_db_app_config.o \
		  src/database/sqlite3/tt_sqlite3_db_app.o \
		  src/database/sqlite3/tt_sqlite3_db_callpath.o \
		  src/database/sqlite3/tt_sqlite3_db_compute_node.o \
		  src/database/sqlite3/tt_sqlite3_db_cpu_core.o \
		  src/database/sqlite3/tt_sqlite3_db_cpu_socket.o \
		  src/database/sqlite3/tt_sqlite3_db_cpu.o \
		  src/database/sqlite3/tt_sqlite3_db_lib_config.o \
		  src/database/sqlite3/tt_sqlite3_db_machine.o \
		  src/database/sqlite3/tt_sqlite3_db_parameters.o \
		  src/database/sqlite3/tt_sqlite3_db_process.o \
		  src/database/sqlite3/tt_sqlite3_db_profile_node.o \
		  src/database/sqlite3/tt_sqlite3_db_profile_run_config.o \
		  src/database/sqlite3/tt_sqlite3_db_profile_type.o \
		  src/database/sqlite3/tt_sqlite3_db_trace_parameters.o \
		  src/database/sqlite3/tt_sqlite3_db_trace_timers.o \
		  src/timing-library/measurement/timers/tt_systimers_linux.o \
		  src/timing-library/measurement/timers/tt_agg_timings.o \
		  src/timing-library/measurement/timers/tt_trace_timer.o \
		  src/timing-library/measurement/timers/tt_timer.o \
		  src/timing-library/measurement/parameters/tt_parameter_singular.o \
		  src/timing-library/measurement/parameters/tt_trace_parameter.o \
		  src/timing-library/measurement/parameters/tt_agg_parameter.o \
		  src/timing-library/measurement/parameters/tt_parameter.o \
		  src/timing-library/measurement/mpi/tt_mpi.o \
		  src/timing-library/measurement/mpi/tt_pmpi.o \
		  src/timing-library/measurement/tt_instrumentation_data.o \
		  src/timing-library/io/json/tt_io_json.o \
		  src/timing-library/io/sqlite3/tt_io_sqlite3.o \
		  src/timing-library/io/interface/tt_io_data.o \
		  src/timing-library/environment/tt_process_config.o \
		  src/timing-library/configuration/tt_config.o \
		  src/timing-library/configuration/tt_state.o \
		  src/timing-library/core/tt_core.o \
		  src/timing-library/interface/tt_interface_c.o \
		  src/timing-library/interface/tt_interface_f90.o \
		  src/timing-library/interface/tt_wrapper_f90.o

INCLUDE+= \
        ${INC} include/third-party/jsoncpp \
		${INC} include/data_structures/tree \
		${INC} include/data_structures/linked_list \
		${INC} include/database/sqlite3 \
		${INC} include/timing_library/measurement \
        ${INC} include/timing_library/measurement/parameters \
        ${INC} include/timing_library/measurement/timers \
        ${INC} include/timing_library/measurement/mpi \
        ${INC} include/timing_library/io/json \
        ${INC} include/timing_library/io/sqlite3 \
        ${INC} include/timing_library/io/interface \
        ${INC} include/timing_library/configuration \
        ${INC} include/timing_library/environment \
        ${INC} include/timing_library/core \
        ${INC} include/timing_library/interface

-include F2C_conf

all: lib-shared

lib: ${OBJECTS}
	${MPICC} -shared -o lib${LIB_NAME}.so ${OBJECTS} ${SQLITE_LIB}

lib-shared: ${OBJECTS}
	${MPICC} -shared -o lib${LIB_NAME}.so ${OBJECTS} ${SQLITE_LIB}

lib-static: ${OBJECTS}
	$(AR) rv lib${LIB_NAME}.a ${OBJECTS}

f90_module: src/timing-library/interface/tt_wrapper_f90.f90
	$(FC) -c $^ 

testing-c: lib-static
	${MPICC} -O2 -c examples/c_interface/main.c ${INCLUDE}
	${MPICC} main.o -o ./test_c_interface -L. -l${LIB_NAME} ${SQLITE_LIB} -lm -lc ${FSTDLIBS}

testing-f90: lib-static
	${MPIFC} -O2 -c examples/fortran_interface/main.f90 ${INCLUDE}
	${MPIFC} main.o -o ./test_fortran_interface -L. -l${LIB_NAME} ${SQLITE_LIB} -lm -lc ${FSTDLIBS} -lstdc++

clean:
	rm -f ${OBJECTS}
	rm -f src/*.o;
	rm -f src/*.mod;
	rm -f *.mod;
	rm -f *.o;
	rm -f *.so;
	rm -f test_fortran_interface
	rm -f test_c_interface
	rm -f libtt.a

%.o : %.f90
	${MPIFC} -fPIC ${INCLUDE} ${FFLAGS} -c $< -o $@

%.o : %.c
	${MPICC} -fPIC ${INCLUDE} ${CFLAGS} -c $< -o $@

%.o : %.cpp
	${MPICXX} -fPIC ${INCLUDE} ${CXXFLAGS} -c $< -o $@

F2C_conf:
	@ echo "subroutine test" > tmp.f90; \
	echo "end subroutine" >> tmp.f90; \
	$(MPIFC) -o tmp.o -c tmp.f90; \
	case `nm tmp.o | cut -d' ' -f3` in \
		test_) F2C_OPT=F2C_UNDERSCORE;; \
		test__) F2C_OPT=F2C_DOUBLEUNDERSCORE;; \
		TEST) F2C_OPT=F2C_CAPS;; \
		TEST_) F2C_OPT=F2C_CAPSUNDERSCORE;; \
		*) F2C_OPT=F2C_NOCHANGE;; \
	esac; \
	echo "F2C_OPT = $$F2C_OPT" > F2C_conf; \
	echo "Set F2C_OPT=$$F2C_OPT"; \
	rm tmp.f90 tmp.o;
