#ifndef __MYCOPY_H__
#define __MYCOPY_H__

void copyBySystemFunction(char *fileSource, char *fileTarget, unsigned int recordQuantity, unsigned int recordLength);
void copyByLibraryFunction(char *fileSource, char *fileTarget, unsigned int recordQuantity, unsigned int recordLength);

#endif