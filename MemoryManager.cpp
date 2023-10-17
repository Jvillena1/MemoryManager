#include "MemoryManager.h"

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
