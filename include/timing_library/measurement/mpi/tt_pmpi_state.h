/*
 * @file
 * @author Andrew Owenson
 * @version 0.1
 *
 * @section LICENSE
 * Based on PMTM/Ichniea - ToDo: Check License for these
 *
 * @section DESCRIPTION
 *
 */

// ====================================================================================
// Cache for function pointers. Assumes that dlsym() is expensive relative to a 
// single MPI call, so best to only call once.
// ====================================================================================

#ifdef TT_PMPI

namespace treetimer
{
    namespace core
    {
        namespace mpi
        {
            int (*lPMPI_Comm_size)(MPI_Comm, int*) = nullptr;
            int (*lPMPI_Comm_rank)(MPI_Comm, int*) = nullptr;

            int (*lPMPI_Send)(const void*, int, MPI_Datatype, int, int, MPI_Comm) = nullptr;
            int (*lPMPI_Isend)(const void*, int, MPI_Datatype, int, int, MPI_Comm, MPI_Request*) = nullptr;
            int (*lPMPI_Ssend)(const void*, int, MPI_Datatype, int, int, MPI_Comm) = nullptr;
            int (*lPMPI_Issend)(const void*, int, MPI_Datatype, int, int, MPI_Comm, MPI_Request*) = nullptr;

            int (*lPMPI_Recv)(void*, int, MPI_Datatype, int, int, MPI_Comm, MPI_Status*) = nullptr;
            int (*lPMPI_Irecv)(void*, int, MPI_Datatype, int, int, MPI_Comm, MPI_Request*) = nullptr;

            int (*lPMPI_Barrier)(MPI_Comm) = nullptr;
            int (*lPMPI_Wait)(MPI_Request*, MPI_Status*) = nullptr;
            int (*lPMPI_Waitall)(int, MPI_Request*, MPI_Status*) = nullptr;
            int (*lPMPI_Waitany)(int, MPI_Request*, int*, MPI_Status*) = nullptr;
            int (*lPMPI_Test)(MPI_Request*, int*, MPI_Status*) = nullptr;
            int (*lPMPI_Probe)(int, int, MPI_Comm, MPI_Status*) = nullptr;

            int (*lPMPI_Gather)(const void*, int, MPI_Datatype, void*, int, MPI_Datatype, int, MPI_Comm) = nullptr;
            int (*lPMPI_Gatherv)(const void*, int, MPI_Datatype, void*, const int*, const int*, MPI_Datatype, int, MPI_Comm) = nullptr;
            int (*lPMPI_Allgather)(const void*, int, MPI_Datatype, void*, int, MPI_Datatype, MPI_Comm) = nullptr;
            int (*lPMPI_Allgatherv)(const void*, int, MPI_Datatype, void*, const int*, const int*, MPI_Datatype, MPI_Comm) = nullptr;
            int (*lPMPI_Iallgather)(const void*, int, MPI_Datatype, void*, int, MPI_Datatype, MPI_Comm, MPI_Request*) = nullptr;
            
            int (*lPMPI_Alltoall)(const void*, int, MPI_Datatype, void*, int, MPI_Datatype, MPI_Comm) = nullptr;
            int (*lPMPI_Bcast)(void*, int, MPI_Datatype, int, MPI_Comm) = nullptr;
            int (*lPMPI_Reduce)(const void*, void*, int, MPI_Datatype, MPI_Op, int, MPI_Comm) = nullptr;
            int (*lPMPI_Allreduce)(const void*, void*, int, MPI_Datatype, MPI_Op, MPI_Comm) = nullptr;

            int (*lPMPI_Unpack)(const void*, int, int*, void*, int, MPI_Datatype, MPI_Comm) = nullptr;
            int (*lPMPI_Pack)(const void*, int, MPI_Datatype, void*, int, int*, MPI_Comm) = nullptr;
            int (*lPMPI_Bsend)(const void*, int, MPI_Datatype, int, int, MPI_Comm) = nullptr;
            int (*lPMPI_Buffer_attach)(void*, int) = nullptr;

            int (*lPMPI_Send_init)(const void*, int, MPI_Datatype, int, int, MPI_Comm, MPI_Request*) = nullptr;
            int (*lPMPI_Bsend_init)(const void*, int, MPI_Datatype, int, int, MPI_Comm, MPI_Request*) = nullptr;
            int (*lPMPI_Ssend_init)(const void*, int, MPI_Datatype, int, int, MPI_Comm, MPI_Request*) = nullptr;
            int (*lPMPI_Rsend_init)(const void*, int, MPI_Datatype, int, int, MPI_Comm, MPI_Request*) = nullptr;
            int (*lPMPI_Recv_init)(void*, int, MPI_Datatype, int, int, MPI_Comm, MPI_Request*) = nullptr;
            int (*lPMPI_Start)(MPI_Request*) = nullptr;
            int (*lPMPI_Startall)(int, MPI_Request*) = nullptr;

            int (*lPMPI_Put)(const void*, int, MPI_Datatype, int, MPI_Aint, int, MPI_Datatype, MPI_Win) = nullptr;
            int (*lPMPI_Win_create)(void*, MPI_Aint, int, MPI_Info, MPI_Comm, MPI_Win*) = nullptr;
            int (*lPMPI_Win_start)(MPI_Group, int, MPI_Win) = nullptr;
            int (*lPMPI_Win_fence)(int, MPI_Win) = nullptr;
            int (*lPMPI_Win_lock)(int, int, int, MPI_Win) = nullptr;
            int (*lPMPI_Win_unlock)(int, MPI_Win) = nullptr;
            int (*lPMPI_Win_post)(MPI_Group, int, MPI_Win) = nullptr;
            int (*lPMPI_Win_complete)(MPI_Win) = nullptr;
            int (*lPMPI_Win_free)(MPI_Win*) = nullptr;
            int (*lPMPI_Win_wait)(MPI_Win) = nullptr;
        }
    }
}

#endif
