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
// Intercept C PMPI calls for profiling. Why PMPI? Some MPI implementations have 
// their Fortran interface call C PMPI rather than C MPI (e.g. Cray). Requires a
// bit more logic than C MPI interception.
// This wraps any PMPI call defined here with a node block for timing etc.
// it is also suitable for storing parameter details if desired.
// ====================================================================================

#ifndef TT_PMPI_OVERRIDE_INCLUDE_H
#define TT_PMPI_OVERRIDE_INCLUDE_H

#ifdef TT_PMPI

#include "mpi.h"

#ifdef  __cplusplus
extern "C" {
#endif

int  MPI_Comm_size(MPI_Comm comm, int * size);
int PMPI_Comm_size(MPI_Comm comm, int * size);
int  MPI_Comm_rank(MPI_Comm comm, int * rank);
int PMPI_Comm_rank(MPI_Comm comm, int * rank);

int  MPI_Send(const void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm);
int PMPI_Send(const void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm);
int  MPI_Isend(const void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request * request);
int PMPI_Isend(const void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request * request);
int  MPI_Ssend(const void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm);
int PMPI_Ssend(const void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm);
int  MPI_Issend(const void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request);
int PMPI_Issend(const void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request);

int  MPI_Recv(void *buf, int count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm, MPI_Status *status);
int PMPI_Recv(void *buf, int count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm, MPI_Status *status);
int  MPI_Irecv(void * buf, int count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm, MPI_Request * request);
int PMPI_Irecv(void * buf, int count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm, MPI_Request * request);

int  MPI_Barrier(MPI_Comm comm);
int PMPI_Barrier(MPI_Comm comm);
int  MPI_Wait(MPI_Request *request, MPI_Status *status);
int PMPI_Wait(MPI_Request *request, MPI_Status *status);
int  MPI_Waitall(int count, MPI_Request array_of_requests[], MPI_Status array_of_statuses[]);
int PMPI_Waitall(int count, MPI_Request array_of_requests[], MPI_Status array_of_statuses[]);
int  MPI_Waitany(int count, MPI_Request array_of_requests[], int *indx, MPI_Status *status);
int PMPI_Waitany(int count, MPI_Request array_of_requests[], int *indx, MPI_Status *status);
int  MPI_Test(MPI_Request *request, int * flag, MPI_Status *status);
int PMPI_Test(MPI_Request *request, int * flag, MPI_Status *status);
int  MPI_Probe(int source, int tag, MPI_Comm comm, MPI_Status * status);
int PMPI_Probe(int source, int tag, MPI_Comm comm, MPI_Status * status);

int  MPI_Gather(const void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount,                MPI_Datatype recvtype, int root, MPI_Comm comm);
int PMPI_Gather(const void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount,                MPI_Datatype recvtype, int root, MPI_Comm comm);
int  MPI_Gatherv(const void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, const int *recvcounts, const int *displs, MPI_Datatype recvtype, int root, MPI_Comm comm);
int PMPI_Gatherv(const void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, const int *recvcounts, const int *displs, MPI_Datatype recvtype, int root, MPI_Comm comm);
int  MPI_Allgather(const void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, MPI_Comm comm);
int PMPI_Allgather(const void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, MPI_Comm comm);
int  MPI_Allgatherv(const void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, const int *recvcounts, const int *displs, MPI_Datatype recvtype, MPI_Comm comm);
int PMPI_Allgatherv(const void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, const int *recvcounts, const int *displs, MPI_Datatype recvtype, MPI_Comm comm);
int  MPI_Iallgather(const void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, MPI_Comm comm, MPI_Request * request);
int PMPI_Iallgather(const void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, MPI_Comm comm, MPI_Request * request);

int  MPI_Alltoall(const void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, MPI_Comm comm);
int PMPI_Alltoall(const void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, MPI_Comm comm);

int  MPI_Bcast(void *buffer, int count, MPI_Datatype datatype, int root, MPI_Comm comm);
int PMPI_Bcast(void *buffer, int count, MPI_Datatype datatype, int root, MPI_Comm comm);
int  MPI_Reduce(const void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, int root, MPI_Comm comm);
int PMPI_Reduce(const void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, int root, MPI_Comm comm);
int  MPI_Allreduce(const void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm);
int PMPI_Allreduce(const void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm);

int  MPI_Unpack(const void *inbuf, int insize, int *position, void *outbuf, int outcount, MPI_Datatype datatype, MPI_Comm comm);
int PMPI_Unpack(const void *inbuf, int insize, int *position, void *outbuf, int outcount, MPI_Datatype datatype, MPI_Comm comm);
int  MPI_Pack(const void *inbuf, int incount, MPI_Datatype datatype, void* outbuf, int outsize, int * position, MPI_Comm comm);
int PMPI_Pack(const void *inbuf, int incount, MPI_Datatype datatype, void* outbuf, int outsize, int * position, MPI_Comm comm);
int  MPI_Bsend(const void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm);
int PMPI_Bsend(const void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm);
int  MPI_Buffer_attach(void *buffer, int size);
int PMPI_Buffer_attach(void *buffer, int size);

int  MPI_Send_init(const void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request);
int PMPI_Send_init(const void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request);
int  MPI_Bsend_init(const void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request);
int PMPI_Bsend_init(const void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request);
int  MPI_Ssend_init(const void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request);
int PMPI_Ssend_init(const void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request);
int  MPI_Rsend_init(const void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request);
int PMPI_Rsend_init(const void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request);
int  MPI_Recv_init(void *buf, int count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm, MPI_Request *request);
int PMPI_Recv_init(void *buf, int count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm, MPI_Request *request);
int  MPI_Start(MPI_Request * request);
int PMPI_Start(MPI_Request * request);
int  MPI_Startall(int count, MPI_Request * array_of_requests);
int PMPI_Startall(int count, MPI_Request * array_of_requests);

int  MPI_Put(const void *origin_addr, int origin_count, MPI_Datatype origin_datatype, int target_rank, MPI_Aint target_disp, int target_count, MPI_Datatype target_datatype, MPI_Win win);
int PMPI_Put(const void *origin_addr, int origin_count, MPI_Datatype origin_datatype, int target_rank, MPI_Aint target_disp, int target_count, MPI_Datatype target_datatype, MPI_Win win);
int  MPI_Win_create(void *base, MPI_Aint size, int disp_unit, MPI_Info info, MPI_Comm comm, MPI_Win *win);
int PMPI_Win_create(void *base, MPI_Aint size, int disp_unit, MPI_Info info, MPI_Comm comm, MPI_Win *win);
int  MPI_Win_start(MPI_Group group, int assert, MPI_Win win);
int PMPI_Win_start(MPI_Group group, int assert, MPI_Win win);
int  MPI_Win_fence(int assert, MPI_Win win);
int PMPI_Win_fence(int assert, MPI_Win win);
int  MPI_Win_lock(int lock_type, int rank, int assert, MPI_Win win);
int PMPI_Win_lock(int lock_type, int rank, int assert, MPI_Win win);
int  MPI_Win_unlock(int rank, MPI_Win win);
int PMPI_Win_unlock(int rank, MPI_Win win);
int  MPI_Win_post(MPI_Group group, int assert, MPI_Win win);
int PMPI_Win_post(MPI_Group group, int assert, MPI_Win win);
int  MPI_Win_complete(MPI_Win win);
int PMPI_Win_complete(MPI_Win win);
int  MPI_Win_free(MPI_Win *win);
int PMPI_Win_free(MPI_Win *win);
int  MPI_Win_wait(MPI_Win win);
int PMPI_Win_wait(MPI_Win win);

#ifdef  __cplusplus
}
#endif

#endif

#endif
