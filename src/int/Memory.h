//////////////////////////////////////////////////////////////////////////////
// Memory.h  (there is NO correspondent Memory.cpp)
//
// Author:
//      Dmitry ("Leo") Kuznetsov
// Revision History
//      Jan 18, 1998 - originated
//      Jan 18, 2001 - moved out inline functions bodies (VC6.0 bugs)

#ifndef _MEMORY_H
#define _MEMORY_H

#include <cstdint>

#define K (1024)

class IGD480;

enum
{
    IGD480base   = 0x1F0000,    // 8MB
    IGD480offset = 0x008000,    // bitmap offset
    IGD480bitmap = 0x1F8000,    // bitmap base
    IGD480size   = 512 * 512 / 8
};


class MEMORY
{
public: // IGD480 starts at 8MB. Don't make MemorySize 8MB!
    MEMORY(int nMemorySizeBytes);
    virtual ~MEMORY();

    inline int GetSize() const { return nMemorySize; }

    private: class reference; public:

    inline
    reference operator[](int n);
                                        
    inline
    bool OutOfRange() 
    { 
        bool bWasOutOfRange = bOutOfRange; 
        bOutOfRange = false; 
        return bWasOutOfRange; 
    }

private:
    class reference // see notes below
    {
        public:
            inline reference(int* ptr, MEMORY* m);
            inline void operator=(int i);
            inline void operator=(const reference& source);
            inline operator int() const;
            // addressing as in &mem[i]
            inline uint8_t* operator&() const;
        private:
            int* p;
            MEMORY& mem;

    };
    friend class reference;
    friend class IGD480;
    int* data;
    int  nMemorySize;
    bool bOutOfRange;
};


inline MEMORY::reference::reference(int* ptr, MEMORY* m) : 
    mem(*m),
    p(ptr)
{
}


inline
MEMORY::reference MEMORY::operator[](int n)
{
    n &= ~0xC0000000; // clear bits {31,30} Kronos feature
    if (n >= 0 && n < nMemorySize)
        return reference(&data[n], this);
    else if (n >= IGD480base && n <= IGD480base + IGD480offset + IGD480size)
    {
        return reference(&data[n], this);  // IGD address
    }
    else
    {
//      trace("MEMORY.OutOfRange 0x%08x\n", n);
        bOutOfRange = true;
        return reference(nullptr, this);
    }
}


inline void MEMORY::reference::operator=(int i)
{
    if (p == nullptr)
        return;
    *p = i;
}


inline void MEMORY::reference::operator=(const MEMORY::reference& source)
{
    if (p == nullptr)
        return;
    *p = int(source);
}


inline MEMORY::reference::operator int() const
{
    if (p != nullptr)
        return *p; 
    else
        return 0;
}


// addressing as in &mem[i]
inline uint8_t* MEMORY::reference::operator&() const
{
    return (uint8_t*)p;
}

#endif //_MEMORY_H

