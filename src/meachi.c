#include <Python.h>
#include <mpi.h>


int getIdleRank(MPI_Request *requests, int rank_size){
	int i;
	for(i=0;i<rank_size;i++){
		int complete;

		MPI_Test(requests+i, &complete, MPI_STATUS_IGNORE);
	
		if(complete){ return i; }
	}
	return -1;
}

PyObject* meachi(PyObject* self, PyObject* args){


	int i;

	int my_rank;
	int rank_size;
	int index;
	int idleRank;
	int retv;
	MPI_Status status;
	MPI_Status send_status;
	MPI_Request req;
	MPI_Request *requests;
	MPI_Status *st;

	int wkSize; 



	Py_ssize_t array_size;

	PyObject *func;
	PyObject *arglist;
	PyObject *aaa;


	if (!PyArg_ParseTuple(args, "OO", &func  ,&arglist)){
 		return NULL;
 	}
	if(!PyList_Check(arglist)){
		return Py_BuildValue("");
	}
		
	array_size = PyList_Size(arglist);


	MPI_Init(0,NULL);


	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &rank_size);

	wkSize = array_size > rank_size-1 ? rank_size-1 : array_size;

	if(my_rank==0){

		requests=calloc(wkSize,sizeof(MPI_Request));
		st=calloc(wkSize,sizeof(MPI_Status));

		index=0; // current element of ARRAY
		for(i=0; i<wkSize; i++){ // worker rank starts from 1

			MPI_Isend(&index,1,MPI_INT,i+1,0,MPI_COMM_WORLD,&req);
			MPI_Wait(&req,&send_status);
			MPI_Irecv(&retv, 1,MPI_INT,i+1,0,MPI_COMM_WORLD,requests+i);

			index++;
		}

		while(index<array_size){
			idleRank=getIdleRank(requests,wkSize);
			if(idleRank<0) continue; // no idle rank found
			MPI_Isend(&index,1,MPI_INT,idleRank+1,0,MPI_COMM_WORLD,&req);
			MPI_Wait(&req,&send_status);
			MPI_Irecv(&retv, 1,MPI_INT,idleRank+1,0,MPI_COMM_WORLD,requests+idleRank);
			index+=1;
		}

		MPI_Waitall(wkSize, requests, st);

		// send a end message to all workers
		for(i=1; i<rank_size; i++){
			index=-1;
			MPI_Send(&index,1,MPI_INT,i,0,MPI_COMM_WORLD);
		}
		free(requests);
		free(st);

	}else{
		while(1){
			retv=0;
			MPI_Recv(&index,1,MPI_INT,0,0,MPI_COMM_WORLD,&status);
			if(index<0) break; // end message from BOSS
			aaa = PyTuple_Pack(1,PyList_GetItem(arglist,index));
			PyObject_Call(func, aaa ,NULL);
			MPI_Send(&retv,1,MPI_INT,0,0,MPI_COMM_WORLD); // send completion message to BOSS
		}
	}
	MPI_Finalize();
	return Py_BuildValue("");
}


static PyMethodDef utilmethods[] = {
	{"meachi",  (PyCFunction)meachi, METH_VARARGS  },
	{NULL}
};



#if PY_MAJOR_VERSION >= 3
static struct PyModuleDef moduledef = {
    PyModuleDef_HEAD_INIT,
    "mparallel",      /* m_name */
    NULL,							     /* m_doc */
    -1,                  /* m_size */
    utilmethods,      /* m_methods */
    NULL,                /* m_reload */
    NULL,                /* m_traverse */
    NULL,                /* m_clear */
    NULL,                /* m_free */
};

PyMODINIT_FUNC
PyInit_mparallel(void){
	PyObject* m;
	m = PyModule_Create(&moduledef);
	if(m==NULL){ return m; }
	return m;
}

#else

void init_mparallel(void){
	PyObject* m;
	m = Py_InitModule("mparallel", utilmethods);
	if(m==NULL){ return ; }

}

#endif


