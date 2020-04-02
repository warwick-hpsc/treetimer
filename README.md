#================================
# ==== TreeTimer ====
#================================

The TreeTimer Library is a custom library intended to capture data about instrumented blocks in the code. It tracks the relationship
between codeblocks based on how they are nested in one another akin to a callpath tree. In each block, as well as timing data, it can also
store 'local' parameters that are associated with the block in which they are stored, and 'global' parameters that can be associated with an entire
run.

#================
Usage
#================

Postprocessing:
The library itself must be initialised with TreeTimerInit() prior to any blocks, and finalized with TreeTimerFinalize() to write out its results.
The results are currently written out to an SQLite Database for postprocessing, such as by the example python script provided at scripts/treetimer-postprocessing.

The script can be used by running it with the path to a SQlite database file output by the instrumentation library as an argument.
A small selection of post-processing methods can be selected by uncommenting the appropriate method.

Instrumentation:
Timer Blocks are started with a entry method call associated with a "BlockName", beginning a timing/parameter data capture.
Timer Blocks are ended with a exit method call associated with a "BlockName", ending the timing of the current block entry and storing either a new trace entry or
adding to the aggregate data. Please note that if the exit blockname differs from the name of the current block the library thinks it is in, a warning will be
printed since the data will be invalid (i.e. the entry/exits should match in a stacking order if they are nested)

Different types of entry/exit methods are used to tracked different types of blocks - e.g. method calls, compute, MPI etc.
The selection can be found in tt_interface_c.h and is currently as follows:

TreeTimer[Enter|Exit]BlockMethod(<Name>) : Enter/Exit a Method Call (typically used at start and end of methods)
TreeTimer[Enter|Exit]BlockLoop(<Name>) : Enter/Exit an arbitrary loop
TreeTimer[Enter|Exit]BlockComputeLoop(<Name>) : Enter/Exit a compute loop
TreeTimer[Enter|Exit]BlockUnspecified(<Name>) : Enter/Exit a custom block not specifically associated with a type of behaviour
TreeTimer[Enter|Exit]BlockNonMPICall(<Name>) : Enter/Exit a Non-MPI method call (similiar to BlockMethod, but might be used for third-party library calls)
TreeTimer[Enter|Exit]BlockMPICommCall(<Name>) : Enter/Exit a MPI Call that uses communications
TreeTimer[Enter|Exit]BlockMPINOnCommCall(<Name>) : Enter/Exit a MPI call that does not use communications
TreeTimer[Enter|Exit]BlockIOCall(<Name>) : Enter/exit an I/O Block.

These use of these is effectively arbitrary - these methods do not do anything special to detect the types of behaviours. It is just the case that which method is used
determines what type of behaviour is associated with the timer block in the output data/post-processing.

Local Parameters can be tracked using the following methods:

TreeTimerLogParameterInt(<"name">, value) : Stores an integer value associated with the parameter "name" and the block the library is currently in (i.e. the last entered but not exited)
TreeTimerLogParameterDouble(<"name">, value) : Stores an double value associated with the parameter "name" and the block the library is currently in (i.e. the last entered but not exited)
TreeTimerLogParameterBool(<"name">, value) : Stores an bool value associated with the parameter "name" and the block the library is currently in (i.e. the last entered but not exited)
TreeTimerLogParameterString(<"name">, value) : Stores an string associated with the parameter "name" and the block the library is currently in (i.e. the last entered but not exited)

Global equivalents can be stored in a similar fashion, but they are not associated with any particular block.

TreeTimerLogGlobalParameterInt
TreeTimerLogGlobalParameterDouble
TreeTimerLogGlobalParameterBool
TreeTimerLogGlobalParameterString

To use these operations the file tt_interface_c.h should be included.
Please see the src code in the benchmarks folder for examples of its usage.
Currently a Fortran and C interface exists (CupCfd uses the C interface)

The data storage layout is too lengthy to detail here, but the schemas used can be found in treetimer/src/database/sqlite3/ folder.

Provision is made to store machine data in a location in the database, but this data is not currently captured and thus these tables in the database remain empty.

#================
RunTime Options
#================

The TreeTimer Library has a number of configuration options that can be set at runtime via the use of enviromental parameters:

TT_CONFIG_APPNAME : String. This environment variable contains the application name that will be associated with the results in the database.

Aggregate Data: This will store aggregated data to minimise data output.
TT_CONFIG_ATIMERS : [0|1] If set to 1, this enables aggregate timing data for a block. 0 disables this option. By default it is on if unset.
TT_CONFIG_APARAM : [0:1] If set to 1, this enables aggregate parameter data for a block. 0 disables this option. By default it is on if unset.
TT_CONFIG_APAPI : [0:1] If set to 1, this enables aggregate papi data for a block. 0 disables this option. This is a placeholder - PAPI is not yet included in this version.

Trace Data: This will store a unique data entry for every block entry/exit, enabling traces to be reconstructed.
TT_CONFIG_TTIMERS : [0|1] If set to 1, this enables trace timing data for a block. 0 disables this option. By default it is on if unset.
TT_CONFIG_TPARAM : [0:1] If set to 1, this enables trace parameter data for a block. 0 disables this option. By default it is on if unset.
TT_CONFIG_TPAPI : [0:1] If set to 1, this enables trace papi data for a block. 0 disables this option. This is a placeholder - PAPI is not yet included in this version.

TT_CONFIG_MPI_HOOKS: [0|1] If set to 1, this enables the wrapping of all MPI function calls (at least, those implemented in the library) such that MPI specific
data including times, destinations, byte sizes, buffer sizes etc will be captured (depending on the type of call). 
0 disables this option. This option is also influenced by the aggreate/trace configuration options - i.e. if trace is enabled then trace data should be stored for MPI calls.

This particular options requires the use of LD_PRELOAD and a shared treetimer library to be effective, since it utilises PMPI to catch MPI calls. It may work without LD_PRELOAD
or a static library, but is not guaranteed or recommended since it will likely depend upon linking orders.

#==============
#  Building
#==============

SQLITE_DIR must be set to point at the root directory for SQLite, this assumes that the include files are located at SQLITE_DIR/include/.
GNU or Intel compilers can be set by setting COMPILER=gnu or COMPILER=intel, respectively.
Once this is done, it can be built with make all or make lib etc.
