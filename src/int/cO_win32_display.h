#if !defined(CO_WIN32_DISPLAY_H)
#define CO_WIN32_DISPLAY_H

class cO_win32_display
{
public:
    virtual void write(char *ptr, int bytes);
    virtual void writeChar(char ch);

    cO_win32_display();
    virtual ~cO_win32_display();

private:
    short H;
    short W;
    char  szEsc[256];
    int   nEscCount;

    int Number(char *p);
    void EraseLine();
    void Erase();
    void EraseChars(int n);
    void InsertChars(int n);
    void DeleteChars(int n);
    void InsertLines(int n);
    void DeleteLines(int n);
};

#endif //CO_WIN32_DISPLAY_H
