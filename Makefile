
# ===== Third party =====

# Directories
ifdef SQLITE_DIR
	SQLITE_INC=-I${SQLITE_DIR}/include
	SQLITE_LIB=-L${SQLITE_DIR}/lib
endif

# Library Name
LIB_NAME = tt

# ===== Do not edit below this line =====

# Compilers:
ifeq ($(COMPILER),gnu)
	CC  ?= gcc
	FC  ?= gfortran
	AR  ?= ar
	MPIFC  ?= mpif90
	MPICC  ?= mpicc
	MPICXX ?= mpic++
	c_warnings = -fmax-errors=1 -Wall -Werror -Wno-deprecated-declarations
	f_warnings = -fmax-errors=1 -Wall -Werror -Wno-deprecated-declarations -Wno-tabs
else ifeq ($(COMPILER),intel)
	CC  ?= icc
	FC  ?= ifort
	AR  ?= xiar
	MPIFC ?= mpiifort
	MPICC ?= mpiicc
	MPICXX ?= mpiicpc
else ifeq ($(COMPILER),clang)
	CC ?= clang
	FC ?= gfortran ## LLVM-Fortran far from ready (pre-Alpha)
	AR ?= ar
	MPIFC ?= mpif90 ## LLVM-Fortran far from ready (pre-Alpha)
	MPICC ?= mpicc -cc=clang
	MPICXX ?= mpicxx -cxx=clang++
else ifeq ($(COMPILER),cray)
	CC ?= cc
	FC ?= ftn
	AR ?= ar
	MPIFC ?= ftn
	MPICC ?= cc
	MPICXX ?= CC
endif

# Build options:
opt   = -O2
debug = -O2 -g -DDEBUG

# Include Paths
TREETIMER_INC=-I./include

INCLUDE=${TREETIMER_INC} ${SQLITE_INC}

# Library Paths
SQLITE_LIB += -lsqlite3

# Library Links
LINK = ${SQLITE_LIB} -lm -lc

# Fortran flags:
FFLAGS        := $(debug) $(f_warnings)
FSTDLIBS      :=
F2C_NAMING    = F2C_UNDERSCORE
ifeq ($(COMPILER),intel)
	FFLAGS += -heap-arrays 64
	FSTDLIBS      = $(LIB)ifcore $(LIB)ifport $(LIB)irc
endif

# C flags:
CFLAGS        = $(debug) -std=gnu99 $(c_warnings)
CDEF          = -D

# C++ flags:
CXXFLAGS      = $(debug) $(c_warnings)

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
        -Iinclude/third-party/jsoncpp \
		-Iinclude/data_structures/tree \
		-Iinclude/data_structures/linked_list \
		-Iinclude/database/sqlite3 \
		-Iinclude/timing_library/measurement \
        -Iinclude/timing_library/measurement/parameters \
        -Iinclude/timing_library/measurement/timers \
        -Iinclude/timing_library/measurement/mpi \
        -Iinclude/timing_library/io/json \
        -Iinclude/timing_library/io/sqlite3 \
        -Iinclude/timing_library/io/interface \
        -Iinclude/timing_library/configuration \
        -Iinclude/timing_library/environment \
        -Iinclude/timing_library/core \
        -Iinclude/timing_library/interface

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
