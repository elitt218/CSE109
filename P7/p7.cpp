/*
  CSE109
  Lizzie Litt
  esl218
  Program Description: create hash objects & test them
  Program #7
*/

#include<iostream>
#include<string>
#include<vector>
#include<algorithm>
#include<ctime>
#include<cstdlib>
#include<cstdio>
#include<iomanip>

#include"Hash.h"

using namespace std;

size_t solveBest(size_t size, size_t count);

template<typename T>
size_t solveCurrent(Hash<T> toScore);

template<typename T>
void testHash(size_t size, int numInputs, int doPrint);

template<typename T>
T generateCrap(size_t size, int bound);

template<typename T>
void printCrap(ostream &out, size_t size, T crap);

class crap_t
{
public:
	int x;
	char y;
	double z;
	bool operator<(const crap_t &rhs) const
		{
			return (z < rhs.z);
		}
	bool operator==(const crap_t &rhs) const
		{
			return (z == rhs.z);
		}
};
ostream & operator<<(ostream &out, const crap_t &rhs)
{
	out << rhs.x;
	return out;
}

int main(int argc, char **argv)
{
	int seed = time(NULL);
	int size = 0;
	int numInputs = 0;
	int doPrint = 0;

	for(int i=1; i<argc; i++)
	{
		char option;
		int value;
		sscanf(argv[i], "-%c:%d", &option, &value);
		switch(option)
		{
			case 's':
				size = value;
				break;
			case 'i':
				numInputs = value;
				break;
			case 'p':
				doPrint = value;
				break;
			case 'r':
				seed = value;
				break;
			default:
				cerr << "Bad argument supplied, do not understand '" << option << "'" << endl;
				break;
		}
	}

	srand(seed);

	testHash<unsigned char>(size, numInputs, doPrint);
	testHash<int>(size, numInputs, doPrint);
//	testHash<double>(size, numInputs, doPrint);
//	testHash<short>(size, numInputs, doPrint);
	testHash<crap_t>(size, numInputs, doPrint);
	return 0;
}

template<typename T>
void testHash(size_t size, int numInputs, int doPrint)
{
	Hash<T> intHash(size);

	vector<T> sourceData;
	for(int i=0; i<numInputs; i++)
	{
		T randData = generateCrap<T>(sizeof(T), size*4);
		sourceData.push_back(randData);
	}

	if(intHash.getSize() != size)
	{
		cerr << "Size of Hash does not match expected size." << endl;
		cerr << "Expected: " << size << ", Received: " << intHash.getSize() << endl;
		exit(1);

	}

	for(size_t i=0; i<sourceData.size(); i++)
	{
		size_t oldCount = intHash.getCount();
		if(oldCount != i)
		{
			cerr << "Count of Hash does not match expected count." << endl;
			cerr << "Expected: " << i << ", Received: " << oldCount << endl;
			exit(1);
		}
		bool result = intHash.insert(sourceData[i]);
		if(result)
		{
			if(i > 0)
			{
				if(find(sourceData.begin(), sourceData.begin() + i, sourceData[i]) != (sourceData.begin() + i))
				{
					cerr << "Insert returned true for inserting duplicate data: " << sourceData[i] << endl;
					exit(2);
				}
			}
		}
		else
		{
			if(find(sourceData.begin(), sourceData.begin() + i, sourceData[i]) == (sourceData.begin() + i))
			{
				cerr << "Insert returned false, claimed that " << sourceData[i] << " was inserted earlier." << endl;
				exit(3);
			}
			sourceData.erase(sourceData.begin() + i);
			i--;
		}
	}

	if(doPrint)
	{
		cout << "Hash: Size: " << intHash.getSize() << ", Elements: " << intHash.getCount() << endl;
		for(size_t i=0; i<intHash.getSize(); i++)
		{
			cout << "Bucket " << i << ": ";
			int j=0;
			while(intHash.getElement(i, j) != NULL)
			{
				T toPrint = *intHash.getElement(i,j);
				printCrap(cout, sizeof(T), toPrint);
				//printCrap<T>(cout, sizeof(T), toPrint);
				cout << " ";
				j++;
			}
			cout << endl;
		}
	}


	size_t bestScore = solveBest(intHash.getSize(), intHash.getCount());
	size_t ourScore = solveCurrent(intHash);

	cout << "Our score is: " << ourScore << ". The best score was: " << bestScore << endl;
	cout << "Lower score is better." << endl;

	vector<T> toRemove;
	for(int i=0; i<numInputs; i++)
	{
		T randValue = generateCrap<T>(sizeof(T), 4 * size);
		toRemove.push_back(randValue);
	}

	for(size_t i=0; i<toRemove.size(); i++)
	{
		int exists = 0;
		if(find(sourceData.begin(), sourceData.end(), toRemove[i]) != sourceData.end())
		{
			exists = 1;
		}

		if(!exists)
		{
			if(intHash.find(toRemove[i]))
			{
				cerr << "Found missing data " << toRemove[i] << " does not exists in our Hash." << endl;
				exit(4);
			}
			if(intHash.remove(toRemove[i]))
			{
				cerr << "Removed missing data " << toRemove[i] << " did not exist in our Hash." << endl;
				exit(5);
			}
		}
	
		else
		{
			if(!intHash.find(toRemove[i]))
			{
				cerr << "Did not find " << toRemove[i] << " in our Hash." << endl;
				exit(6);
			}
			if(!intHash.remove(toRemove[i]))
			{
				cerr << "Did not remove " << toRemove[i] << " in our Hash." << endl;
				exit(7);
			}
			sourceData.erase(find(sourceData.begin(), sourceData.end(), toRemove[i]));
		}
	}

	if(doPrint)
	{
		cout << "Hash: Size: " << intHash.getSize() << ", Elements: " << intHash.getCount() << endl;
		for(size_t i=0; i<intHash.getSize(); i++)
		{
			cout << "Bucket " << i << ": ";
			int j=0;
			while(intHash.getElement(i, j) != NULL)
			{
				T toPrint = *intHash.getElement(i,j);
				printCrap(cout, sizeof(T), toPrint);
				//printCrap<T>(cout, sizeof(T), toPrint);
				cout << " ";
			j++;
			}
			cout << endl;
		}
		cout << "Expected contents: " << endl;
		vector<T> temp(sourceData);
		sort(temp.begin(), temp.end());
		for(size_t i=0; i<temp.size(); i++)
		{
			T toPrint = temp[i];
			printCrap(cout , sizeof(T), toPrint);
			cout << " ";
		}
		cout << endl;
	}
	
	while(intHash.getCount())
	{
		int j = 0;
		while(intHash.getElement(j, 0) == NULL)
		{
			j++;
		}
		T toRemove = *intHash.getElement(j,0);
		intHash.remove(toRemove);
		if((rand() % 5) == 0)
		{
			intHash.insert(toRemove);
		}
	}
	return;
}

template<typename T>
T generateCrap(size_t size, int bound)
{
	T retval = T();
	char *ptr = (char *)&retval;
	for(size_t i=0; i<size; i++)
	{
		*(ptr + i) = (char)(rand() % bound);
	}
	return retval;
}

template<typename T>
void printCrap(ostream &out, size_t size, T crap)
{
	out << "'" << crap << "'";
	out << "(0x";

	unsigned char *ptr = (unsigned char *)&crap;
	for(size_t i=0; i<size; i++)
	{
		unsigned char at = *(ptr + i);
		out << hex << setw(2) << setfill('0') << (int)at << setfill(' ') << setw(0) << dec;
	}

	out << ")";
}

template<typename T>
size_t solveCurrent(Hash<T> toScore)
{
	size_t score = 0;

	for(size_t i=0; i<toScore.getSize(); i++)
	{
		size_t j=0;
		while(toScore.getElement(i, j) != NULL)
		{
			score += j + 1;
			j++;
		}
	}
	return score;
}

size_t solveBest(size_t size, size_t count)
{
	size_t levels = count / size;
	size_t leftover = count - (size * levels);

	size_t base = size * ((levels+1)*levels)/2;
	return base + (leftover * (levels+1));
}
