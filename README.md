TreeTimer
==========================================

The TreeTimer Library is a custom library intended to capture data about instrumented blocks in the code. It tracks the relationship between codeblocks based on how they are nested in one another akin to a callpath tree. In each block, as well as timing data, it can also store 'local' parameters that are associated with the block in which they are stored, and 'global' parameters that can be associated with an entire run.

Building
==========================================

SQLITE_DIR must be set to point at the root directory for SQLite, this assumes that the include files are located at SQLITE_DIR/include/.
GNU or Intel compilers can be set by setting COMPILER=gnu or COMPILER=intel, respectively.
Once this is done, it can be built with make all or make lib etc.

TreeTimer by default intecepts calls to the C-MPI API. Not all systems are configured like this and instead call the PMPI API (e.g. Cray MPI), so TreeTimer will miss MPI data. In this case enable `TT_PMPI` flag in Makefile.

Usage
==========================================

## Instrumentation:
The library itself must be initialised with TreeTimerInit() prior to any blocks, and finalized with TreeTimerFinalize() to write out its results.
Timer Blocks are started with a entry method call associated with a "BlockName", beginning a timing/parameter data capture.
Timer Blocks are ended with a exit method call associated with a "BlockName", ending the timing of the current block entry and storing either a new trace entry or adding to the aggregate data. Please note that if the exit blockname differs from the name of the current block the library thinks it is in, a warning will be printed since the data will be invalid (i.e. the entry/exits should match in a stacking order if they are nested)

Different types of entry/exit methods are used to tracked different types of blocks - e.g. method calls, compute, MPI etc.
The selection can be found in tt_interface_c.h and is currently as follows:

`TreeTimer[Enter|Exit]<Type>(<Name>)` where `<Type>` indicates Entry/Exit of a:
- `Method` : method call (typically used at start and end of methods)
- `Loop` : arbitrary loop
- `Compute` : compute loop
- `Block` : custom block not specifically associated with a type of behaviour
- `MPICommCall` : MPI communication call
- `MPISyncCall` : MPI synchronisation call
- `MPICollectiveCall` : MPI collective call
- `IOCall` : I/O call
- `LibraryCall` : call to a 'black-box' library function

Selecting the appropriate type for your codeblocks simply allows TreeTimer post-processing to colour-code different types in charts.
As MPI calls are automatically intercepted, you should not need to manually use the MPI types in your code.

Local Parameters can be tracked using the following methods:

`TreeTimerLogParameter<Type>(<name>, value)` where `<Type>` is one of: `Int, Double, Bool, String`. When invoked, the value is associated with the parameter "name" and the current codeblock (i.e. the last entered but not exited)

Global equivalents can be stored in a similar fashion, but they are not associated with any particular block:

`TreeTimerLogGlobalParameter<Type>`

To use these operations the file `tt_interface_c.h` should be included in your code. Examples are provided in the `examples` folder. Currently a Fortran and C interface exists ([CUP-CFD](github.com/warwick-hpsc/cup-cfd) uses the C interface)

Runtime Options
==========================================

The TreeTimer Library has a number of configuration options that can be set at runtime via the use of enviromental parameters:

#### Aggregate Data: This will store aggregated data to minimise data output.
- TT_CONFIG_ATIMERS : [0|1] Disable/enable aggregate timing data for a block. Default 1. 
- TT_CONFIG_APARAM : [0|1] Disable/enable aggregate parameter data for a block. Default 1.
- TT_CONFIG_APAPI : [0|1] Disable/enable aggregate papi data for a block. This is a placeholder - PAPI is not yet included in this version.

#### Trace Data: This will store a unique data entry for every block entry/exit, enabling traces to be reconstructed.
- TT_CONFIG_TTIMERS : [0|1] Disable/enable trace timing data for a block. Default 0.
- TT_CONFIG_TPARAM : [0|1] Disable/enable trace parameter data for a block. Default 0.
- TT_CONFIG_TPAPI : [0|1] Disable/enable trace papi data for a block. This is a placeholder - PAPI is not yet included in this version.

TT_CONFIG_MPI_HOOKS: [0|1] Disable/enable the wrapping of all MPI function calls (at least, those implemented in the library), *enabling* collection of MPI-specific times and data. Default 0. Whether time/data is actually collected also depends on above options, e.g. enable TT_CONFIG_ATIMERS for aggregate timing of MPI functions. This particular options requires the use of LD_PRELOAD and a shared treetimer library to be effective, since it utilises PMPI to catch MPI calls. It may work without LD_PRELOAD or a static library, but is not guaranteed or recommended since it will likely depend upon linking orders.

TT_CONFIG_MPI_HOOKS_TPARAM : [0|1] Special option for collecting MPI parameters while tracing - destinations, byte sizes, buffer sizes etc. Default 0. If enabled, the traces are enormous, so you probably don't want this unless debugging. Requires TT_CONFIG_TPARAM and TT_CONFIG_MPI_HOOKS to also be set.

Measuring performance dynamics over time: It can be useful to measure how performance data varies over a run. But collecting a full trace is likely prohibitively expensive. Here are two means for feasibly measuring dynamic load, intended for codes with one main loop:

#### Windowed aggregation: aggregate within a fixed-size 'time' window. At end, move to next window.
- TT_CONFIG_AGG_STEP_NODE : String. Node to use to determine when to move to next window, all children nodes synchronise with this.
- TT_CONFIG_AGG_STEP_INTERVAL : Integer. Number of specified node entries/exits per window.

#### Intermittent tracing: collect full traces of 1-in-N entries of a particlular node
- TT_CONFIG_TRACE_CONDUCTOR_NODE : String. Name of codeblock node to use to determine when to enable/disable tracing, all children nodes synchronise with this.
- TT_CONFIG_TRACE_INTERVAL : Integer. Number of specified node entries/exits to ignore between traces.

## Postprocessing:
A script is provided to process the collected data:
- printing call tree of a specific rank
- charting runtime breakdown of all ranks, coloured by codeblock type (compute / MPI / library). To reduce clutter, ranks with the same calltree are plotted in the same chart
  
Further functions are enabled if time-series data is collected, either with tracing or windowed-aggregation:
  
 - plot heatmap of MPI %, done automatically if data present. If only interested in a particular sub-tree of main solver loop, then collect trace data and specify head codeblock via `args.g`
 - plot heatmap of specified parameter value
  
TreeTimer outputs one database file per compute node. Most processing can be performed in parallel, enable with `args.m`. Intermediate processing data is written to disk for faster repeat processing.

For documentation on command-line arguments run:
  
```Shell
     $ python3 ./scripts/post-process.py --help
```
