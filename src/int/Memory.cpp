#include <cassert>

#include "Memory.h"


MEMORY::MEMORY(int nMemorySizeBytes) :
    data(nullptr),
    bOutOfRange(false)
{
    nMemorySize = (nMemorySizeBytes + 3) / 4;
    
    assert(nMemorySize < IGD480bitmap + IGD480size);
    int nSizeWithIGD = IGD480bitmap + IGD480size;

    // allocate none commited memory
    uint8_t* pReservered = nullptr;
    pReservered = (uint8_t*)::VirtualAlloc(nullptr, nSizeWithIGD * 4, MEM_RESERVE, PAGE_READWRITE);

    data = (int*)::VirtualAlloc(pReservered, nMemorySize*4, MEM_COMMIT,  PAGE_READWRITE);
    assert(pReservered == (uint8_t*)data);
//  trace("Memory: %08x\n", data);

    void* pIGDregisters = ::VirtualAlloc(pReservered + IGD480base*4, 4*K, MEM_COMMIT,  PAGE_READWRITE);
    assert(pIGDregisters == pReservered  + IGD480base*4);
    (void)pIGDregisters;

    void* pIGDbitmap = ::VirtualAlloc(pReservered + IGD480bitmap*4, IGD480size * 4, MEM_COMMIT,  PAGE_READWRITE);
    assert(pIGDbitmap == pReservered  + IGD480bitmap*4);
    (void)pIGDbitmap;
}


MEMORY::~MEMORY()
{
    // we do not necesseraly need VirtualFree(data) here
    if (data != nullptr)
        ::VirtualFree(data, 0, MEM_RELEASE);
    data = nullptr;
}

