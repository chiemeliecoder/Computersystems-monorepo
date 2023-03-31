intro


int** dramAllocate(int frameCount, int blockSize):

This function allocates dynamic memory for a 2D integer array with frameCount rows and blockSize columns.
It returns a pointer to the allocated memory.
If there is not enough memory to allocate, the function prints an error message and exits the program.
void freeDRAM(int*** dblPtrArr, int frameCount):

This function frees the dynamically allocated memory pointed to by dblPtrArr.
It takes frameCount as a parameter, which specifies the number of frames in the memory to be freed.
If dblPtrArr is already NULL, the function returns without doing anything.
vmTable_t* createVMtable(int length):

This function allocates dynamic memory for a vmTable_t struct, which contains three dynamically allocated integer arrays of size length.
It initializes the fields of the struct to appropriate values.
It returns a pointer to the newly allocated struct.
If there is not enough memory to allocate, the function returns NULL.





