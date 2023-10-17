#include "MemoryManager.h"




//using namespace std;
//
///// <summary>
///// Constructor
///// Sets native word size (in bytes, for alignment) and default allocator for finding a memory hole.
///// </summary>
///// <param name="wordSize"></param>
///// <param name="allocator"></param>
//MemoryManager::MemoryManager(unsigned wordSize, std::function<int(int, void*)> allocator)
//{
//	this->wordsize = wordSize;
//	this->allocator = allocator;
//}
//
//
//MemoryManager::~MemoryManager()
//{
//	shutdown();
//}
///// <summary>
///// Instantiates block of requested size, no larger than 65536 words; cleans up previous block if applicable.
///// </summary>
///// <param name="sizeInWords"></param>
//void MemoryManager::initialize(size_t sizeInWords)
//{
//	if (init == true)
//	{
//		shutdown();
//	}
//	this->sizeInWords = sizeInWords;
//	block = sbrk(sizeInWords * wordsize);
//	init = true;
//	
//}
//
///// <summary>
///// Changes the allocation algorithm to identifying the memory hole to use for allocation.
///// </summary>
///// <param name="allocator"></param>
//void MemoryManager::setAllocator(std::function<int(int, void*)> allocator)
//{
//	this->allocator = allocator;
//
//}
//
///// <summary>
///// Allocates a memory using the allocator function. If no memory is available or size is invalid, returns nullptr.
///// </summary>
///// <param name="sizeInBytes"></param>
///// <returns></returns>
//void* MemoryManager::allocate(size_t sizeInBytes)
//{
//	int size;
//	if (sizeInBytes % wordsize != 0)
//	{
//		size = sizeInBytes / wordsize + 1;
//	}
//	else
//	{
//		size = sizeInBytes / wordsize;
//		
//	}
//	uint16_t* holeList = static_cast<uint16_t*>(getList());
//	
//	int adressReturn = allocator(size, holeList);// allocator function call to find start adress
//
//	if (adressReturn == -1 && !first)
//
//	{
//		return nullptr;
//	}
//	else
//	{
//		if (first)
//		{
//			adressReturn = 0;
//			first = false;
//		}
//		map m;
//		m.StartAddress = adressReturn; // need functionality to find start adress
//		m.EndAddress = adressReturn + size - 1; // need functionality to find end adress
//		m.datap = static_cast<int*>(block) + adressReturn * 2;
//		//unsigned char* bytePtr = static_cast<unsigned char*>(block);
//		//m.datap = static_cast<void*>(bytePtr + m.StartAddress);
//		datablock.emplace_back(m);
//		delete[] holeList;
//		return m.datap;
//	}
//
//}
//
///// <summary>
///// Frees the memory block within the memory manager so that it can be reused.
///// </summary>
///// <param name="address"></param>
//void MemoryManager::free(void* address)
//{
//	list<map>::iterator it;
//	for (it = datablock.begin(); it != datablock.end(); it++)
//	{
//		if (it->datap == address)
//		{
//			//datablock.remove(it);
//			//delete static_cast<map>(address);
//			it = datablock.erase(it);
//			
//		}
//	}
//
//}
//
///// <summary>
///// Releases memory block acquired during initialization, if any.
///// </summary>
//void MemoryManager::shutdown()
//{
//	if (init == true)
//	{
//		
//		datablock.clear();
//		sbrk(sizeInWords * wordsize * - 1);
//		init = false;
//		first = true;
//	}
//}
//
///// <summary>
///// Uses standard POSIX calls to write hole list to filename as text; returning -1 on error and 0 if successful.
///// </summary>
///// <param name="filename"></param>
///// <returns></returns>
//int MemoryManager::dumpMemoryMap(char* filename)
//{
//	string Response;
//	
//
//	uint16_t* temp = static_cast<uint16_t*>(getList());
//	for (int i = 1; i < temp[0]*2 - 1; i = i + 2)
//	{
//		
//	
//		Response = Response + "[";
//		Response = Response + to_string(temp[i]);
//		Response = Response + ",";
//		Response = Response + " ";
//		Response = Response + to_string(temp[i + 1]);
//		Response = Response + "]";
//		Response = Response + " ";
//		Response = Response + "-";
//		Response = Response + " ";
//	}
//
//	
//	Response = Response + "[";
//	Response = Response + to_string(temp[temp[0] * 2 - 1]);
//	Response = Response + ",";
//	Response = Response + " ";
//	Response = Response + to_string(temp[temp[0] * 2]);
//	Response = Response + "]";
//
//	delete[] temp;
//
//	
//
//	int fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 00700);
//	if (write(fd, Response.c_str(), Response.length()) == -1)
//	{
//		return -1;
//	}
//	else
//	{
//		close(fd);
//		return 0;
//	}
//
//	
//
//
//
//	return 0;
//}
//
///// <summary>
///// Returns a byte-stream of information (in binary) about holes for use by the allocator function (little-Endian).
///// Offsetand length are in words.If no memory has been allocated, the function should return a NULL pointer.
///// </summary>
///// <returns></returns>
//void* MemoryManager::getList()
//{
//	uint16_t EndOfPrev = 0;
//	uint16_t NumOfHoles = 0;
//	uint16_t Offset = 0;
//	uint16_t Length = 0;
//	uint16_t arrayIt = 0;
//	datablock.sort();
//	list<map>::iterator it;
//	for (it = datablock.begin(); it != datablock.end(); it++)
//	{
//		if (it == datablock.begin())  
//		{
//			if (it->StartAddress != 0)// hole in begiing of data
//			{
//				NumOfHoles++;
//				
//			}
//			if (datablock.size() == 1 && it->EndAddress != sizeInWords-1) // only object in data
//			{
//				NumOfHoles++;
//				
//			}
//			
//		}
//
//		 if (EndOfPrev + 1 != it->StartAddress && it != datablock.begin()) // hole in middle of data
//		{
//			NumOfHoles++;
//			
//		}
//		 if (it == --datablock.end()) // last object in data
//		{
//			if (it->EndAddress != sizeInWords - 1)
//			{
//				NumOfHoles++;
//			}
//		}
//		EndOfPrev = it->EndAddress;
//
//	}
//	uint16_t* arr = new uint16_t[NumOfHoles * 2 + 1];
//	arr[arrayIt] = NumOfHoles;
//	arrayIt++;
//	if (NumOfHoles != 0)
//	{
//		for (it = datablock.begin(); it != datablock.end(); it++)
//		{
//			if (it == datablock.begin()) // if first element in list
//			{
//				if (it->StartAddress != 0) // if there is a whole in the beggining
//				{
//					Offset = 0;
//					Length = it->StartAddress;
//					arr[arrayIt] = Offset;
//					arrayIt++;
//					arr[arrayIt] = Length;
//					arrayIt++;
//				}
//
//			}
//			if (it == --datablock.end()) // last block of data
//			{
//				if (it->StartAddress != EndOfPrev + 1) // whole before last entry
//				{
//					Offset = EndOfPrev + 1;
//					Length = it->StartAddress - EndOfPrev - 1;
//					arr[arrayIt] = Offset;
//					arrayIt++;
//					arr[arrayIt] = Length;
//					arrayIt++;
//				}
//
//				if (it->EndAddress != sizeInWords - 1)
//				{
//					Offset = it->EndAddress + 1;
//					Length = sizeInWords - it->EndAddress - 1;
//					arr[arrayIt] = Offset;
//					arrayIt++;
//					arr[arrayIt] = Length;
//					arrayIt++;
//				}
//			}
//			if (it != --datablock.end() && it != datablock.begin()) // middle of the data
//			{
//				if (EndOfPrev + 1 != it->StartAddress)
//				{
//
//					Offset = EndOfPrev + 1;
//					Length = it->StartAddress - EndOfPrev - 1;
//					EndOfPrev = it->EndAddress;
//					arr[arrayIt] = Offset;
//					arrayIt++;
//					arr[arrayIt] = Length;
//					arrayIt++;
//				}
//
//			}
//			EndOfPrev = it->EndAddress;
//		}
//
//
//	}
//	
//	
//	
//
//	return arr;
//
//}
//
///// <summary>
///// Returns a bit-stream of bits representing whether words are used (1) or free (0). The first two bytes are the
///// size of the bitmap(little - Endian); the rest is the bitmap, word - wise.
///// </summary>
///// <returns></returns>
//void* MemoryManager::getBitmap()
//{
//	int curr = 0; // adress of data that was last scanned for bits
//	int TotalBytes = 0; //size of bytes for amount of bits
//	if (sizeInWords % 8 != 0)
//	{
//		TotalBytes = sizeInWords / 8 + 1;
//	}
//	else 
//	{
//		TotalBytes = sizeInWords / 8;
//	}
//	list<map>::reverse_iterator it;
//	int arrayIt = 0;
//	uint8_t* arr = new uint8_t[sizeInWords]; // array of size of words to get all words out of structure
//	int temper = TotalBytes + 2;
//	uint8_t* secondArr = new uint8_t[TotalBytes + 2]; // array od Total Btyees plus room for 2 size decimals
//	int arrayIt2 = TotalBytes + 1;
//	if (TotalBytes < 255)
//	{
//		secondArr[0] = TotalBytes;
//		secondArr[1] = 0;
//	}
//	else
//	{
//		secondArr[0] = TotalBytes / 255;
//		secondArr[1] = 255;
//	}
//
//	datablock.sort();
//	// go through data and get values
//	for (it = datablock.rbegin(); it != datablock.rend(); it++) // check end
//	{
//		if (it == datablock.rbegin() ) // end data block
//		{
//			if (it->EndAddress != sizeInWords - 1) // if there is a hole at the bottom of data
//			{
//				for (int i = 0; i < sizeInWords - it->EndAddress - 1; i++) // addd zeros for hole at bottom of data
//				{
//					arr[arrayIt] = 0;
//					arrayIt++;
//				}
//
//			}
//			for(int i = 0; i < it->EndAddress + 1 - it->StartAddress; i++)
//			{
//				arr[arrayIt] = 1;
//				arrayIt++ ;
//			}
//			curr = it->StartAddress;
//
//		}
//		if (it != datablock.rbegin() && it != --datablock.rend()) // middle of data
//		{
//			if (it->EndAddress != curr - 1) // if there is a whole 
//			{
//				for (int i = 0; i < (curr - 1) - it->EndAddress; i++)
//				{
//					arr[arrayIt] = 0;
//					arrayIt++;
//				}
//			}
//			
//				for (int i = 0; i <= it->EndAddress - it->StartAddress; i++)
//				{
//					arr[arrayIt] = 1;
//					arrayIt++;
//				}
//			curr = it->StartAddress;
//
//		}
//		if (it == --datablock.rend())
//		{
//
//			if (it->EndAddress != curr - 1) // if there is a whole before last entry
//			{
//				for (int i = 0; i < (curr - 1) - it->EndAddress; i++)
//				{
//					arr[arrayIt] = 0;
//					arrayIt++;
//				}
//			}
//
//			if (it->StartAddress == 0) // there is no hole at the beginnning of data
//			{
//				for (int i = 0; i <= it->EndAddress - it->StartAddress; i++)
//				{
//					arr[arrayIt] = 1;
//					arrayIt++;
//				}
//
//			}
//			else // there is a hole at the begiinnning of data
//			{
//				for (int i = 0; i < it->EndAddress + 1 - it->StartAddress; i++)
//				{
//					arr[arrayIt] = 1;
//					arrayIt++;
//				}
//
//				for (int i = 0; i < it->StartAddress; i++)
//				{
//					arr[arrayIt] = 0;
//					arrayIt++;
//				}
//			}
//				
//		}
//	}
//	string number;
//	arrayIt = 0;
//	if (sizeInWords % 8 != 0)
//	{
//		for (int i = 0; i < sizeInWords % 8; i++)
//		{
//			number.append(to_string(arr[arrayIt]));
//			arrayIt++;
//			
//		}
//
//		uint8_t dec = 0;
//		uint8_t base = 1;
//		for (int i = number.length() - 1; i >= 0; i--)
//		{
//			if (number[i] == '1')
//			{
//				dec += base;
//			}
//			base = base * 2;
//		}
//
//		secondArr[arrayIt2] = dec;
//		arrayIt2--;
//		number.clear();
//
//	}
//
//	
//		
//
//	// convert to decimal string
//	curr = 0;
//	while (curr < 8 && arrayIt2 >= 2)
//	{
//			
//
//			number.append(to_string(arr[arrayIt]));
//
//			if (curr == 7 || arrayIt == sizeInWords - 1)
//			{
//				uint8_t dec = 0;
//				uint8_t base = 1;
//				for (int i = number.length() - 1; i >= 0; i--)
//				{
//					if (number[i] == '1')
//					{
//						dec += base;
//					}
//					base = base * 2;
//				}
//
//				secondArr[arrayIt2] = dec;
//				arrayIt2--;
//				curr = 0;
//				number.clear();
//			}
//			else
//			{
//				curr++;
//			}
//			arrayIt++;
//
//	}
//	//delete arr;
//	delete[] arr;
//
//	return secondArr;
//}
//
///// <summary>
///// Returns the word size used for alignment.
///// </summary>
///// <returns></returns>
//unsigned MemoryManager::getWordSize()
//{
//	return wordsize;
//}
//
///// <summary>
///// Returns the byte-wise memory address of the beginning of the memory block.
///// </summary>
///// <returns></returns>
//void* MemoryManager::getMemoryStart()
//{
//	return block;
//}
//
///// <summary>
///// Returns the byte limit of the current memory block.
///// </summary>
///// <returns></returns>
//unsigned MemoryManager::getMemoryLimit()
//{
//	return sizeInWords * wordsize;
//}
//
///// <summary>
///// Returns word offset of hole selected by the best fit memory allocation algorithm, and -1 if there is no fit.
///// </summary>
///// <param name="sizeInWords"></param>
///// <param name="list"></param>
///// <returns></returns>
//int bestFit(int sizeInWords, void* list)
//{
//
//	uint16_t* lister = static_cast<uint16_t*>(list);
//	int SizeOfFit = 65537;
//	int RetOff = 0;
//	bool found = false;
//	for (int i = 2; i < lister[0]*2+1; i = i + 2)
//	{
//		if (lister[i] - sizeInWords < SizeOfFit && lister[i] - sizeInWords >= 0)
//		{
//			SizeOfFit = lister[i] - sizeInWords;
//			RetOff = lister[i - 1];
//			found = true;
//		}
//
//	}
//	//delete[] lister;
//
//	if (found)
//	{
//		return RetOff;
//	}
//	else
//	{
//		return -1;
//	}
//	
//}
//
///// <summary>
///// Returns word offset of hole selected by the worst fit memory allocation algorithm, and -1 if there is no fit.
///// </summary>
///// <param name="sizeInWords"></param>
///// <param name="list"></param>
///// <returns></returns>
//int worstFit(int sizeInWords, void* list)
//{
//	uint16_t* lister = static_cast<uint16_t*>(list);
//	int SizeOfFit = 0;
//	int RetOff = 0;
//	bool found = false;
//	for (int i = 2; i < lister[0] * 2 + 1; i = i + 2)
//	{
//		if (lister[i] - sizeInWords > SizeOfFit && lister[i] - sizeInWords >= 0)
//		{
//			SizeOfFit = lister[i] - sizeInWords;
//			RetOff = lister[i - 1];
//			found = true;
//		}
//
//	}
//	//delete[] lister;
//
//	if (found)
//	{
//		return RetOff;
//	}
//	else
//	{
//		return -1;
//	}
//}
//

//Constructor
MemoryManager::MemoryManager(unsigned wordSize, std::function<int(int, void*)> allocator) {
    this->wordSize = wordSize;
    this->allocator = allocator;
    sizeInWords = 0;
    initialized = false;
}

//Destructor
MemoryManager::~MemoryManager() {
    //Delete whatever was initialized
    shutdown();
}

//Allocates block of memory using malloc, stores starting address in mem_location
void MemoryManager::initialize(size_t sizeInWords) {
    if (initialized) {
        this->~MemoryManager();
    }
    this->sizeInWords = sizeInWords;
    mem_location = sbrk(wordSize * sizeInWords);
    initialized = true;
}

//calls deconstructor
void MemoryManager::shutdown() {
    if (initialized) {
        memory.clear();
        //std::free(mem_location);
        sbrk(wordSize * sizeInWords * -1);
        initialized = false;
    }

}


void* MemoryManager::allocate(size_t sizeInBytes) {

    int numWords = sizeInBytes / wordSize;
    if (sizeInBytes % wordSize != 0) {
        numWords += 1;
    }
    uint16_t* holes = static_cast<uint16_t*>(getList());
    int relativeAddr = allocator(numWords, holes);
    delete[] holes;
    if (relativeAddr == -1) {
        return nullptr;
    }
    unsigned char* bytePtr = static_cast<unsigned char*>(mem_location);
    void* startAddr = static_cast<void*>(bytePtr + relativeAddr * wordSize);
    int endAddr = relativeAddr + numWords - 1;
    memory.push_back(memBlock(numWords, startAddr, relativeAddr, endAddr));

    return startAddr;
}
void MemoryManager::free(void* address) {
    //Remove memBlock at that address if there is one
    std::list<memBlock>::iterator it;
    for (it = memory.begin(); it != memory.end(); it++) {
        if (it->startAddr == address) {
            it = memory.erase(it);
        }
    }
}


void MemoryManager::setAllocator(std::function<int(int, void*)> allocator) {
    this->allocator = allocator;
}


int MemoryManager::dumpMemoryMap(char* filename) {

  
    uint16_t* holes = static_cast<uint16_t*>(getList());
    uint16_t arrSize = holes[0];
    std::string memMap = "";
    bool firstPass = true;
    for (int i = 1; i < arrSize * 2 + 1; i += 2) {
        if (!firstPass) {
            memMap = memMap + " - ";
        }
        std::string start = std::to_string(holes[i]);
        std::string length = std::to_string(holes[i + 1]);
        memMap = memMap + "[" + start + ", " + length + "]";
        firstPass = false;
    }
    delete[] holes;
    int fd = open(filename, O_WRONLY | O_CREAT, 00700);
    if (write(fd, memMap.c_str(), memMap.length()) == -1){
    return -1;
    }
    else {
        close(fd);
        return 0;
    }
    return 0;
}


void* MemoryManager::getList() {
    if (memory.empty()) {
        return nullptr;
    }
    memory.sort();
    int numHoles = 0;
    int prevAddr = 0;
    std::vector<int> holes;
    if (memory.front().relativeAddr > 0) {
        numHoles++;
        holes.push_back(0);
        holes.push_back(memory.front().relativeAddr);
    }
    bool firstPass = true;
    for (memBlock block : memory) {
        if (block.relativeAddr > prevAddr + 1 && !firstPass) {
            numHoles++;
            holes.push_back(prevAddr + 1);
            holes.push_back(block.relativeAddr - (prevAddr)-1);
        }
        prevAddr = block.endAddr;
        firstPass = false;
    }
    std::list<memBlock>::reverse_iterator rit = memory.rbegin();
    // ++rit;
    if (rit->endAddr < sizeInWords - 1) {
        numHoles++;
        holes.push_back(rit->endAddr + 1);
        holes.push_back(sizeInWords - rit->endAddr - 1);
    }
    //Predefined size from specifications
    int arrSize = 1 + 2 * numHoles;
    uint16_t* arr = new uint16_t[arrSize];
    arr[0] = numHoles;
    int i = 1;
    for (int num : holes) {
        arr[i] = num;
        i++;
    }
    return arr;
}


void* MemoryManager::getBitmap() {
    int size = sizeInWords / 8;
    if (sizeInWords % 8 != 0) {
        size += 1;
    }
    uint8_t* bitMap = new uint8_t[size + 2];
    if (size > 255) {
        bitMap[0] = 255;
        bitMap[1] = size / 255;
        if (size % 255 != 0) {
            bitMap[1] += 1;
        }
    }
    else {
        bitMap[0] = size;
        bitMap[1] = 0;
    }
    std::vector<int> bits;
    memory.sort();
    int prevAddr = 0;
    std::list<memBlock>::reverse_iterator rit;
    bool firstPass = true;
    for (rit = memory.rbegin(); rit != memory.rend(); rit++) {
        //Tests for hole from last block in memory to edge of memory
        if (firstPass) {
            if (rit->endAddr < sizeInWords - 1) {
                for (int i = 0; i < sizeInWords - 1 - rit->endAddr; i++) {
                    bits.push_back(0);
                }
            }
        }
        //test for if there is a hole in between this block and the last block in memory
        if (!firstPass) {
            if (rit->endAddr < prevAddr - 1) {
                for (int i = 0; i < prevAddr - 1 - rit->endAddr; i++) {
                    bits.push_back(0);
                }
            }
        }
        //pushes 1's for each word in this block of memory
        for (int i = 0; i < rit->numWords; i++) {
            bits.push_back(1);
        }
        prevAddr = rit->relativeAddr;
        firstPass = false;

    }
    if (memory.front().relativeAddr > 0) {
        for (int i = 0; i < memory.front().relativeAddr; i++) {
            bits.push_back(0);
        }
    }

    //now bits vector contains the bitMap in little Endian, lets break it into bytes:
    int firstByteSize = bits.size() % 8;
    int nextIndex = 0;
    int* flipped = new int[size];
    if (bits.size() % 8 != 0) {
        std::string byte = "";
        for (int i = 0; i < firstByteSize; i++) {
            std::ostringstream ss;
            ss << bits.at(i);
            byte = byte + ss.str();
        }
        char* cByte = &byte[0];
        char* byteEnd = &byte[byte.length() - 1];
        int fByte = int(std::strtoull(cByte, &byteEnd, 2));
        flipped[nextIndex] = fByte;
        nextIndex++;
    }
    std::string byte = "";
    int count = 0;
    for (int i = firstByteSize; i < bits.size(); i++) {
        std::ostringstream ss;
        ss << bits.at(i);
        byte = byte + ss.str();
        count++;
        if (count == 8) {
            char* cByte = &byte[0];
            char* byteEnd = &byte[byte.length() - 1];
            int fByte = int(std::strtoull(cByte, &byteEnd, 2));
            flipped[nextIndex] = fByte;
            nextIndex++;
            byte = "";
            count = 0;
        }

    }
    for (int i = size; i > 0; i--) {
        bitMap[i + 1] = flipped[size - i];
    }

    return bitMap;
}


unsigned MemoryManager::getWordSize() {
    return this->wordSize;
}


void* MemoryManager::getMemoryStart() {
    return mem_location;
}


unsigned MemoryManager::getMemoryLimit() {
    return wordSize * sizeInWords;
}
int bestFit(int sizeInWords, void* list) {
    if (list == nullptr) {
        return 0;
    }
    uint16_t* holes = static_cast<uint16_t*>(list);
    int arrSize = 1 + holes[0] * 2;
    int index = 0;
    int minDiff = 100000;
    bool holeFound = false;
    for (int i = 2; i < arrSize; i = i + 2) {
        if ((holes[i] - sizeInWords) >= 0) {
            holeFound = true;
            if (minDiff > (holes[i] - sizeInWords)) {
                minDiff = holes[i] - sizeInWords;
                index = i - 1;
            }
        }
    }
    int rval = holes[index];
    if (!holeFound) {
        return -1;
    }
    return rval;
}

int worstFit(int sizeInWords, void* list) {
    if (list == nullptr) {
        return 0;
    }
    uint16_t* holes = static_cast<uint16_t*>(list);
    int arrSize = 1 + holes[0] * 2;
    int index = 0;
    int maxDiff = 0;
    bool holeFound = false;
    for (int i = 2; i < arrSize; i = i + 2) {
        if ((holes[i] - sizeInWords) >= 0) {
            holeFound = true;
            if (maxDiff < (holes[i] - sizeInWords)) {
                maxDiff = holes[i] - sizeInWords;
                index = i - 1;
            }
        }
    }
    int rval = holes[index];
    if (!holeFound) {
        return -1;
    }
    return rval;
}
