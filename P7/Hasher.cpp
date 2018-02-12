#include<iostream>

#include"Hasher.h"

using namespace std;

size_t hashThat(void *data, size_t size, size_t hashSize)
{
	size_t hash = 0;
	if(data == NULL)
	{
		return -1;
	}
	if(size == 0 || hashSize == 0)
	{
		return -1;
	}

	for(size_t i=0; i<size; i++)
	{
		hash += *((unsigned char *)data + i);
	}

	hash = hash % hashSize;
	return hash;
}
