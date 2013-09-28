#include <Python.h>

int main(int argc, char** argv) {
	Py_Initialize();

	//PyRun_SimpleString("import sys; print sys.path\n");
	FILE* file_handle = fopen("/home/liuyang/test1.py", "r");

	//先load 一个file 添加路径

	PyRun_AnyFile(file_handle, "/home/liuyang/test1.py");
	
	PyObject *rootname = PyString_FromString("test2");
	PyObject *module = PyImport_Import(rootname);

	assert(module != NULL);

	PyObject *rootname1 = PyString_FromString("test2");
	PyObject *module1 = PyImport_Import(rootname1);

//	FILE* file_handle1 = fopen("/home/liuyang/test1.py", "r");
//	PyRun_AnyFile(file_handle1, "/home/liuyang/test1.py");

	PyObject *dict = PyModule_GetDict(module);

	assert(dict != NULL);

	PyObject *func = PyDict_GetItemString(dict, "a");

	PyObject_CallObject(func, PyTuple_New(0));

	
	Py_Finalize();

	return 0;
}
