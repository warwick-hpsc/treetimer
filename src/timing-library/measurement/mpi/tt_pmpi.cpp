/*
 * @file
 * @author James Davis
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

// ToD: create utility to detect whether PMPI interception is required.

// ToDo: MPI_Get

#ifdef TT_PMPI

#include "tt_pmpi.h"
#include "tt_pmpi_state.h"
#include "tt_core.h"
#include "tt_code_block_type.h"
#include "tt_global.h"

#include <string>
#include <iostream>
#include <dlfcn.h> // For dlsym()

namespace tt_c = treetimer::core;
namespace tt_d = treetimer::core::drivers;
namespace tt_m = treetimer::core::mpi;

#define INSTR_MPI_CONDITION (tt_c::libInit && tt_c::instrumState->config->eMPIHooks && (! tt_c::instrumState->config->inLibrary) && (!tt_c::instrumState->sleeping))

int MPI_Comm_size(MPI_Comm comm, int * size)
{
	return PMPI_Comm_size(comm, size);
}
int PMPI_Comm_size(MPI_Comm comm, int * size)
{
	if (tt_m::lPMPI_Comm_size == nullptr) {
		tt_m::lPMPI_Comm_size = (int(*)(MPI_Comm, int*))dlsym(RTLD_NEXT, "PMPI_Comm_size");
	}

	if(INSTR_MPI_CONDITION)
	{
		// Code Block
		tt_d::TreeTimerEnterBlock("PMPI_Comm_size",TT_NODE_TYPE_MPI_COMM_CALL);

		// Parameters
		char commGroupName[MPI_MAX_OBJECT_NAME+1];
		int resultlen;
		MPI_Comm_get_name(comm, commGroupName, &resultlen);
		tt_d::TreeTimerLogParameter("CommGroupName", std::string(commGroupName));
	}

	int r = tt_m::lPMPI_Comm_size(comm, size);

	if(INSTR_MPI_CONDITION)
	{
		tt_d::TreeTimerExit("PMPI_Comm_size");
	}

	return r;
}

int MPI_Comm_rank(MPI_Comm comm, int * rank)
{
	return PMPI_Comm_rank(comm, rank);
}
int PMPI_Comm_rank(MPI_Comm comm, int * rank)
{
	if (tt_m::lPMPI_Comm_rank == nullptr) {
		tt_m::lPMPI_Comm_rank = (int(*)(MPI_Comm, int*))dlsym(RTLD_NEXT, "PMPI_Comm_rank");
	}

	if(INSTR_MPI_CONDITION)
	{
		// Code Block
		tt_d::TreeTimerEnterBlock("PMPI_Comm_rank",TT_NODE_TYPE_MPI_COMM_CALL);

		// Parameters
		char commGroupName[MPI_MAX_OBJECT_NAME+1];
		int resultlen;
		MPI_Comm_get_name(comm, commGroupName, &resultlen);
		tt_d::TreeTimerLogParameter("CommGroupName", std::string(commGroupName));
	}

	int r = tt_m::lPMPI_Comm_rank(comm, rank);

	if(INSTR_MPI_CONDITION)
	{
		tt_d::TreeTimerExit("PMPI_Comm_rank");
	}

	return r;
}

int MPI_Send(const void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm)
{
	return PMPI_Send(buf, count, datatype, dest, tag, comm);
}
int PMPI_Send(const void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm)
{
	if (tt_m::lPMPI_Send == nullptr) {
		tt_m::lPMPI_Send = (int(*)(const void*, int, MPI_Datatype, int, int, MPI_Comm))dlsym(RTLD_NEXT, "PMPI_Send");
	}

	if(INSTR_MPI_CONDITION)
	{
		// Code Block
		tt_d::TreeTimerEnterBlock("PMPI_Send",TT_NODE_TYPE_MPI_COMM_CALL);

		// Parameters
		int typeByteSize;
		MPI_Type_size(datatype, &typeByteSize);
		tt_d::TreeTimerLogParameter("MPITypeBytes", typeByteSize);
		tt_d::TreeTimerLogParameter("Count", count);
		tt_d::TreeTimerLogParameter("DestProcess", dest);

		char commGroupName[MPI_MAX_OBJECT_NAME+1];
		int resultlen;
		MPI_Comm_get_name(comm, commGroupName, &resultlen);
		tt_d::TreeTimerLogParameter("CommGroupName", std::string(commGroupName));
	}

	int r = tt_m::lPMPI_Send(buf, count, datatype, dest, tag, comm);

	if(INSTR_MPI_CONDITION)
	{
		tt_d::TreeTimerExit("PMPI_Send");
	}

	return r;
}

int MPI_Isend(const void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request * request)
{
	return PMPI_Isend(buf, count, datatype, dest, tag, comm, request);
}
int PMPI_Isend(const void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request * request)
{
	if (tt_m::lPMPI_Isend == nullptr) {
		tt_m::lPMPI_Isend = (int(*)(const void*, int, MPI_Datatype, int, int, MPI_Comm, MPI_Request*))dlsym(RTLD_NEXT, "PMPI_Isend");
	}

	if(INSTR_MPI_CONDITION)
	{
		// Timer
		tt_d::TreeTimerEnterBlock("PMPI_Isend",TT_NODE_TYPE_MPI_COMM_CALL);

		// Parameters
		int typeByteSize;
		MPI_Type_size(datatype, &typeByteSize);
		tt_d::TreeTimerLogParameter("MPITypeBytes", typeByteSize);
		tt_d::TreeTimerLogParameter("Count", count);
		tt_d::TreeTimerLogParameter("DestProcess", dest);

		char commGroupName[MPI_MAX_OBJECT_NAME+1];
		int resultlen;
		MPI_Comm_get_name(comm, commGroupName, &resultlen);
		tt_d::TreeTimerLogParameter("CommGroupName", std::string(commGroupName));
	}

	int r = tt_m::lPMPI_Isend(buf, count, datatype, dest, tag, comm, request);

	if(INSTR_MPI_CONDITION)
	{
		tt_d::TreeTimerExit("PMPI_Isend");
	}

	return r;

}

int MPI_Ssend(const void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm)
{
	return PMPI_Ssend(buf, count, datatype, dest, tag, comm);
}
int PMPI_Ssend(const void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm)
{
	if (tt_m::lPMPI_Ssend == nullptr) {
		tt_m::lPMPI_Ssend = (int(*)(const void*, int, MPI_Datatype, int, int, MPI_Comm))dlsym(RTLD_NEXT, "PMPI_Ssend");
	}

	if(INSTR_MPI_CONDITION)
	{
		// Code Block
		tt_d::TreeTimerEnterBlock("PMPI_Ssend",TT_NODE_TYPE_MPI_COMM_CALL);

		// Parameters
		char commGroupName[MPI_MAX_OBJECT_NAME+1];
		int resultlen;
		MPI_Comm_get_name(comm, commGroupName, &resultlen);
		tt_d::TreeTimerLogParameter("CommGroupName", std::string(commGroupName));

		tt_d::TreeTimerLogParameter("DestProcess", dest);

		int typeByteSize;
		MPI_Type_size(datatype, &typeByteSize);
		tt_d::TreeTimerLogParameter("MPITypeBytes", typeByteSize);
		tt_d::TreeTimerLogParameter("Count", count);
	}

	int r = tt_m::lPMPI_Ssend(buf, count, datatype, dest, tag, comm);

	if(INSTR_MPI_CONDITION)
	{
		tt_d::TreeTimerExit("PMPI_Ssend");
	}

	return r;
}

int MPI_Issend(const void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request)
{
	return PMPI_Issend(buf, count, datatype, dest, tag, comm, request);
}
int PMPI_Issend(const void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request)
{
	if (tt_m::lPMPI_Issend == nullptr) {
		tt_m::lPMPI_Issend = (int(*)(const void*, int, MPI_Datatype, int, int, MPI_Comm, MPI_Request*))dlsym(RTLD_NEXT, "PMPI_Issend");
	}

	if(INSTR_MPI_CONDITION)
	{
		// Code Block
		tt_d::TreeTimerEnterBlock("PMPI_Issend",TT_NODE_TYPE_MPI_COMM_CALL);

		// Parameters
		char commGroupName[MPI_MAX_OBJECT_NAME+1];
		int resultlen;
		MPI_Comm_get_name(comm, commGroupName, &resultlen);
		tt_d::TreeTimerLogParameter("CommGroupName", std::string(commGroupName));

		tt_d::TreeTimerLogParameter("DestProcess", dest);

		int typeByteSize;
		MPI_Type_size(datatype, &typeByteSize);
		tt_d::TreeTimerLogParameter("MPITypeBytes", typeByteSize);
		tt_d::TreeTimerLogParameter("Count", count);
	}

	int r = tt_m::lPMPI_Issend(buf, count, datatype, dest, tag, comm, request);

	if(INSTR_MPI_CONDITION)
	{
		tt_d::TreeTimerExit("PMPI_Issend");
	}

	return r;
}

int MPI_Recv(void *buf, int count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm, MPI_Status *status)
{
	return PMPI_Recv(buf, count, datatype, source, tag, comm, status);
}
int PMPI_Recv(void *buf, int count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm, MPI_Status *status)
{
	if (tt_m::lPMPI_Recv == nullptr) {
		tt_m::lPMPI_Recv = (int(*)(void*, int, MPI_Datatype, int, int, MPI_Comm, MPI_Status*))dlsym(RTLD_NEXT, "PMPI_Recv");
	}

	if(INSTR_MPI_CONDITION)
	{
		// Code Block
		tt_d::TreeTimerEnterBlock("PMPI_Recv",TT_NODE_TYPE_MPI_COMM_CALL);

		// Parameters
		int typeByteSize;
		MPI_Type_size(datatype, &typeByteSize);
		tt_d::TreeTimerLogParameter("MPITypeBytes", typeByteSize);
		tt_d::TreeTimerLogParameter("Count", count);
		tt_d::TreeTimerLogParameter("SourceProcess", source);

		char commGroupName[MPI_MAX_OBJECT_NAME+1];
		int resultlen;
		MPI_Comm_get_name(comm, commGroupName, &resultlen);
		tt_d::TreeTimerLogParameter("CommGroupName", std::string(commGroupName));
	}

	int r = tt_m::lPMPI_Recv(buf, count, datatype, source, tag, comm, status);

	if(INSTR_MPI_CONDITION)
	{
		tt_d::TreeTimerExit("PMPI_Recv");
	}

	return r;
}

int MPI_Irecv(void * buf, int count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm, MPI_Request * request)
{
	return PMPI_Irecv(buf, count, datatype, source, tag, comm, request);
}
int PMPI_Irecv(void * buf, int count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm, MPI_Request * request)
{
	if (tt_m::lPMPI_Irecv == nullptr) {
		tt_m::lPMPI_Irecv = (int(*)(void*, int, MPI_Datatype, int, int, MPI_Comm, MPI_Request*))dlsym(RTLD_NEXT, "PMPI_Irecv");
	}

	if(INSTR_MPI_CONDITION)
	{
		// Code Block
		tt_d::TreeTimerEnterBlock("PMPI_Irecv",TT_NODE_TYPE_MPI_COMM_CALL);

		// Parameters
		int typeByteSize;
		MPI_Type_size(datatype, &typeByteSize);
		tt_d::TreeTimerLogParameter("MPITypeBytes", typeByteSize);
		tt_d::TreeTimerLogParameter("Count", count);
		tt_d::TreeTimerLogParameter("SourceProcess", source);

		char commGroupName[MPI_MAX_OBJECT_NAME+1];
		int resultlen;
		MPI_Comm_get_name(comm, commGroupName, &resultlen);
		tt_d::TreeTimerLogParameter("CommGroupName", std::string(commGroupName));
	}

	int r = tt_m::lPMPI_Irecv(buf, count, datatype, source, tag, comm, request);

	if(INSTR_MPI_CONDITION)
	{
		tt_d::TreeTimerExit("PMPI_Irecv");
	}

	return r;
}

int MPI_Barrier(MPI_Comm comm)
{
	return PMPI_Barrier(comm);
}
int PMPI_Barrier(MPI_Comm comm)
{
	if (tt_m::lPMPI_Barrier == nullptr) {
		tt_m::lPMPI_Barrier = (int(*)(MPI_Comm))dlsym(RTLD_NEXT, "PMPI_Barrier");
	}

	if(INSTR_MPI_CONDITION)
	{
		// Timer
		tt_d::TreeTimerEnterBlock("PMPI_Barrier",TT_NODE_TYPE_MPI_COMM_CALL);

		// Parameters
		char commGroupName[MPI_MAX_OBJECT_NAME+1];
		int resultlen;
		MPI_Comm_get_name(comm, commGroupName, &resultlen);
		tt_d::TreeTimerLogParameter("CommGroupName", std::string(commGroupName));
	}

	int r = tt_m::lPMPI_Barrier(comm);

	if(INSTR_MPI_CONDITION)
	{
		tt_d::TreeTimerExit("PMPI_Barrier");
	}

	return r;
}

int MPI_Wait(MPI_Request *request, MPI_Status *status)
{
	return PMPI_Wait(request, status);
}
int PMPI_Wait(MPI_Request *request, MPI_Status *status)
{
	if (tt_m::lPMPI_Wait == nullptr) {
		tt_m::lPMPI_Wait = (int(*)(MPI_Request*, MPI_Status*))dlsym(RTLD_NEXT, "PMPI_Wait");
	}

	if(INSTR_MPI_CONDITION)
	{
		// Code Block
		tt_d::TreeTimerEnterBlock("PMPI_Wait",TT_NODE_TYPE_MPI_COMM_CALL);
	}

	int r = tt_m::lPMPI_Wait(request, status);

	if(INSTR_MPI_CONDITION)
	{
		tt_d::TreeTimerExit("PMPI_Wait");
	}

	return r;
}

int MPI_Waitall(int count, MPI_Request array_of_requests[], MPI_Status array_of_statuses[])
{
	return PMPI_Waitall(count, array_of_requests, array_of_statuses);
}
int PMPI_Waitall(int count, MPI_Request array_of_requests[], MPI_Status array_of_statuses[])
{
	if (tt_m::lPMPI_Waitall == nullptr) {
		tt_m::lPMPI_Waitall = (int(*)(int, MPI_Request*, MPI_Status*))dlsym(RTLD_NEXT, "MPI_Waitall");
	}

	if(INSTR_MPI_CONDITION)
	{
		tt_d::TreeTimerEnterBlock("PMPI_Waitall",TT_NODE_TYPE_MPI_COMM_CALL);
	}

	int r = tt_m::lPMPI_Waitall(count, array_of_requests, array_of_statuses);

	if(INSTR_MPI_CONDITION)
	{
		tt_d::TreeTimerExit("PMPI_Waitall");
	}

	return r;
}

int MPI_Waitany(int count, MPI_Request array_of_requests[], int *indx, MPI_Status *status)
{
	return PMPI_Waitany(count, array_of_requests, indx, status);
}
int PMPI_Waitany(int count, MPI_Request array_of_requests[], int *indx, MPI_Status *status)
{
	if (tt_m::lPMPI_Waitany == nullptr) {
		tt_m::lPMPI_Waitany = (int(*)(int, MPI_Request*, int*, MPI_Status*))dlsym(RTLD_NEXT, "MPI_Waitany");
	}

	if(INSTR_MPI_CONDITION)
	{
		tt_d::TreeTimerEnterBlock("PMPI_Waitany",TT_NODE_TYPE_MPI_COMM_CALL);
	}

	int r = tt_m::lPMPI_Waitany(count, array_of_requests, indx, status);

	if(INSTR_MPI_CONDITION)
	{
		tt_d::TreeTimerExit("PMPI_Waitany");
	}

	return r;
}

int MPI_Test(MPI_Request *request, int * flag, MPI_Status *status)
{
	return PMPI_Test(request, flag, status);
}
int PMPI_Test(MPI_Request *request, int * flag, MPI_Status *status)
{
	if (tt_m::lPMPI_Test == nullptr) {
		tt_m::lPMPI_Test = (int(*)(MPI_Request*, int*, MPI_Status*))dlsym(RTLD_NEXT, "MPI_Test");
	}

	if(INSTR_MPI_CONDITION)
	{
		// Code Block
		tt_d::TreeTimerEnterBlock("PMPI_Test",TT_NODE_TYPE_MPI_COMM_CALL);
	}

	int r = tt_m::lPMPI_Test(request, flag, status);

	if(INSTR_MPI_CONDITION)
	{
		tt_d::TreeTimerExit("PMPI_Test");
	}

	return r;
}

int MPI_Probe(int source, int tag, MPI_Comm comm, MPI_Status * status)
{
	return PMPI_Probe(source, tag, comm, status);
}
int PMPI_Probe(int source, int tag, MPI_Comm comm, MPI_Status * status)
{
	if (tt_m::lPMPI_Probe == nullptr) {
		tt_m::lPMPI_Probe = (int(*)(int, int, MPI_Comm, MPI_Status*))dlsym(RTLD_NEXT, "MPI_Probe");
	}

	if(INSTR_MPI_CONDITION)
	{
		// Code Block
		tt_d::TreeTimerEnterBlock("PMPI_Probe",TT_NODE_TYPE_MPI_COMM_CALL);
	}

	int r = tt_m::lPMPI_Probe(source, tag, comm, status);

	if(INSTR_MPI_CONDITION)
	{
		tt_d::TreeTimerExit("PMPI_Probe");
	}

	return r;
}

int MPI_Gather(const void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm)
{
	return PMPI_Gather(sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, root, comm);
}
int PMPI_Gather(const void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm)
{
	if (tt_m::lPMPI_Gather == nullptr) {
		tt_m::lPMPI_Gather = (int(*)(const void*, int, MPI_Datatype, void*, int, MPI_Datatype, int, MPI_Comm))dlsym(RTLD_NEXT, "MPI_Gather");
	}

	if(INSTR_MPI_CONDITION)
	{
		// Code Block
		tt_d::TreeTimerEnterBlock("PMPI_Gather",TT_NODE_TYPE_MPI_COMM_CALL);

		// Parameters
		char commGroupName[MPI_MAX_OBJECT_NAME+1];
		int resultlen;
		MPI_Comm_get_name(comm, commGroupName, &resultlen);
		tt_d::TreeTimerLogParameter("CommGroupName", std::string(commGroupName));

		tt_d::TreeTimerLogParameter("RootProcess", root);

		int typeByteSize;

		MPI_Type_size(sendtype, &typeByteSize);
		tt_d::TreeTimerLogParameter("SendMPITypeBytes", typeByteSize);
		tt_d::TreeTimerLogParameter("SendCount", sendcount);

		MPI_Type_size(recvtype, &typeByteSize);
		tt_d::TreeTimerLogParameter("RecvMPITypeBytes", typeByteSize);
		tt_d::TreeTimerLogParameter("RecvCount", recvcount);
	}

	int r = tt_m::lPMPI_Gather(sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, root, comm);

	if(INSTR_MPI_CONDITION)
	{
		tt_d::TreeTimerExit("PMPI_Gather");
	}

	return r;

}

int MPI_Gatherv(const void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, const int *recvcounts, const int *displs, MPI_Datatype recvtype, int root, MPI_Comm comm)
{
	return PMPI_Gatherv(sendbuf, sendcount, sendtype, recvbuf, recvcounts, displs, recvtype, root, comm);
}
int PMPI_Gatherv(const void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, const int *recvcounts, const int *displs, MPI_Datatype recvtype, int root, MPI_Comm comm)
{
	if (tt_m::lPMPI_Gatherv == nullptr) {
		tt_m::lPMPI_Gatherv = (int(*)(const void*, int, MPI_Datatype, void*, const int*, const int*, MPI_Datatype, int, MPI_Comm))dlsym(RTLD_NEXT, "MPI_Gatherv");
	}

	if(INSTR_MPI_CONDITION)
	{
		// Code Block
		tt_d::TreeTimerEnterBlock("PMPI_Gatherv",TT_NODE_TYPE_MPI_COMM_CALL);

		// Parameters
		char commGroupName[MPI_MAX_OBJECT_NAME+1];
		int resultlen;
		MPI_Comm_get_name(comm, commGroupName, &resultlen);
		tt_d::TreeTimerLogParameter("CommGroupName", std::string(commGroupName));

		tt_d::TreeTimerLogParameter("RootProcess", root);

		int typeByteSize;
		MPI_Type_size(sendtype, &typeByteSize);
		tt_d::TreeTimerLogParameter("SendMPITypeBytes", typeByteSize);
		tt_d::TreeTimerLogParameter("SendCount", sendcount);

		// ToDo: Tracking number of received bytes - same issue as allgatherv
		// We have send amounts and root, so we could infer it in post-processing
	}

	int r = tt_m::lPMPI_Gatherv(sendbuf, sendcount, sendtype, recvbuf, recvcounts, displs, recvtype, root, comm);

	if(INSTR_MPI_CONDITION)
	{
		tt_d::TreeTimerExit("PMPI_Gatherv");
	}

	return r;
}

int MPI_Allgather(const void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, MPI_Comm comm)
{
	return PMPI_Allgather(sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, comm);
}
int PMPI_Allgather(const void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, MPI_Comm comm)
{
	if (tt_m::lPMPI_Allgather == nullptr) {
		tt_m::lPMPI_Allgather = (int(*)(const void*, int, MPI_Datatype, void*, int, MPI_Datatype, MPI_Comm))dlsym(RTLD_NEXT, "MPI_Allgather");
	}

	if(INSTR_MPI_CONDITION)
	{
		// Code Block
		tt_d::TreeTimerEnterBlock("PMPI_Allgather",TT_NODE_TYPE_MPI_COMM_CALL);

		// Parameters
		int typeByteSize;

		char commGroupName[MPI_MAX_OBJECT_NAME+1];
		int resultlen;
		MPI_Comm_get_name(comm, commGroupName, &resultlen);
		tt_d::TreeTimerLogParameter("CommGroupName", std::string(commGroupName));

		MPI_Type_size(sendtype, &typeByteSize);
		tt_d::TreeTimerLogParameter("SendMPITypeBytes", typeByteSize);
		tt_d::TreeTimerLogParameter("SendCount", sendcount);

		MPI_Type_size(recvtype, &typeByteSize);
		tt_d::TreeTimerLogParameter("RecvMPITypeBytes", typeByteSize);
		tt_d::TreeTimerLogParameter("RecvCount", recvcount);
	}

	int r = tt_m::lPMPI_Allgather(sendbuf, sendcount, sendtype,
						   recvbuf, recvcount, recvtype, comm);

	if(INSTR_MPI_CONDITION)
	{
		tt_d::TreeTimerExit("PMPI_Allgather");
	}

	return r;
}

int MPI_Allgatherv(const void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, const int *recvcounts, const int *displs, MPI_Datatype recvtype, MPI_Comm comm)
{
	return PMPI_Allgatherv(sendbuf, sendcount, sendtype, recvbuf, recvcounts, displs, recvtype, comm);
}
int PMPI_Allgatherv(const void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, const int *recvcounts, const int *displs, MPI_Datatype recvtype, MPI_Comm comm)
{
	if (tt_m::lPMPI_Allgatherv == nullptr) {
		tt_m::lPMPI_Allgatherv = (int(*)(const void*, int, MPI_Datatype, void*, const int*, const int*, MPI_Datatype, MPI_Comm))dlsym(RTLD_NEXT, "MPI_Allgatherv");
	}

	if(INSTR_MPI_CONDITION)
	{
		// Code Block
		tt_d::TreeTimerEnterBlock("PMPI_Allgatherv",TT_NODE_TYPE_MPI_COMM_CALL);

		// Parameters
		char commGroupName[MPI_MAX_OBJECT_NAME+1];
		int resultlen;
		MPI_Comm_get_name(comm, commGroupName, &resultlen);
		tt_d::TreeTimerLogParameter("CommGroupName", std::string(commGroupName));

		int typeByteSize;

		MPI_Type_size(sendtype, &typeByteSize);
		tt_d::TreeTimerLogParameter("SendMPITypeBytes", typeByteSize);
		tt_d::TreeTimerLogParameter("SendCount", sendcount);

		// ToDo: Consider summing the number of elements to be received (recvcounts) - could we do this in post-processing
		// from send counts?
		// Don't really want to do here since the overhead would scale with process count.
	}

	int r = tt_m::lPMPI_Allgatherv(sendbuf, sendcount, sendtype, recvbuf, recvcounts, displs, recvtype, comm);

	if(INSTR_MPI_CONDITION)
	{
		tt_d::TreeTimerExit("PMPI_Allgatherv");
	}

	return r;
}

int MPI_Iallgather(const void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, MPI_Comm comm, MPI_Request * request)
{
	return PMPI_Iallgather(sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, comm, request);
}
int PMPI_Iallgather(const void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, MPI_Comm comm, MPI_Request * request)
{
	if (tt_m::lPMPI_Iallgather == nullptr) {
		tt_m::lPMPI_Iallgather = (int(*)(const void*, int, MPI_Datatype, void*, int, MPI_Datatype, MPI_Comm, MPI_Request*))dlsym(RTLD_NEXT, "MPI_Iallgather");
	}

	if(INSTR_MPI_CONDITION)
	{
		// Code Block
		tt_d::TreeTimerEnterBlock("PMPI_Iallgather",TT_NODE_TYPE_MPI_COMM_CALL);

		// Parameters
		char commGroupName[MPI_MAX_OBJECT_NAME+1];
		int resultlen;
		MPI_Comm_get_name(comm, commGroupName, &resultlen);
		tt_d::TreeTimerLogParameter("CommGroupName", std::string(commGroupName));

		int typeByteSize;

		MPI_Type_size(sendtype, &typeByteSize);
		tt_d::TreeTimerLogParameter("SendMPITypeBytes", typeByteSize);
		tt_d::TreeTimerLogParameter("SendCount", sendcount);

		MPI_Type_size(recvtype, &typeByteSize);
		tt_d::TreeTimerLogParameter("RecvMPITypeBytes", typeByteSize);
		tt_d::TreeTimerLogParameter("RecvCount", recvcount);
	}

	int r = tt_m::lPMPI_Iallgather(sendbuf, sendcount, sendtype,
						   recvbuf, recvcount, recvtype, comm, request);

	if(INSTR_MPI_CONDITION)
	{
		tt_d::TreeTimerExit("PMPI_Iallgather");
	}

	return r;
}

int MPI_Alltoall(const void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, MPI_Comm comm)
{
	return PMPI_Alltoall(sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, comm);
}
int PMPI_Alltoall(const void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, MPI_Comm comm)
{
	if (tt_m::lPMPI_Alltoall == nullptr) {
		tt_m::lPMPI_Alltoall = (int(*)(const void*, int, MPI_Datatype, void*, int, MPI_Datatype, MPI_Comm))dlsym(RTLD_NEXT, "MPI_Alltoall");
	}

	if(INSTR_MPI_CONDITION)
	{
		// Code Block
		tt_d::TreeTimerEnterBlock("PMPI_Alltoall",TT_NODE_TYPE_MPI_COMM_CALL);

		// Parameters
		char commGroupName[MPI_MAX_OBJECT_NAME+1];
		int resultlen;
		MPI_Comm_get_name(comm, commGroupName, &resultlen);
		tt_d::TreeTimerLogParameter("CommGroupName", std::string(commGroupName));

		int typeByteSize;

		MPI_Type_size(sendtype, &typeByteSize);
		tt_d::TreeTimerLogParameter("SendMPITypeBytes", typeByteSize);
		tt_d::TreeTimerLogParameter("SendCount", sendcount);

		MPI_Type_size(recvtype, &typeByteSize);
		tt_d::TreeTimerLogParameter("RecvMPITypeBytes", typeByteSize);
		tt_d::TreeTimerLogParameter("RecvCount", recvcount);
	}

	int r = tt_m::lPMPI_Alltoall(sendbuf, sendcount, sendtype,
						   recvbuf, recvcount, recvtype, comm);

	if(INSTR_MPI_CONDITION)
	{
		tt_d::TreeTimerExit("PMPI_Alltoall");
	}

	return r;
}

int MPI_Bcast(void *buffer, int count, MPI_Datatype datatype, int root, MPI_Comm comm)
{
	return PMPI_Bcast(buffer, count, datatype, root, comm);
}
int PMPI_Bcast(void *buffer, int count, MPI_Datatype datatype, int root, MPI_Comm comm)
{
	if (tt_m::lPMPI_Bcast == nullptr) {
		tt_m::lPMPI_Bcast = (int(*)(void*, int, MPI_Datatype, int, MPI_Comm))dlsym(RTLD_NEXT, "MPI_Bcast");
	}

	if(INSTR_MPI_CONDITION)
	{
		// Code Block
		tt_d::TreeTimerEnterBlock("PMPI_Bcast",TT_NODE_TYPE_MPI_COMM_CALL);

		// Parameters
		char commGroupName[MPI_MAX_OBJECT_NAME+1];
		int resultlen;
		MPI_Comm_get_name(comm, commGroupName, &resultlen);
		tt_d::TreeTimerLogParameter("CommGroupName", std::string(commGroupName));
		tt_d::TreeTimerLogParameter("RootProcess", root);

		int typeByteSize;
		MPI_Type_size(datatype, &typeByteSize);
		tt_d::TreeTimerLogParameter("MPITypeBytes", typeByteSize);
		tt_d::TreeTimerLogParameter("Count", count);
	}

	int r = tt_m::lPMPI_Bcast(buffer, count, datatype, root, comm);

	if(INSTR_MPI_CONDITION)
	{
		tt_d::TreeTimerExit("PMPI_Bcast");
	}

	return r;
}

int MPI_Reduce(const void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, int root, MPI_Comm comm)
{
	return PMPI_Reduce(sendbuf, recvbuf, count, datatype, op, root, comm);
}
int PMPI_Reduce(const void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, int root, MPI_Comm comm)
{
	if (tt_m::lPMPI_Reduce == nullptr) {
		tt_m::lPMPI_Reduce = (int(*)(const void*, void*, int, MPI_Datatype, MPI_Op, int, MPI_Comm))dlsym(RTLD_NEXT, "MPI_Reduce");
	}

	// Predefined Operations - select a timer based on the operation?

	if(INSTR_MPI_CONDITION)
	{
		// Code Block
		tt_d::TreeTimerEnterBlock("PMPI_Reduce",TT_NODE_TYPE_MPI_COMM_CALL);

		// Parameters
		char commGroupName[MPI_MAX_OBJECT_NAME+1];
		int resultlen;
		MPI_Comm_get_name(comm, commGroupName, &resultlen);
		tt_d::TreeTimerLogParameter("CommGroupName", std::string(commGroupName));
		tt_d::TreeTimerLogParameter("RootProcess", root);

		int typeByteSize;
		MPI_Type_size(datatype, &typeByteSize);
		tt_d::TreeTimerLogParameter("MPITypeBytes", typeByteSize);
		tt_d::TreeTimerLogParameter("Count", count);

		// ToDo: Track MPI Operation
	}

	int r = tt_m::lPMPI_Reduce(sendbuf, recvbuf, count, datatype, op, root, comm);

	if(INSTR_MPI_CONDITION)
	{
		tt_d::TreeTimerExit("PMPI_Reduce");
	}

	return r;
}

int MPI_Allreduce(const void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm)
{
  return PMPI_Allreduce(sendbuf, recvbuf, count, datatype, op, comm);
}
int PMPI_Allreduce(const void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm)
{
	if (tt_m::lPMPI_Allreduce == nullptr) {
		tt_m::lPMPI_Allreduce = (int(*)(const void*, void*, int, MPI_Datatype, MPI_Op, MPI_Comm))dlsym(RTLD_NEXT, "PMPI_Allreduce");
	}

	if(INSTR_MPI_CONDITION)
	{
		// Code Block
		tt_d::TreeTimerEnterBlock("PMPI_Allreduce",TT_NODE_TYPE_MPI_COMM_CALL);

		// Parameters
		char commGroupName[MPI_MAX_OBJECT_NAME+1];
		int resultlen;
		MPI_Comm_get_name(comm, commGroupName, &resultlen);
		tt_d::TreeTimerLogParameter("CommGroupName", std::string(commGroupName));

		int typeByteSize;
		MPI_Type_size(datatype, &typeByteSize);
		tt_d::TreeTimerLogParameter("MPITypeBytes", typeByteSize);
		tt_d::TreeTimerLogParameter("Count", count);

		// ToDo: Store/Track MPI Op
	}

	int r = tt_m::lPMPI_Allreduce(sendbuf, recvbuf, count, datatype, op, comm);

	if(INSTR_MPI_CONDITION)
	{
		tt_d::TreeTimerExit("PMPI_Allreduce");
	}

	return r;
}

int MPI_Unpack(const void *inbuf, int insize, int *position, void *outbuf, int outcount, MPI_Datatype datatype, MPI_Comm comm)
{
	return PMPI_Unpack(inbuf, insize, position, outbuf, outcount, datatype, comm);
}
int PMPI_Unpack(const void *inbuf, int insize, int *position, void *outbuf, int outcount, MPI_Datatype datatype, MPI_Comm comm)
{
	if (tt_m::lPMPI_Unpack == nullptr) {
		tt_m::lPMPI_Unpack = (int(*)(const void*, int, int*, void*, int, MPI_Datatype, MPI_Comm))dlsym(RTLD_NEXT, "PMPI_Unpack");
	}

	if(INSTR_MPI_CONDITION)
	{
		// Code Block
		tt_d::TreeTimerEnterBlock("PMPI_Unpack",TT_NODE_TYPE_MPI_COMM_CALL);

		// Parameters
		char commGroupName[MPI_MAX_OBJECT_NAME+1];
		int resultlen;
		MPI_Comm_get_name(comm, commGroupName, &resultlen);
		tt_d::TreeTimerLogParameter("CommGroupName", std::string(commGroupName));

		int typeByteSize;
		MPI_Type_size(datatype, &typeByteSize);
		tt_d::TreeTimerLogParameter("InSizeBytes", insize);
		tt_d::TreeTimerLogParameter("MPITypeBytes", typeByteSize);
		tt_d::TreeTimerLogParameter("Count", outcount);
	}

	int r = tt_m::lPMPI_Unpack(inbuf, insize, position, outbuf, outcount, datatype, comm);

	if(INSTR_MPI_CONDITION)
	{
		tt_d::TreeTimerExit("PMPI_Unpack");
	}

	return r;
}

int MPI_Pack(const void *inbuf, int incount, MPI_Datatype datatype, void* outbuf, int outsize, int * position, MPI_Comm comm)
{
	return PMPI_Pack(inbuf, incount, datatype, outbuf, outsize, position, comm);
}
int PMPI_Pack(const void *inbuf, int incount, MPI_Datatype datatype, void* outbuf, int outsize, int * position, MPI_Comm comm)
{
	if (tt_m::lPMPI_Pack == nullptr) {
		tt_m::lPMPI_Pack = (int(*)(const void*, int, MPI_Datatype, void*, int, int*, MPI_Comm))dlsym(RTLD_NEXT, "PMPI_Pack");
	}

	if(INSTR_MPI_CONDITION)
	{
		// Code Block
		tt_d::TreeTimerEnterBlock("PMPI_Pack",TT_NODE_TYPE_MPI_COMM_CALL);

		// Parameters
		char commGroupName[MPI_MAX_OBJECT_NAME+1];
		int resultlen;
		MPI_Comm_get_name(comm, commGroupName, &resultlen);
		tt_d::TreeTimerLogParameter("CommGroupName", std::string(commGroupName));

		int typeByteSize;
		MPI_Type_size(datatype, &typeByteSize);
		tt_d::TreeTimerLogParameter("OutSizeBytes", outsize);
		tt_d::TreeTimerLogParameter("MPITypeBytes", typeByteSize);
		tt_d::TreeTimerLogParameter("Count", incount);
	}

	int r = tt_m::lPMPI_Pack(inbuf, incount, datatype, outbuf, outsize, position, comm);

	if(INSTR_MPI_CONDITION)
	{
		tt_d::TreeTimerExit("PMPI_Pack");
	}

	return r;
}

int MPI_Bsend(const void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm)
{
	return PMPI_Bsend(buf, count, datatype, dest, tag, comm);
}
int PMPI_Bsend(const void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm)
{
	if (tt_m::lPMPI_Bsend == nullptr) {
		tt_m::lPMPI_Bsend = (int(*)(const void*, int, MPI_Datatype, int, int, MPI_Comm))dlsym(RTLD_NEXT, "PMPI_Bsend");
	}

	if(INSTR_MPI_CONDITION)
	{
		// Code Block
		tt_d::TreeTimerEnterBlock("PMPI_Bsend",TT_NODE_TYPE_MPI_COMM_CALL);

		// Parameters
		char commGroupName[MPI_MAX_OBJECT_NAME+1];
		int resultlen;
		MPI_Comm_get_name(comm, commGroupName, &resultlen);
		tt_d::TreeTimerLogParameter("CommGroupName", std::string(commGroupName));

		tt_d::TreeTimerLogParameter("DestProcess", dest);

		int typeByteSize;
		MPI_Type_size(datatype, &typeByteSize);
		tt_d::TreeTimerLogParameter("MPITypeBytes", typeByteSize);
		tt_d::TreeTimerLogParameter("Count", count);
	}

	int r = tt_m::lPMPI_Bsend(buf, count, datatype, dest, tag, comm);

	if(INSTR_MPI_CONDITION)
	{
		tt_d::TreeTimerExit("PMPI_Bsend");
	}

	return r;
}

int MPI_Buffer_attach(void *buffer, int size)
{
	return PMPI_Buffer_attach(buffer, size);
}
int PMPI_Buffer_attach(void *buffer, int size)
{
	if (tt_m::lPMPI_Buffer_attach == nullptr) {
		tt_m::lPMPI_Buffer_attach = (int(*)(void*, int))dlsym(RTLD_NEXT, "PMPI_Buffer_attach");
	}

	if(INSTR_MPI_CONDITION)
	{
		// Code Block
		tt_d::TreeTimerEnterBlock("PMPI_Buffer_attach",TT_NODE_TYPE_MPI_COMM_CALL);
	}

	int r = tt_m::lPMPI_Buffer_attach(buffer, size);

	if(INSTR_MPI_CONDITION)
	{
		tt_d::TreeTimerExit("PMPI_Buffer_attach");
	}

	return r;
}

//

int MPI_Send_init(const void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request)
{
	return PMPI_Send_init(buf, count, datatype, dest, tag, comm, request);
}
int PMPI_Send_init(const void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request)
{
	if (tt_m::lPMPI_Send_init == nullptr) {
		tt_m::lPMPI_Send_init = (int(*)(const void*, int, MPI_Datatype, int, int, MPI_Comm, MPI_Request*))dlsym(RTLD_NEXT, "PMPI_Send_init");
	}

	if(INSTR_MPI_CONDITION)
	{
		// Code Block
		tt_d::TreeTimerEnterBlock("PMPI_Send_init",TT_NODE_TYPE_MPI_COMM_CALL);

		// Parameters
		char commGroupName[MPI_MAX_OBJECT_NAME+1];
		int resultlen;
		MPI_Comm_get_name(comm, commGroupName, &resultlen);
		tt_d::TreeTimerLogParameter("CommGroupName", std::string(commGroupName));

		tt_d::TreeTimerLogParameter("DestProcess", dest);

		int typeByteSize;
		MPI_Type_size(datatype, &typeByteSize);
		tt_d::TreeTimerLogParameter("MPITypeBytes", typeByteSize);
		tt_d::TreeTimerLogParameter("Count", count);
	}

	int r = tt_m::lPMPI_Send_init(buf, count, datatype, dest, tag, comm, request);

	if(INSTR_MPI_CONDITION)
	{
		tt_d::TreeTimerExit("PMPI_Send_init");
	}

	return r;
}

int MPI_Bsend_init(const void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request)
{
	return PMPI_Bsend_init(buf, count, datatype, dest, tag, comm, request);
}
int PMPI_Bsend_init(const void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request)
{
	if (tt_m::lPMPI_Bsend_init == nullptr) {
		tt_m::lPMPI_Bsend_init = (int(*)(const void*, int, MPI_Datatype, int, int, MPI_Comm, MPI_Request*))dlsym(RTLD_NEXT, "PMPI_Bsend_init");
	}

	if(INSTR_MPI_CONDITION)
	{
		// Code Block
		tt_d::TreeTimerEnterBlock("PMPI_Bsend_init",TT_NODE_TYPE_MPI_COMM_CALL);

		// Parameters
		char commGroupName[MPI_MAX_OBJECT_NAME+1];
		int resultlen;
		MPI_Comm_get_name(comm, commGroupName, &resultlen);
		tt_d::TreeTimerLogParameter("CommGroupName", std::string(commGroupName));

		tt_d::TreeTimerLogParameter("DestProcess", dest);

		int typeByteSize;
		MPI_Type_size(datatype, &typeByteSize);
		tt_d::TreeTimerLogParameter("MPITypeBytes", typeByteSize);
		tt_d::TreeTimerLogParameter("Count", count);
	}

	int r = tt_m::lPMPI_Bsend_init(buf, count, datatype, dest, tag, comm, request);

	if(INSTR_MPI_CONDITION)
	{
		tt_d::TreeTimerExit("PMPI_Bsend_init");
	}

	return r;
}

int MPI_Ssend_init(const void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request)
{
	return PMPI_Ssend_init(buf, count, datatype, dest, tag, comm, request);
}
int PMPI_Ssend_init(const void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request)
{
	if (tt_m::lPMPI_Ssend_init == nullptr) {
		tt_m::lPMPI_Ssend_init = (int(*)(const void*, int, MPI_Datatype, int, int, MPI_Comm, MPI_Request*))dlsym(RTLD_NEXT, "PMPI_Ssend_init");
	}

	if(INSTR_MPI_CONDITION)
	{
		// Code Block
		tt_d::TreeTimerEnterBlock("PMPI_Ssend_init",TT_NODE_TYPE_MPI_COMM_CALL);

		// Parameters
		char commGroupName[MPI_MAX_OBJECT_NAME+1];
		int resultlen;
		MPI_Comm_get_name(comm, commGroupName, &resultlen);
		tt_d::TreeTimerLogParameter("CommGroupName", std::string(commGroupName));

		tt_d::TreeTimerLogParameter("DestProcess", dest);

		int typeByteSize;
		MPI_Type_size(datatype, &typeByteSize);
		tt_d::TreeTimerLogParameter("MPITypeBytes", typeByteSize);
		tt_d::TreeTimerLogParameter("Count", count);
	}

	int r = tt_m::lPMPI_Ssend_init(buf, count, datatype, dest, tag, comm, request);

	if(INSTR_MPI_CONDITION)
	{
		tt_d::TreeTimerExit("PMPI_Ssend_init");
	}

	return r;
}


int MPI_Rsend_init(const void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request)
{
	return PMPI_Rsend_init(buf, count, datatype, dest, tag, comm, request);
}
int PMPI_Rsend_init(const void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request)
{
	if (tt_m::lPMPI_Rsend_init == nullptr) {
		tt_m::lPMPI_Rsend_init = (int(*)(const void*, int, MPI_Datatype, int, int, MPI_Comm, MPI_Request*))dlsym(RTLD_NEXT, "PMPI_Rsend_init");
	}

	if(INSTR_MPI_CONDITION)
	{
		// Code Block
		tt_d::TreeTimerEnterBlock("PMPI_Rsend_init",TT_NODE_TYPE_MPI_COMM_CALL);

		// Parameters
		char commGroupName[MPI_MAX_OBJECT_NAME+1];
		int resultlen;
		MPI_Comm_get_name(comm, commGroupName, &resultlen);
		tt_d::TreeTimerLogParameter("CommGroupName", std::string(commGroupName));

		tt_d::TreeTimerLogParameter("DestProcess", dest);

		int typeByteSize;
		MPI_Type_size(datatype, &typeByteSize);
		tt_d::TreeTimerLogParameter("MPITypeBytes", typeByteSize);
		tt_d::TreeTimerLogParameter("Count", count);
	}

	int r = tt_m::lPMPI_Rsend_init(buf, count, datatype, dest, tag, comm, request);

	if(INSTR_MPI_CONDITION)
	{
		tt_d::TreeTimerExit("PMPI_Rsend_init");
	}

	return r;
}

int MPI_Recv_init(void *buf, int count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm, MPI_Request *request)
{
	return PMPI_Recv_init(buf, count, datatype, source, tag, comm, request);
}
int PMPI_Recv_init(void *buf, int count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm, MPI_Request *request)
{
	if (tt_m::lPMPI_Recv_init == nullptr) {
		tt_m::lPMPI_Recv_init = (int(*)(void*, int, MPI_Datatype, int, int, MPI_Comm, MPI_Request*))dlsym(RTLD_NEXT, "PMPI_Recv_init");
	}

	if(INSTR_MPI_CONDITION)
	{
		// Code Block
		tt_d::TreeTimerEnterBlock("PMPI_Recv_init",TT_NODE_TYPE_MPI_COMM_CALL);

		// Parameters
		char commGroupName[MPI_MAX_OBJECT_NAME+1];
		int resultlen;
		MPI_Comm_get_name(comm, commGroupName, &resultlen);
		tt_d::TreeTimerLogParameter("CommGroupName", std::string(commGroupName));

		tt_d::TreeTimerLogParameter("SourceProcess", source);

		int typeByteSize;
		MPI_Type_size(datatype, &typeByteSize);
		tt_d::TreeTimerLogParameter("MPITypeBytes", typeByteSize);
		tt_d::TreeTimerLogParameter("Count", count);
	}

	int r = tt_m::lPMPI_Recv_init(buf, count, datatype, source, tag, comm, request);

	if(INSTR_MPI_CONDITION)
	{
		tt_d::TreeTimerExit("PMPI_Recv_init");
	}

	return r;
}

int MPI_Start(MPI_Request * request)
{
	return PMPI_Start(request);
}
int PMPI_Start(MPI_Request * request)
{
	if (tt_m::lPMPI_Start == nullptr) {
		tt_m::lPMPI_Start = (int(*)(MPI_Request*))dlsym(RTLD_NEXT, "PMPI_Start");
	}

	if(INSTR_MPI_CONDITION)
	{
		tt_d::TreeTimerEnterBlock("PMPI_Start",TT_NODE_TYPE_MPI_COMM_CALL);
	}

	int r = tt_m::lPMPI_Start(request);

	if(INSTR_MPI_CONDITION)
	{
		tt_d::TreeTimerExit("PMPI_Start");
	}

	return r;
}

int MPI_Startall(int count, MPI_Request * array_of_requests)
{
	return PMPI_Startall(count, array_of_requests);
}
int PMPI_Startall(int count, MPI_Request * array_of_requests)
{
	if (tt_m::lPMPI_Startall == nullptr) {
		tt_m::lPMPI_Startall = (int(*)(int, MPI_Request*))dlsym(RTLD_NEXT, "PMPI_Startall");
	}

	if(INSTR_MPI_CONDITION)
	{
		tt_d::TreeTimerEnterBlock("PMPI_Startall",TT_NODE_TYPE_MPI_COMM_CALL);
	}

	int r = tt_m::lPMPI_Startall(count, array_of_requests);

	if(INSTR_MPI_CONDITION)
	{
		tt_d::TreeTimerExit("PMPI_Startall");
	}

	return r;
}

int MPI_Put(const void *origin_addr, int origin_count, MPI_Datatype origin_datatype, int target_rank, MPI_Aint target_disp, int target_count, MPI_Datatype target_datatype, MPI_Win win)
{
	return PMPI_Put(origin_addr, origin_count, origin_datatype, target_rank, target_disp, target_count, target_datatype, win);
}
int PMPI_Put(const void *origin_addr, int origin_count, MPI_Datatype origin_datatype, int target_rank, MPI_Aint target_disp, int target_count, MPI_Datatype target_datatype, MPI_Win win)
{
	if (tt_m::lPMPI_Put == nullptr) {
		tt_m::lPMPI_Put = (int(*)(const void*, int, MPI_Datatype, int, MPI_Aint, int, MPI_Datatype, MPI_Win))dlsym(RTLD_NEXT, "PMPI_Put");
	}

	if(INSTR_MPI_CONDITION)
	{
		int typeByteSize;

		// Code Block
		tt_d::TreeTimerEnterBlock("PMPI_Put",TT_NODE_TYPE_MPI_COMM_CALL);

		// Parameters
		MPI_Type_size(origin_datatype, &typeByteSize);
		tt_d::TreeTimerLogParameter("OriginMPITypeBytes", typeByteSize);
		tt_d::TreeTimerLogParameter("OriginCount", origin_count);

		MPI_Type_size(target_datatype, &typeByteSize);
		tt_d::TreeTimerLogParameter("TargetMPITypeBytes", typeByteSize);
		tt_d::TreeTimerLogParameter("TargetCount", target_count);

		tt_d::TreeTimerLogParameter("DestProcess", target_rank);
	}

	int r = tt_m::lPMPI_Put(origin_addr, origin_count, origin_datatype, target_rank, target_disp, target_count, target_datatype, win);

	if(INSTR_MPI_CONDITION)
	{
		tt_d::TreeTimerExit("PMPI_Put");
	}

	return r;
}

int MPI_Win_create(void *base, MPI_Aint size, int disp_unit, MPI_Info info, MPI_Comm comm, MPI_Win *win)
{
	return PMPI_Win_create(base, size, disp_unit, info, comm, win);
}
int PMPI_Win_create(void *base, MPI_Aint size, int disp_unit, MPI_Info info, MPI_Comm comm, MPI_Win *win)
{
	if (tt_m::lPMPI_Win_create == nullptr) {
		tt_m::lPMPI_Win_create = (int(*)(void*, MPI_Aint, int, MPI_Info, MPI_Comm, MPI_Win*))dlsym(RTLD_NEXT, "PMPI_Win_create");
	}

	if(INSTR_MPI_CONDITION)
	{
		tt_d::TreeTimerEnterBlock("PMPI_Win_create",TT_NODE_TYPE_MPI_COMM_CALL);
	}

	int r = tt_m::lPMPI_Win_create(base, size, disp_unit, info, comm, win);

	if(INSTR_MPI_CONDITION)
	{
		tt_d::TreeTimerExit("PMPI_Win_create");
	}

	return r;
}

int MPI_Win_start(MPI_Group group, int assert, MPI_Win win)
{
	return PMPI_Win_start(group, assert, win);
}
int PMPI_Win_start(MPI_Group group, int assert, MPI_Win win)
{
	if (tt_m::lPMPI_Win_start == nullptr) {
		tt_m::lPMPI_Win_start = (int(*)(MPI_Group, int, MPI_Win))dlsym(RTLD_NEXT, "PMPI_Win_start");
	}

	if(INSTR_MPI_CONDITION)
	{
		// Code Block
		tt_d::TreeTimerEnterBlock("PMPI_Win_start",TT_NODE_TYPE_MPI_COMM_CALL);
	}

	int r = tt_m::lPMPI_Win_start(group, assert, win);

	if(INSTR_MPI_CONDITION)
	{
		tt_d::TreeTimerExit("PMPI_Win_start");
	}

	return r;
}

int MPI_Win_fence(int assert, MPI_Win win)
{
	return PMPI_Win_fence(assert, win);
}
int PMPI_Win_fence(int assert, MPI_Win win)
{
	if (tt_m::lPMPI_Win_fence == nullptr) {
		tt_m::lPMPI_Win_fence = (int(*)(int, MPI_Win))dlsym(RTLD_NEXT, "PMPI_Win_fence");
	}

	if(INSTR_MPI_CONDITION)
	{
		// Code Block
		tt_d::TreeTimerEnterBlock("PMPI_Win_fence",TT_NODE_TYPE_MPI_COMM_CALL);
	}

	int r = tt_m::lPMPI_Win_fence(assert, win);

	if(INSTR_MPI_CONDITION)
	{
		tt_d::TreeTimerExit("PMPI_Win_fence");
	}

	return r;
}

int MPI_Win_lock(int lock_type, int rank, int assert, MPI_Win win)
{
	return PMPI_Win_lock(lock_type, rank, assert, win);
}
int PMPI_Win_lock(int lock_type, int rank, int assert, MPI_Win win)
{
	if (tt_m::lPMPI_Win_lock == nullptr) {
		tt_m::lPMPI_Win_lock = (int(*)(int, int, int, MPI_Win))dlsym(RTLD_NEXT, "PMPI_Win_lock");
	}

	if(INSTR_MPI_CONDITION)
	{
		// Code Block
		tt_d::TreeTimerEnterBlock("PMPI_Win_lock",TT_NODE_TYPE_MPI_COMM_CALL);
	}

	int r = tt_m::lPMPI_Win_lock(lock_type, rank, assert, win);

	if(INSTR_MPI_CONDITION)
	{
		tt_d::TreeTimerExit("PMPI_Win_lock");
	}

	return r;
}

int MPI_Win_unlock(int rank, MPI_Win win)
{
	return PMPI_Win_unlock(rank, win);
}
int PMPI_Win_unlock(int rank, MPI_Win win)
{
	if (tt_m::lPMPI_Win_unlock == nullptr) {
		tt_m::lPMPI_Win_unlock = (int(*)(int, MPI_Win))dlsym(RTLD_NEXT, "PMPI_Win_unlock");
	}

	if(INSTR_MPI_CONDITION)
	{
		// Code Block
		tt_d::TreeTimerEnterBlock("PMPI_Win_unlock",TT_NODE_TYPE_MPI_COMM_CALL);
	}

	int r = tt_m::lPMPI_Win_unlock(rank, win);

	if(INSTR_MPI_CONDITION)
	{
		tt_d::TreeTimerExit("PMPI_Win_unlock");
	}

	return r;
}

int MPI_Win_post(MPI_Group group, int assert, MPI_Win win)
{
	return PMPI_Win_post(group, assert, win);
}
int PMPI_Win_post(MPI_Group group, int assert, MPI_Win win)
{
	if (tt_m::lPMPI_Win_post == nullptr) {
		tt_m::lPMPI_Win_post = (int(*)(MPI_Group, int, MPI_Win))dlsym(RTLD_NEXT, "PMPI_Win_post");
	}

	if(INSTR_MPI_CONDITION)
	{
		tt_d::TreeTimerEnterBlock("PMPI_Win_post",TT_NODE_TYPE_MPI_COMM_CALL);
	}

	int r = tt_m::lPMPI_Win_post(group, assert, win);

	if(INSTR_MPI_CONDITION)
	{
		tt_d::TreeTimerExit("PMPI_Win_post");
	}

	return r;
}

int MPI_Win_complete(MPI_Win win)
{
	return PMPI_Win_complete(win);
}
int PMPI_Win_complete(MPI_Win win)
{
	if (tt_m::lPMPI_Win_complete == nullptr) {
		tt_m::lPMPI_Win_complete = (int(*)(MPI_Win))dlsym(RTLD_NEXT, "PMPI_Win_complete");
	}

	if(INSTR_MPI_CONDITION)
	{
		tt_d::TreeTimerEnterBlock("PMPI_Win_complete",TT_NODE_TYPE_MPI_COMM_CALL);
	}

	int r = tt_m::lPMPI_Win_complete(win);

	if(INSTR_MPI_CONDITION)
	{
		tt_d::TreeTimerExit("PMPI_Win_complete");
	}

	return r;
}

int MPI_Win_free(MPI_Win *win)
{
	return PMPI_Win_free(win);
}
int PMPI_Win_free(MPI_Win *win)
{
	if (tt_m::lPMPI_Win_free == nullptr) {
		tt_m::lPMPI_Win_free = (int(*)(MPI_Win*))dlsym(RTLD_NEXT, "PMPI_Win_free");
	}

	if(INSTR_MPI_CONDITION)
	{
		tt_d::TreeTimerEnterBlock("PMPI_Win_free",TT_NODE_TYPE_MPI_COMM_CALL);
	}

	int r = tt_m::lPMPI_Win_free(win);

	if(INSTR_MPI_CONDITION)
	{
		tt_d::TreeTimerExit("PMPI_Win_free");
	}

	return r;
}

int MPI_Win_wait(MPI_Win win)
{
	return PMPI_Win_wait(win);
}
int PMPI_Win_wait(MPI_Win win)
{
	if (tt_m::lPMPI_Win_wait == nullptr) {
		tt_m::lPMPI_Win_wait = (int(*)(MPI_Win))dlsym(RTLD_NEXT, "PMPI_Win_wait");
	}

	if(INSTR_MPI_CONDITION)
	{
		tt_d::TreeTimerEnterBlock("PMPI_Win_wait",TT_NODE_TYPE_MPI_COMM_CALL);
	}

	int r = tt_m::lPMPI_Win_wait(win);

	if(INSTR_MPI_CONDITION)
	{
		tt_d::TreeTimerExit("PMPI_Win_wait");
	}

	return r;
}

#endif
