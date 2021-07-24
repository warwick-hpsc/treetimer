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
// MPI Profiling Function Call Override
// This wraps any MPI call defined here with a node block for timing etc.
// it is also suitable for storing parameter details if desired.
// The call stack should be such that fortran methods call c methods, hence
// implementation here should be sufficient for fortran calls too.
// ====================================================================================

// ToDo: MPI_Get

#include "tt_mpi.h"
#include "tt_core.h"
#include "tt_code_block_type.h"
#include "tt_global.h"
#include <string>
#include <iostream>

namespace tt_c = treetimer::core;

#define INSTR_MPI_CONDITION (tt_c::libInit && tt_c::instrumState->config->eMPIHooks && (! tt_c::instrumState->config->inLibrary) && (!tt_c::instrumState->sleeping))

int MPI_Comm_size(MPI_Comm comm, int * size)
{
	if(INSTR_MPI_CONDITION)
	{
		// Code Block
		treetimer::core::drivers::TreeTimerEnterBlock("MPI_PMPI_Comm_size",TT_NODE_TYPE_MPI_COMM_CALL);

		// Parameters
		char commGroupName[MPI_MAX_OBJECT_NAME+1];
		int resultlen;
		MPI_Comm_get_name(comm, commGroupName, &resultlen);
		treetimer::core::drivers::TreeTimerLogParameter("CommGroupName", std::string(commGroupName));
	}

	int r = PMPI_Comm_size(comm, size);

	if(INSTR_MPI_CONDITION)
	{
		treetimer::core::drivers::TreeTimerExitBlock("MPI_PMPI_Comm_size");
	}

	return r;
}

int MPI_Comm_rank(MPI_Comm comm, int * rank)
{
	if(INSTR_MPI_CONDITION)
	{
		// Code Block
		treetimer::core::drivers::TreeTimerEnterBlock("MPI_PMPI_Comm_rank",TT_NODE_TYPE_MPI_COMM_CALL);

		// Parameters
		char commGroupName[MPI_MAX_OBJECT_NAME+1];
		int resultlen;
		MPI_Comm_get_name(comm, commGroupName, &resultlen);
		treetimer::core::drivers::TreeTimerLogParameter("CommGroupName", std::string(commGroupName));
	}

	int r = PMPI_Comm_rank(comm, rank);

	if(INSTR_MPI_CONDITION)
	{
		treetimer::core::drivers::TreeTimerExitBlock("MPI_PMPI_Comm_rank");
	}

	return r;
}

int MPI_Send(const void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm)
{
	if(INSTR_MPI_CONDITION)
	{
		// Code Block
		treetimer::core::drivers::TreeTimerEnterBlock("MPI_PMPI_Send",TT_NODE_TYPE_MPI_COMM_CALL);

		// Parameters
		int typeByteSize;
		MPI_Type_size(datatype, &typeByteSize);
		treetimer::core::drivers::TreeTimerLogParameter("MPITypeBytes", typeByteSize);
		treetimer::core::drivers::TreeTimerLogParameter("Count", count);
		treetimer::core::drivers::TreeTimerLogParameter("DestProcess", dest);

		char commGroupName[MPI_MAX_OBJECT_NAME+1];
		int resultlen;
		MPI_Comm_get_name(comm, commGroupName, &resultlen);
		treetimer::core::drivers::TreeTimerLogParameter("CommGroupName", std::string(commGroupName));
	}

	int r = PMPI_Send(buf, count, datatype, dest, tag, comm);

	if(INSTR_MPI_CONDITION)
	{
		treetimer::core::drivers::TreeTimerExitBlock("MPI_PMPI_Send");
	}

	return r;
}

int MPI_Isend(const void *buf, int count, MPI_Datatype datatype, int dest, int tag,
			  MPI_Comm comm, MPI_Request * request)
{
	if(INSTR_MPI_CONDITION)
	{
		// Timer
		treetimer::core::drivers::TreeTimerEnterBlock("MPI_PMPI_Isend",TT_NODE_TYPE_MPI_COMM_CALL);

		// Parameters
		int typeByteSize;
		MPI_Type_size(datatype, &typeByteSize);
		treetimer::core::drivers::TreeTimerLogParameter("MPITypeBytes", typeByteSize);
		treetimer::core::drivers::TreeTimerLogParameter("Count", count);
		treetimer::core::drivers::TreeTimerLogParameter("DestProcess", dest);

		char commGroupName[MPI_MAX_OBJECT_NAME+1];
		int resultlen;
		MPI_Comm_get_name(comm, commGroupName, &resultlen);
		treetimer::core::drivers::TreeTimerLogParameter("CommGroupName", std::string(commGroupName));
	}

	int r = PMPI_Isend(buf, count, datatype, dest, tag, comm, request);

	if(INSTR_MPI_CONDITION)
	{
		treetimer::core::drivers::TreeTimerExitBlock("MPI_PMPI_Isend");
	}

	return r;

}

int MPI_Ssend(const void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm)
{
	if(INSTR_MPI_CONDITION)
	{
		// Code Block
		treetimer::core::drivers::TreeTimerEnterBlock("MPI_PMPI_Ssend",TT_NODE_TYPE_MPI_COMM_CALL);

		// Parameters
		char commGroupName[MPI_MAX_OBJECT_NAME+1];
		int resultlen;
		MPI_Comm_get_name(comm, commGroupName, &resultlen);
		treetimer::core::drivers::TreeTimerLogParameter("CommGroupName", std::string(commGroupName));

		treetimer::core::drivers::TreeTimerLogParameter("DestProcess", dest);

		int typeByteSize;
		MPI_Type_size(datatype, &typeByteSize);
		treetimer::core::drivers::TreeTimerLogParameter("MPITypeBytes", typeByteSize);
		treetimer::core::drivers::TreeTimerLogParameter("Count", count);
	}

	int r = PMPI_Ssend(buf, count, datatype, dest, tag, comm);

	if(INSTR_MPI_CONDITION)
	{
		treetimer::core::drivers::TreeTimerExitBlock("MPI_PMPI_Ssend");
	}

	return r;
}

int MPI_Issend(const void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request)
{
	if(INSTR_MPI_CONDITION)
	{
		// Code Block
		treetimer::core::drivers::TreeTimerEnterBlock("MPI_PMPI_Issend",TT_NODE_TYPE_MPI_COMM_CALL);

		// Parameters
		char commGroupName[MPI_MAX_OBJECT_NAME+1];
		int resultlen;
		MPI_Comm_get_name(comm, commGroupName, &resultlen);
		treetimer::core::drivers::TreeTimerLogParameter("CommGroupName", std::string(commGroupName));

		treetimer::core::drivers::TreeTimerLogParameter("DestProcess", dest);

		int typeByteSize;
		MPI_Type_size(datatype, &typeByteSize);
		treetimer::core::drivers::TreeTimerLogParameter("MPITypeBytes", typeByteSize);
		treetimer::core::drivers::TreeTimerLogParameter("Count", count);
	}

	int r = PMPI_Issend(buf, count, datatype, dest, tag, comm, request);

	if(INSTR_MPI_CONDITION)
	{
		treetimer::core::drivers::TreeTimerExitBlock("MPI_PMPI_Issend");
	}

	return r;
}

int MPI_Recv(void *buf, int count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm, MPI_Status *status)
{
	if(INSTR_MPI_CONDITION)
	{
		// Code Block
		treetimer::core::drivers::TreeTimerEnterBlock("MPI_PMPI_Recv",TT_NODE_TYPE_MPI_COMM_CALL);

		// Parameters
		int typeByteSize;
		MPI_Type_size(datatype, &typeByteSize);
		treetimer::core::drivers::TreeTimerLogParameter("MPITypeBytes", typeByteSize);
		treetimer::core::drivers::TreeTimerLogParameter("Count", count);
		treetimer::core::drivers::TreeTimerLogParameter("SourceProcess", source);

		char commGroupName[MPI_MAX_OBJECT_NAME+1];
		int resultlen;
		MPI_Comm_get_name(comm, commGroupName, &resultlen);
		treetimer::core::drivers::TreeTimerLogParameter("CommGroupName", std::string(commGroupName));
	}

	int r = PMPI_Recv(buf, count, datatype, source, tag, comm, status);

	if(INSTR_MPI_CONDITION)
	{
		treetimer::core::drivers::TreeTimerExitBlock("MPI_PMPI_Recv");
	}

	return r;
}

int MPI_Irecv(void * buf, int count, MPI_Datatype datatype, int source, int tag,
			  MPI_Comm comm, MPI_Request * request)
{
	if(INSTR_MPI_CONDITION)
	{
		// Code Block
		treetimer::core::drivers::TreeTimerEnterBlock("MPI_PMPI_Irecv",TT_NODE_TYPE_MPI_COMM_CALL);

		// Parameters
		int typeByteSize;
		MPI_Type_size(datatype, &typeByteSize);
		treetimer::core::drivers::TreeTimerLogParameter("MPITypeBytes", typeByteSize);
		treetimer::core::drivers::TreeTimerLogParameter("Count", count);
		treetimer::core::drivers::TreeTimerLogParameter("SourceProcess", source);

		char commGroupName[MPI_MAX_OBJECT_NAME+1];
		int resultlen;
		MPI_Comm_get_name(comm, commGroupName, &resultlen);
		treetimer::core::drivers::TreeTimerLogParameter("CommGroupName", std::string(commGroupName));
	}

	int r = PMPI_Irecv(buf, count, datatype, source, tag, comm, request);

	if(INSTR_MPI_CONDITION)
	{
		treetimer::core::drivers::TreeTimerExitBlock("MPI_PMPI_Irecv");
	}

	return r;
}

int MPI_Barrier(MPI_Comm comm)
{
	if(INSTR_MPI_CONDITION)
	{
		// Timer
		treetimer::core::drivers::TreeTimerEnterBlock("MPI_PMPI_Barrier",TT_NODE_TYPE_MPI_COMM_CALL);

		// Parameters
		char commGroupName[MPI_MAX_OBJECT_NAME+1];
		int resultlen;
		MPI_Comm_get_name(comm, commGroupName, &resultlen);
		treetimer::core::drivers::TreeTimerLogParameter("CommGroupName", std::string(commGroupName));
	}

	int r = PMPI_Barrier(comm);

	if(INSTR_MPI_CONDITION)
	{
		treetimer::core::drivers::TreeTimerExitBlock("MPI_PMPI_Barrier");
	}

	return r;
}

int MPI_Wait(MPI_Request *request, MPI_Status *status)
{
	if(INSTR_MPI_CONDITION)
	{
		// Code Block
		treetimer::core::drivers::TreeTimerEnterBlock("MPI_PMPI_Wait",TT_NODE_TYPE_MPI_COMM_CALL);
	}

	int r = PMPI_Wait(request, status);

	if(INSTR_MPI_CONDITION)
	{
		treetimer::core::drivers::TreeTimerExitBlock("MPI_PMPI_Wait");
	}

	return r;
}

int MPI_Waitall(int count, MPI_Request array_of_requests[], MPI_Status array_of_statuses[])
{
	if(INSTR_MPI_CONDITION)
	{
		treetimer::core::drivers::TreeTimerEnterBlock("MPI_PMPI_Waitall",TT_NODE_TYPE_MPI_COMM_CALL);
	}

	int r = PMPI_Waitall(count, array_of_requests, array_of_statuses);

	if(INSTR_MPI_CONDITION)
	{
		treetimer::core::drivers::TreeTimerExitBlock("MPI_PMPI_Waitall");
	}

	return r;
}

int MPI_Waitany(int count, MPI_Request array_of_requests[], int *indx,
               MPI_Status *status)
{
	if(INSTR_MPI_CONDITION)
	{
		treetimer::core::drivers::TreeTimerEnterBlock("MPI_PMPI_Waitany",TT_NODE_TYPE_MPI_COMM_CALL);
	}

	int r = PMPI_Waitany(count, array_of_requests, indx, status);

	if(INSTR_MPI_CONDITION)
	{
		treetimer::core::drivers::TreeTimerExitBlock("MPI_PMPI_Waitany");
	}

	return r;
}

int MPI_Test(MPI_Request *request, int * flag, MPI_Status *status)
{
	if(INSTR_MPI_CONDITION)
	{
		// Code Block
		treetimer::core::drivers::TreeTimerEnterBlock("MPI_PMPI_Test",TT_NODE_TYPE_MPI_COMM_CALL);
	}

	int r = PMPI_Test(request, flag, status);

	if(INSTR_MPI_CONDITION)
	{
		treetimer::core::drivers::TreeTimerExitBlock("MPI_PMPI_Test");
	}

	return r;
}

int MPI_Probe(int source, int tag, MPI_Comm comm, MPI_Status * status)
{
	if(INSTR_MPI_CONDITION)
	{
		// Code Block
		treetimer::core::drivers::TreeTimerEnterBlock("MPI_PMPI_Probe",TT_NODE_TYPE_MPI_COMM_CALL);
	}

	int r = PMPI_Probe(source, tag, comm, status);

	if(INSTR_MPI_CONDITION)
	{
		treetimer::core::drivers::TreeTimerExitBlock("MPI_PMPI_Probe");
	}

	return r;
}

int MPI_Gather(const void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount,
			   MPI_Datatype recvtype, int root, MPI_Comm comm)
{
	if(INSTR_MPI_CONDITION)
	{
		// Code Block
		treetimer::core::drivers::TreeTimerEnterBlock("MPI_PMPI_Gather",TT_NODE_TYPE_MPI_COMM_CALL);

		// Parameters
		char commGroupName[MPI_MAX_OBJECT_NAME+1];
		int resultlen;
		MPI_Comm_get_name(comm, commGroupName, &resultlen);
		treetimer::core::drivers::TreeTimerLogParameter("CommGroupName", std::string(commGroupName));

		treetimer::core::drivers::TreeTimerLogParameter("RootProcess", root);

		int typeByteSize;

		MPI_Type_size(sendtype, &typeByteSize);
		treetimer::core::drivers::TreeTimerLogParameter("SendMPITypeBytes", typeByteSize);
		treetimer::core::drivers::TreeTimerLogParameter("SendCount", sendcount);

		MPI_Type_size(recvtype, &typeByteSize);
		treetimer::core::drivers::TreeTimerLogParameter("RecvMPITypeBytes", typeByteSize);
		treetimer::core::drivers::TreeTimerLogParameter("RecvCount", recvcount);
	}

	int r = PMPI_Gather(sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, root, comm);

	if(INSTR_MPI_CONDITION)
	{
		treetimer::core::drivers::TreeTimerExitBlock("MPI_PMPI_Gather");
	}

	return r;

}

int MPI_Gatherv(const void *sendbuf, int sendcount, MPI_Datatype sendtype,
				void *recvbuf, const int *recvcounts, const int *displs, MPI_Datatype recvtype,
				int root, MPI_Comm comm)
{
	if(INSTR_MPI_CONDITION)
	{
		// Code Block
		treetimer::core::drivers::TreeTimerEnterBlock("MPI_PMPI_Gatherv",TT_NODE_TYPE_MPI_COMM_CALL);

		// Parameters
		char commGroupName[MPI_MAX_OBJECT_NAME+1];
		int resultlen;
		MPI_Comm_get_name(comm, commGroupName, &resultlen);
		treetimer::core::drivers::TreeTimerLogParameter("CommGroupName", std::string(commGroupName));

		treetimer::core::drivers::TreeTimerLogParameter("RootProcess", root);

		int typeByteSize;
		MPI_Type_size(sendtype, &typeByteSize);
		treetimer::core::drivers::TreeTimerLogParameter("SendMPITypeBytes", typeByteSize);
		treetimer::core::drivers::TreeTimerLogParameter("SendCount", sendcount);

		// ToDo: Tracking number of received bytes - same issue as allgatherv
		// We have send amounts and root, so we could infer it in post-processing
	}

	int r = PMPI_Gatherv(sendbuf, sendcount, sendtype, recvbuf, recvcounts, displs, recvtype, root, comm);

	if(INSTR_MPI_CONDITION)
	{
		treetimer::core::drivers::TreeTimerExitBlock("MPI_PMPI_Gatherv");
	}

	return r;
}

int MPI_Allgather(const void *sendbuf, int sendcount, MPI_Datatype sendtype,
				  void *recvbuf, int recvcount, MPI_Datatype recvtype,
				  MPI_Comm comm)
{
	if(INSTR_MPI_CONDITION)
	{
		// Code Block
		treetimer::core::drivers::TreeTimerEnterBlock("MPI_PMPI_Allgather",TT_NODE_TYPE_MPI_COMM_CALL);

		// Parameters
		int typeByteSize;

		char commGroupName[MPI_MAX_OBJECT_NAME+1];
		int resultlen;
		MPI_Comm_get_name(comm, commGroupName, &resultlen);
		treetimer::core::drivers::TreeTimerLogParameter("CommGroupName", std::string(commGroupName));

		MPI_Type_size(sendtype, &typeByteSize);
		treetimer::core::drivers::TreeTimerLogParameter("SendMPITypeBytes", typeByteSize);
		treetimer::core::drivers::TreeTimerLogParameter("SendCount", sendcount);

		MPI_Type_size(recvtype, &typeByteSize);
		treetimer::core::drivers::TreeTimerLogParameter("RecvMPITypeBytes", typeByteSize);
		treetimer::core::drivers::TreeTimerLogParameter("RecvCount", recvcount);
	}

	int r = PMPI_Allgather(sendbuf, sendcount, sendtype,
						   recvbuf, recvcount, recvtype, comm);

	if(INSTR_MPI_CONDITION)
	{
		treetimer::core::drivers::TreeTimerExitBlock("MPI_PMPI_Allgather");
	}

	return r;
}

int MPI_Allgatherv(const void *sendbuf, int sendcount, MPI_Datatype sendtype,
				   void *recvbuf, const int *recvcounts, const int *displs,
				   MPI_Datatype recvtype, MPI_Comm comm)
{
	if(INSTR_MPI_CONDITION)
	{
		// Code Block
		treetimer::core::drivers::TreeTimerEnterBlock("MPI_PMPI_Allgatherv",TT_NODE_TYPE_MPI_COMM_CALL);

		// Parameters
		char commGroupName[MPI_MAX_OBJECT_NAME+1];
		int resultlen;
		MPI_Comm_get_name(comm, commGroupName, &resultlen);
		treetimer::core::drivers::TreeTimerLogParameter("CommGroupName", std::string(commGroupName));

		int typeByteSize;

		MPI_Type_size(sendtype, &typeByteSize);
		treetimer::core::drivers::TreeTimerLogParameter("SendMPITypeBytes", typeByteSize);
		treetimer::core::drivers::TreeTimerLogParameter("SendCount", sendcount);

		// ToDo: Consider summing the number of elements to be received (recvcounts) - could we do this in post-processing
		// from send counts?
		// Don't really want to do here since the overhead would scale with process count.
	}

	int r = PMPI_Allgatherv(sendbuf, sendcount, sendtype, recvbuf, recvcounts, displs, recvtype, comm);

	if(INSTR_MPI_CONDITION)
	{
		treetimer::core::drivers::TreeTimerExitBlock("MPI_PMPI_Allgatherv");
	}

	return r;
}
int MPI_Iallgather(const void *sendbuf, int sendcount, MPI_Datatype sendtype,
				  void *recvbuf, int recvcount, MPI_Datatype recvtype,
				  MPI_Comm comm, MPI_Request * request)
{
	if(INSTR_MPI_CONDITION)
	{
		// Code Block
		treetimer::core::drivers::TreeTimerEnterBlock("MPI_PMPI_Iallgather",TT_NODE_TYPE_MPI_COMM_CALL);

		// Parameters
		char commGroupName[MPI_MAX_OBJECT_NAME+1];
		int resultlen;
		MPI_Comm_get_name(comm, commGroupName, &resultlen);
		treetimer::core::drivers::TreeTimerLogParameter("CommGroupName", std::string(commGroupName));

		int typeByteSize;

		MPI_Type_size(sendtype, &typeByteSize);
		treetimer::core::drivers::TreeTimerLogParameter("SendMPITypeBytes", typeByteSize);
		treetimer::core::drivers::TreeTimerLogParameter("SendCount", sendcount);

		MPI_Type_size(recvtype, &typeByteSize);
		treetimer::core::drivers::TreeTimerLogParameter("RecvMPITypeBytes", typeByteSize);
		treetimer::core::drivers::TreeTimerLogParameter("RecvCount", recvcount);
	}

	int r = PMPI_Iallgather(sendbuf, sendcount, sendtype,
						   recvbuf, recvcount, recvtype, comm, request);

	if(INSTR_MPI_CONDITION)
	{
		treetimer::core::drivers::TreeTimerExitBlock("MPI_PMPI_Iallgather");
	}

	return r;
}

int MPI_Alltoall(const void *sendbuf, int sendcount, MPI_Datatype sendtype,
                 void *recvbuf, int recvcount, MPI_Datatype recvtype, MPI_Comm comm)
{
	if(INSTR_MPI_CONDITION)
	{
		// Code Block
		treetimer::core::drivers::TreeTimerEnterBlock("MPI_PMPI_Alltoall",TT_NODE_TYPE_MPI_COMM_CALL);

		// Parameters
		char commGroupName[MPI_MAX_OBJECT_NAME+1];
		int resultlen;
		MPI_Comm_get_name(comm, commGroupName, &resultlen);
		treetimer::core::drivers::TreeTimerLogParameter("CommGroupName", std::string(commGroupName));

		int typeByteSize;

		MPI_Type_size(sendtype, &typeByteSize);
		treetimer::core::drivers::TreeTimerLogParameter("SendMPITypeBytes", typeByteSize);
		treetimer::core::drivers::TreeTimerLogParameter("SendCount", sendcount);

		MPI_Type_size(recvtype, &typeByteSize);
		treetimer::core::drivers::TreeTimerLogParameter("RecvMPITypeBytes", typeByteSize);
		treetimer::core::drivers::TreeTimerLogParameter("RecvCount", recvcount);
	}

	int r = PMPI_Alltoall(sendbuf, sendcount, sendtype,
						   recvbuf, recvcount, recvtype, comm);

	if(INSTR_MPI_CONDITION)
	{
		treetimer::core::drivers::TreeTimerExitBlock("MPI_PMPI_Alltoall");
	}

	return r;
}

int MPI_Bcast(void *buffer, int count, MPI_Datatype datatype, int root, MPI_Comm comm)
{
	if(INSTR_MPI_CONDITION)
	{
		// Code Block
		treetimer::core::drivers::TreeTimerEnterBlock("MPI_PMPI_Bcast",TT_NODE_TYPE_MPI_COMM_CALL);

		// Parameters
		char commGroupName[MPI_MAX_OBJECT_NAME+1];
		int resultlen;
		MPI_Comm_get_name(comm, commGroupName, &resultlen);
		treetimer::core::drivers::TreeTimerLogParameter("CommGroupName", std::string(commGroupName));
		treetimer::core::drivers::TreeTimerLogParameter("RootProcess", root);

		int typeByteSize;
		MPI_Type_size(datatype, &typeByteSize);
		treetimer::core::drivers::TreeTimerLogParameter("MPITypeBytes", typeByteSize);
		treetimer::core::drivers::TreeTimerLogParameter("Count", count);
	}

	int r = PMPI_Bcast(buffer, count, datatype, root, comm);

	if(INSTR_MPI_CONDITION)
	{
		treetimer::core::drivers::TreeTimerExitBlock("MPI_PMPI_Bcast");
	}

	return r;
}

int MPI_Reduce(const void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, int root, MPI_Comm comm)
{
	// Predefined Operations - select a timer based on the operation?

	if(INSTR_MPI_CONDITION)
	{
		// Code Block
		treetimer::core::drivers::TreeTimerEnterBlock("MPI_PMPI_Reduce",TT_NODE_TYPE_MPI_COMM_CALL);

		// Parameters
		char commGroupName[MPI_MAX_OBJECT_NAME+1];
		int resultlen;
		MPI_Comm_get_name(comm, commGroupName, &resultlen);
		treetimer::core::drivers::TreeTimerLogParameter("CommGroupName", std::string(commGroupName));
		treetimer::core::drivers::TreeTimerLogParameter("RootProcess", root);

		int typeByteSize;
		MPI_Type_size(datatype, &typeByteSize);
		treetimer::core::drivers::TreeTimerLogParameter("MPITypeBytes", typeByteSize);
		treetimer::core::drivers::TreeTimerLogParameter("Count", count);

		// ToDo: Track MPI Operation
	}

	int r = PMPI_Reduce(sendbuf, recvbuf, count, datatype, op, root, comm);

	if(INSTR_MPI_CONDITION)
	{
		treetimer::core::drivers::TreeTimerExitBlock("MPI_PMPI_Reduce");
	}

	return r;
}

int MPI_Allreduce(const void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm)
{
	if(INSTR_MPI_CONDITION)
	{
		// Code Block
		treetimer::core::drivers::TreeTimerEnterBlock("MPI_PMPI_Allreduce",TT_NODE_TYPE_MPI_COMM_CALL);

		// Parameters
		char commGroupName[MPI_MAX_OBJECT_NAME+1];
		int resultlen;
		MPI_Comm_get_name(comm, commGroupName, &resultlen);
		treetimer::core::drivers::TreeTimerLogParameter("CommGroupName", std::string(commGroupName));

		int typeByteSize;
		MPI_Type_size(datatype, &typeByteSize);
		treetimer::core::drivers::TreeTimerLogParameter("MPITypeBytes", typeByteSize);
		treetimer::core::drivers::TreeTimerLogParameter("Count", count);

		// ToDo: Store/Track MPI Op
	}

	int r = PMPI_Allreduce(sendbuf, recvbuf, count, datatype, op, comm);

	if(INSTR_MPI_CONDITION)
	{
		treetimer::core::drivers::TreeTimerExitBlock("MPI_PMPI_Allreduce");
	}

	return r;
}

int MPI_Unpack(const void *inbuf, int insize, int *position,
			   void *outbuf, int outcount, MPI_Datatype datatype,
			   MPI_Comm comm)
{
	if(INSTR_MPI_CONDITION)
	{
		// Code Block
		treetimer::core::drivers::TreeTimerEnterBlock("MPI_PMPI_Unpack",TT_NODE_TYPE_MPI_COMM_CALL);

		// Parameters
		char commGroupName[MPI_MAX_OBJECT_NAME+1];
		int resultlen;
		MPI_Comm_get_name(comm, commGroupName, &resultlen);
		treetimer::core::drivers::TreeTimerLogParameter("CommGroupName", std::string(commGroupName));

		int typeByteSize;
		MPI_Type_size(datatype, &typeByteSize);
		treetimer::core::drivers::TreeTimerLogParameter("InSizeBytes", insize);
		treetimer::core::drivers::TreeTimerLogParameter("MPITypeBytes", typeByteSize);
		treetimer::core::drivers::TreeTimerLogParameter("Count", outcount);
	}

	int r = PMPI_Unpack(inbuf, insize, position, outbuf, outcount, datatype, comm);

	if(INSTR_MPI_CONDITION)
	{
		treetimer::core::drivers::TreeTimerExitBlock("MPI_PMPI_Unpack");
	}

	return r;
}

int MPI_Pack(const void *inbuf, int incount, MPI_Datatype datatype,
			 void* outbuf, int outsize, int * position, MPI_Comm comm)
{
	if(INSTR_MPI_CONDITION)
	{
		// Code Block
		treetimer::core::drivers::TreeTimerEnterBlock("MPI_PMPI_Pack",TT_NODE_TYPE_MPI_COMM_CALL);

		// Parameters
		char commGroupName[MPI_MAX_OBJECT_NAME+1];
		int resultlen;
		MPI_Comm_get_name(comm, commGroupName, &resultlen);
		treetimer::core::drivers::TreeTimerLogParameter("CommGroupName", std::string(commGroupName));

		int typeByteSize;
		MPI_Type_size(datatype, &typeByteSize);
		treetimer::core::drivers::TreeTimerLogParameter("OutSizeBytes", outsize);
		treetimer::core::drivers::TreeTimerLogParameter("MPITypeBytes", typeByteSize);
		treetimer::core::drivers::TreeTimerLogParameter("Count", incount);
	}

	int r = PMPI_Pack(inbuf, incount, datatype, outbuf, outsize, position, comm);

	if(INSTR_MPI_CONDITION)
	{
		treetimer::core::drivers::TreeTimerExitBlock("MPI_PMPI_Pack");
	}

	return r;
}

int MPI_Bsend(const void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm)
{
	if(INSTR_MPI_CONDITION)
	{
		// Code Block
		treetimer::core::drivers::TreeTimerEnterBlock("MPI_PMPI_Bsend",TT_NODE_TYPE_MPI_COMM_CALL);

		// Parameters
		char commGroupName[MPI_MAX_OBJECT_NAME+1];
		int resultlen;
		MPI_Comm_get_name(comm, commGroupName, &resultlen);
		treetimer::core::drivers::TreeTimerLogParameter("CommGroupName", std::string(commGroupName));

		treetimer::core::drivers::TreeTimerLogParameter("DestProcess", dest);

		int typeByteSize;
		MPI_Type_size(datatype, &typeByteSize);
		treetimer::core::drivers::TreeTimerLogParameter("MPITypeBytes", typeByteSize);
		treetimer::core::drivers::TreeTimerLogParameter("Count", count);
	}

	int r = PMPI_Bsend(buf, count, datatype, dest, tag, comm);

	if(INSTR_MPI_CONDITION)
	{
		treetimer::core::drivers::TreeTimerExitBlock("MPI_PMPI_Bsend");
	}

	return r;
}

int MPI_Buffer_attach(void *buffer, int size)
{
	if(INSTR_MPI_CONDITION)
	{
		// Code Block
		treetimer::core::drivers::TreeTimerEnterBlock("MPI_PMPI_Buffer_attach",TT_NODE_TYPE_MPI_COMM_CALL);
	}

	int r = PMPI_Buffer_attach(buffer, size);

	if(INSTR_MPI_CONDITION)
	{
		treetimer::core::drivers::TreeTimerExitBlock("MPI_PMPI_Buffer_attach");
	}

	return r;
}

//

int MPI_Send_init(const void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request)
{
	if(INSTR_MPI_CONDITION)
	{
		// Code Block
		treetimer::core::drivers::TreeTimerEnterBlock("MPI_PMPI_Send_init",TT_NODE_TYPE_MPI_COMM_CALL);

		// Parameters
		char commGroupName[MPI_MAX_OBJECT_NAME+1];
		int resultlen;
		MPI_Comm_get_name(comm, commGroupName, &resultlen);
		treetimer::core::drivers::TreeTimerLogParameter("CommGroupName", std::string(commGroupName));

		treetimer::core::drivers::TreeTimerLogParameter("DestProcess", dest);

		int typeByteSize;
		MPI_Type_size(datatype, &typeByteSize);
		treetimer::core::drivers::TreeTimerLogParameter("MPITypeBytes", typeByteSize);
		treetimer::core::drivers::TreeTimerLogParameter("Count", count);
	}

	int r = PMPI_Send_init(buf, count, datatype, dest, tag, comm, request);

	if(INSTR_MPI_CONDITION)
	{
		treetimer::core::drivers::TreeTimerExitBlock("MPI_PMPI_Send_init");
	}

	return r;
}

int MPI_Bsend_init(const void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request)
{
	if(INSTR_MPI_CONDITION)
	{
		// Code Block
		treetimer::core::drivers::TreeTimerEnterBlock("MPI_PMPI_Bsend_init",TT_NODE_TYPE_MPI_COMM_CALL);

		// Parameters
		char commGroupName[MPI_MAX_OBJECT_NAME+1];
		int resultlen;
		MPI_Comm_get_name(comm, commGroupName, &resultlen);
		treetimer::core::drivers::TreeTimerLogParameter("CommGroupName", std::string(commGroupName));

		treetimer::core::drivers::TreeTimerLogParameter("DestProcess", dest);

		int typeByteSize;
		MPI_Type_size(datatype, &typeByteSize);
		treetimer::core::drivers::TreeTimerLogParameter("MPITypeBytes", typeByteSize);
		treetimer::core::drivers::TreeTimerLogParameter("Count", count);
	}

	int r = PMPI_Bsend_init(buf, count, datatype, dest, tag, comm, request);

	if(INSTR_MPI_CONDITION)
	{
		treetimer::core::drivers::TreeTimerExitBlock("MPI_PMPI_Bsend_init");
	}

	return r;
}

int MPI_Ssend_init(const void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request)
{
	if(INSTR_MPI_CONDITION)
	{
		// Code Block
		treetimer::core::drivers::TreeTimerEnterBlock("MPI_PMPI_Ssend_init",TT_NODE_TYPE_MPI_COMM_CALL);

		// Parameters
		char commGroupName[MPI_MAX_OBJECT_NAME+1];
		int resultlen;
		MPI_Comm_get_name(comm, commGroupName, &resultlen);
		treetimer::core::drivers::TreeTimerLogParameter("CommGroupName", std::string(commGroupName));

		treetimer::core::drivers::TreeTimerLogParameter("DestProcess", dest);

		int typeByteSize;
		MPI_Type_size(datatype, &typeByteSize);
		treetimer::core::drivers::TreeTimerLogParameter("MPITypeBytes", typeByteSize);
		treetimer::core::drivers::TreeTimerLogParameter("Count", count);
	}

	int r = PMPI_Ssend_init(buf, count, datatype, dest, tag, comm, request);

	if(INSTR_MPI_CONDITION)
	{
		treetimer::core::drivers::TreeTimerExitBlock("MPI_PMPI_Ssend_init");
	}

	return r;
}


int MPI_Rsend_init(const void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request)
{
	if(INSTR_MPI_CONDITION)
	{
		// Code Block
		treetimer::core::drivers::TreeTimerEnterBlock("MPI_PMPI_Rsend_init",TT_NODE_TYPE_MPI_COMM_CALL);

		// Parameters
		char commGroupName[MPI_MAX_OBJECT_NAME+1];
		int resultlen;
		MPI_Comm_get_name(comm, commGroupName, &resultlen);
		treetimer::core::drivers::TreeTimerLogParameter("CommGroupName", std::string(commGroupName));

		treetimer::core::drivers::TreeTimerLogParameter("DestProcess", dest);

		int typeByteSize;
		MPI_Type_size(datatype, &typeByteSize);
		treetimer::core::drivers::TreeTimerLogParameter("MPITypeBytes", typeByteSize);
		treetimer::core::drivers::TreeTimerLogParameter("Count", count);
	}

	int r = PMPI_Rsend_init(buf, count, datatype, dest, tag, comm, request);

	if(INSTR_MPI_CONDITION)
	{
		treetimer::core::drivers::TreeTimerExitBlock("MPI_PMPI_Rsend_init");
	}

	return r;
}

int MPI_Recv_init(void *buf, int count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm, MPI_Request *request)
{
	if(INSTR_MPI_CONDITION)
	{
		// Code Block
		treetimer::core::drivers::TreeTimerEnterBlock("MPI_PMPI_Recv_init",TT_NODE_TYPE_MPI_COMM_CALL);

		// Parameters
		char commGroupName[MPI_MAX_OBJECT_NAME+1];
		int resultlen;
		MPI_Comm_get_name(comm, commGroupName, &resultlen);
		treetimer::core::drivers::TreeTimerLogParameter("CommGroupName", std::string(commGroupName));

		treetimer::core::drivers::TreeTimerLogParameter("SourceProcess", source);

		int typeByteSize;
		MPI_Type_size(datatype, &typeByteSize);
		treetimer::core::drivers::TreeTimerLogParameter("MPITypeBytes", typeByteSize);
		treetimer::core::drivers::TreeTimerLogParameter("Count", count);
	}

	int r = PMPI_Recv_init(buf, count, datatype, source, tag, comm, request);

	if(INSTR_MPI_CONDITION)
	{
		treetimer::core::drivers::TreeTimerExitBlock("MPI_PMPI_Recv_init");
	}

	return r;
}

int MPI_Start(MPI_Request * request)
{
	if(INSTR_MPI_CONDITION)
	{
		treetimer::core::drivers::TreeTimerEnterBlock("MPI_PMPI_Start",TT_NODE_TYPE_MPI_COMM_CALL);
	}

	int r = PMPI_Start(request);

	if(INSTR_MPI_CONDITION)
	{
		treetimer::core::drivers::TreeTimerExitBlock("MPI_PMPI_Start");
	}

	return r;
}

int MPI_Startall(int count, MPI_Request * array_of_requests)
{
	if(INSTR_MPI_CONDITION)
	{
		treetimer::core::drivers::TreeTimerEnterBlock("MPI_PMPI_Startall",TT_NODE_TYPE_MPI_COMM_CALL);
	}

	int r = PMPI_Startall(count, array_of_requests);

	if(INSTR_MPI_CONDITION)
	{
		treetimer::core::drivers::TreeTimerExitBlock("MPI_PMPI_Startall");
	}

	return r;
}

int MPI_Put(const void *origin_addr, int origin_count, MPI_Datatype origin_datatype, int target_rank,
			MPI_Aint target_disp, int target_count, MPI_Datatype target_datatype, MPI_Win win)
{
	if(INSTR_MPI_CONDITION)
	{
		int typeByteSize;

		// Code Block
		treetimer::core::drivers::TreeTimerEnterBlock("MPI_PMPI_Put",TT_NODE_TYPE_MPI_COMM_CALL);

		// Parameters
		MPI_Type_size(origin_datatype, &typeByteSize);
		treetimer::core::drivers::TreeTimerLogParameter("OriginMPITypeBytes", typeByteSize);
		treetimer::core::drivers::TreeTimerLogParameter("OriginCount", origin_count);

		MPI_Type_size(target_datatype, &typeByteSize);
		treetimer::core::drivers::TreeTimerLogParameter("TargetMPITypeBytes", typeByteSize);
		treetimer::core::drivers::TreeTimerLogParameter("TargetCount", target_count);

		treetimer::core::drivers::TreeTimerLogParameter("DestProcess", target_rank);
	}

	int r = PMPI_Put(origin_addr, origin_count, origin_datatype, target_rank, target_disp, target_count, target_datatype, win);

	if(INSTR_MPI_CONDITION)
	{
		treetimer::core::drivers::TreeTimerExitBlock("MPI_PMPI_Put");
	}

	return r;
}

int MPI_Win_create(void *base, MPI_Aint size, int disp_unit, MPI_Info info, 
                  MPI_Comm comm, MPI_Win *win)
{
	if(INSTR_MPI_CONDITION)
	{
		treetimer::core::drivers::TreeTimerEnterBlock("MPI_PMPI_Win_create",TT_NODE_TYPE_MPI_COMM_CALL);
	}

	int r = PMPI_Win_create(base, size, disp_unit, info, comm, win);

	if(INSTR_MPI_CONDITION)
	{
		treetimer::core::drivers::TreeTimerExitBlock("MPI_PMPI_Win_create");
	}

	return r;
}

int MPI_Win_start(MPI_Group group, int assert, MPI_Win win)
{
	if(INSTR_MPI_CONDITION)
	{
		// Code Block
		treetimer::core::drivers::TreeTimerEnterBlock("MPI_PMPI_Win_start",TT_NODE_TYPE_MPI_COMM_CALL);
	}

	int r = PMPI_Win_start(group, assert, win);

	if(INSTR_MPI_CONDITION)
	{
		treetimer::core::drivers::TreeTimerExitBlock("MPI_PMPI_Win_start");
	}

	return r;
}

int MPI_Win_fence(int assert, MPI_Win win)
{
	if(INSTR_MPI_CONDITION)
	{
		// Code Block
		treetimer::core::drivers::TreeTimerEnterBlock("MPI_PMPI_Win_fence",TT_NODE_TYPE_MPI_COMM_CALL);
	}

	int r = PMPI_Win_fence(assert, win);

	if(INSTR_MPI_CONDITION)
	{
		treetimer::core::drivers::TreeTimerExitBlock("MPI_PMPI_Win_fence");
	}

	return r;
}

int MPI_Win_lock(int lock_type, int rank, int assert, MPI_Win win)
{
	if(INSTR_MPI_CONDITION)
	{
		// Code Block
		treetimer::core::drivers::TreeTimerEnterBlock("MPI_PMPI_Win_lock",TT_NODE_TYPE_MPI_COMM_CALL);
	}

	int r = PMPI_Win_lock(lock_type, rank, assert, win);

	if(INSTR_MPI_CONDITION)
	{
		treetimer::core::drivers::TreeTimerExitBlock("MPI_PMPI_Win_lock");
	}

	return r;
}

int MPI_Win_unlock(int rank, MPI_Win win)
{
	if(INSTR_MPI_CONDITION)
	{
		// Code Block
		treetimer::core::drivers::TreeTimerEnterBlock("MPI_PMPI_Win_unlock",TT_NODE_TYPE_MPI_COMM_CALL);
	}

	int r = PMPI_Win_unlock(rank, win);

	if(INSTR_MPI_CONDITION)
	{
		treetimer::core::drivers::TreeTimerExitBlock("MPI_PMPI_Win_unlock");
	}

	return r;
}

int MPI_Win_post(MPI_Group group, int assert, MPI_Win win)
{
	if(INSTR_MPI_CONDITION)
	{
		treetimer::core::drivers::TreeTimerEnterBlock("MPI_PMPI_Win_post",TT_NODE_TYPE_MPI_COMM_CALL);
	}

	int r = PMPI_Win_post(group, assert, win);

	if(INSTR_MPI_CONDITION)
	{
		treetimer::core::drivers::TreeTimerExitBlock("MPI_PMPI_Win_post");
	}

	return r;
}

int MPI_Win_complete(MPI_Win win)
{
	if(INSTR_MPI_CONDITION)
	{
		treetimer::core::drivers::TreeTimerEnterBlock("MPI_PMPI_Win_complete",TT_NODE_TYPE_MPI_COMM_CALL);
	}

	int r = PMPI_Win_complete(win);

	if(INSTR_MPI_CONDITION)
	{
		treetimer::core::drivers::TreeTimerExitBlock("MPI_PMPI_Win_complete");
	}

	return r;
}

int MPI_Win_free(MPI_Win *win)
{
	if(INSTR_MPI_CONDITION)
	{
		treetimer::core::drivers::TreeTimerEnterBlock("MPI_PMPI_Win_free",TT_NODE_TYPE_MPI_COMM_CALL);
	}

	int r = PMPI_Win_free(win);

	if(INSTR_MPI_CONDITION)
	{
		treetimer::core::drivers::TreeTimerExitBlock("MPI_PMPI_Win_free");
	}

	return r;
}

int MPI_Win_wait(MPI_Win win)
{
	if(INSTR_MPI_CONDITION)
	{
		treetimer::core::drivers::TreeTimerEnterBlock("MPI_PMPI_Win_wait",TT_NODE_TYPE_MPI_COMM_CALL);
	}

	int r = PMPI_Win_wait(win);

	if(INSTR_MPI_CONDITION)
	{
		treetimer::core::drivers::TreeTimerExitBlock("MPI_PMPI_Win_wait");
	}

	return r;
}

