#pragma once


//#include <iostream>
//#include <functional>
//#include <list>
//#include <math.h>
//#include <fcntl.h>
//#include <unistd.h>
//#include <cstring>
//#include <sstream>
//#include <string>
//#include <stdlib.h>
//#include <stdio.h>
//
//
//
//class MemoryManager
//{ 
//
//public:
//	struct map
//	{
//		int StartAddress;
//		int EndAddress;
//		void* datap;
//		bool operator <(const map& var) const
//		{
//			return this->StartAddress < var.StartAddress;
//		}
//		bool operator >(const map& var) const
//		{
//			return this->StartAddress > var.StartAddress;
//		}
//		bool operator == (const map& var) const
//		{
//			return this->StartAddress > var.StartAddress;
//		}
//		
//	};
//
//	bool init = false;
//	void* block;
//	unsigned wordsize;
//	std::list<map> datablock;
//	size_t sizeInWords;
//	bool first = true;
//	
//	std::function<int(int, void*)> allocator;
//
//	
//
//
//
//
//	
//
//	MemoryManager(unsigned wordSize, std::function<int(int, void*)> allocator);
//	~MemoryManager();
//
//	unsigned nativeWordSize;
//	void initialize(size_t sizeInWords);
//	void setAllocator(std::function<int(int, void*)> allocator);
//	void* allocate(size_t sizeInBytes);
//	void free(void* address);
//	void shutdown();
//	int dumpMemoryMap(char* filename);
//	void* getList();
//	void* getBitmap();
//	unsigned getWordSize();
//	void* getMemoryStart();
//	unsigned getMemoryLimit();
//};
//
//extern int bestFit(int sizeInWords, void* list);
//extern int worstFit(int sizeInWords, void* list);
//#pragma once
//#include <iostream>
//#include <functional>
//#include <list>
//#include <math.h>
//#include <fcntl.h>
//#include <unistd.h>
//#include <cstring>
//#include <sstream>
//#include <string>
//#include <stdlib.h>
//#include <stdio.h>
#include <functional>
#include <math.h>
#include <vector>
#include <sstream>
#include <string>
#include <iostream>
#include <stdlib.h> 
#include <unistd.h>
#include <fcntl.h>
#include <list>
#include <cmath>
#include <cstring>

class MemoryManager {
public:
    struct memBlock {
        int numWords;
        void* startAddr;
        int relativeAddr;
        int endAddr;

        memBlock(int numWords, void* startAddr, int relativeAddr, int endAddr) {
            this->numWords = numWords;
            this->startAddr = startAddr;
            this->relativeAddr = relativeAddr;
            this->endAddr = endAddr;
        }

        //Method returns the end address relative to the starting address being 0
        int getEndAddr() {
            return endAddr;
        }

        //Overloading operator to allow for sorting of list
        bool operator <(const memBlock& block) const {
            return this->relativeAddr < block.relativeAddr;
        }
        bool operator ==(const memBlock& block) const {
            return this->relativeAddr == block.relativeAddr;
        }
        bool operator >(const memBlock& block) const {
            return this->relativeAddr > block.relativeAddr;
        }
    };

    //State Variables
    std::function<int(int, void*)> allocator;
    std::list<memBlock> memory;
    void* mem_location;
    unsigned wordSize;
    size_t sizeInWords;
    bool initialized;

    //Methods
    MemoryManager(unsigned wordSize, std::function<int(int, void*)> allocator);
    ~MemoryManager();
    void initialize(size_t sizeInWords);
    void shutdown();
    void* allocate(size_t sizeInBytes);
    void free(void* address);
    void setAllocator(std::function<int(int, void*)> allocator);
    int dumpMemoryMap(char* filename);
    void* getList();
    void* getBitmap();
    unsigned getWordSize();
    void* getMemoryStart();
    unsigned getMemoryLimit();

};

extern int bestFit(int sizeInWords, void* list);
extern int worstFit(int sizeInWords, void* list);
