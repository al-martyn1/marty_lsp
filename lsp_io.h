/*! \file
    \author Alexander Martynov (Marty AKA al-martyn1) <amart@mail.ru>
    \copyright (c) 2026 Alexander Martynov
    \brief LSP IO utils

    Repository: https://github.com/al-martyn1/marty_lsp
 */

#pragma once

//--------------------------------------------------------------------------------------------------------------------



//----------------------------------------------------------------------------
#if defined(_WIN32) || defined(WIN32)

    #include <conio.h>
    #include <fcntl.h>
    #include <io.h>
    #include <stdio.h>

    #ifndef _O_BINARY
        #define _O_BINARY O_BINARY
    #endif

    #ifndef _O_TEXT
        #define _O_TEXT O_TEXT
    #endif

#elif defined(__linux__) || defined(__linux) || defined(__unux__) || defined(__unux)

    #include <termios.h>
    #include <unistd.h>
    #include <stdio.h>

#endif

#include <string>
#include <iostream>
#include <iomanip>

//----------------------------------------------------------------------------



//--------------------------------------------------------------------------------------------------------------------
// marty::lsp::io::
namespace marty {
namespace lsp {
namespace io {

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
// Глобально меняет параметры потока STDIN
// Не следует использовать поток STDIN до использования данного класса
struct StdinCharReader
{

    

#if defined(__linux__) || defined(__linux) || defined(__unux__) || defined(__unux)
protected:
    struct termios oldt;
public:
#endif

    StdinCharReader()
    {
#if defined(_WIN32) || defined(WIN32)
    #if defined(_MSC_VER)
        _setmode(_fileno(stdin), _O_BINARY);
    #else
        setmode(fileno(stdin), _O_BINARY);
    #endif
#elif defined(__linux__) || defined(__linux) || defined(__unux__) || defined(__unux)
        struct termios newt = oldt;
        newt.c_lflag &= ~(ICANON | ECHO); // отключаем канонический режим, отключаем эхо
        tcsetattr(STDIN_FILENO, TCSANOW, &newt);
#endif
    }

    ~StdinCharReader()
    {
#if defined(__linux__) || defined(__linux) || defined(__unux__) || defined(__unux)
        tcsetattr(STDIN_FILENO, TCSANOW, &oldt); // восстанавливаем настройки
#endif
    }

    StdinCharReader(const StdinCharReader &) = delete;
    StdinCharReader& operator=(const StdinCharReader &) = delete;
    StdinCharReader(StdinCharReader &&) = delete;
    StdinCharReader& operator=(StdinCharReader &&) = delete;

    //static
    int getChar() const
    {
        return getchar();
    }


    struct InputIterator
    {
        const StdinCharReader *pReader;

        InputIterator(const StdinCharReader &r) : pReader(&r) {}
        InputIterator(const InputIterator &) = default;
        InputIterator(InputIterator &&) = default;
        InputIterator& operator=(const InputIterator &) = default;
        InputIterator& operator=(InputIterator &&) = default;

        InputIterator operator++() // prefix
        {
            return *this;
        }

        InputIterator operator++(int) // postfix
        {
            return *this;
        }

        int operator*()
        {
            return pReader->getChar();
        }

    }; // struct InputIterator


    InputIterator inputIterator() const
    {
        return InputIterator(*this);
    }


}; // struct StdinCharReader

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
template<typename StringView>
struct StringViewInputIterator
{
    using traits_type            = typename StringView::traits_type           ;
    using value_type             = typename StringView::value_type            ;
    using pointer                = typename StringView::pointer               ;
    using const_pointer          = typename StringView::const_pointer         ;
    using reference              = typename StringView::reference             ;
    using const_reference        = typename StringView::const_reference       ;
    using const_iterator         = typename StringView::const_iterator        ;
    using iterator               = typename StringView::iterator              ;
    using const_reverse_iterator = typename StringView::const_reverse_iterator;
    using reverse_iterator       = typename StringView::reverse_iterator      ;
    using size_type              = typename StringView::size_type             ;
    using difference_type        = typename StringView::difference_type       ;


protected:

    StringView       sv;
    const_iterator   it;


public:

    StringViewInputIterator() = delete;
    StringViewInputIterator(StringView sv_) : sv(sv_), it(sv.cbegin()) {}
    StringViewInputIterator(const StringViewInputIterator &) = default;
    StringViewInputIterator& operator=(const StringViewInputIterator &) = default;
    StringViewInputIterator(StringViewInputIterator &&) = default;
    StringViewInputIterator& operator=(StringViewInputIterator &&) = default;

    StringViewInputIterator operator++() // prefix
    {
        ++it;
        return *this;
    }

    StringViewInputIterator operator++(int) // postfix
    {
        auto res = *this;
        ++it;
        return res;
    }

    int operator*()
    {
        if (it==sv.end())
            return -1;

        return int(*it);
    }

}; // struct StringViewInputIterator

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
// Глобально меняет параметры потока STDOUT
// Не следует использовать поток STDOUT до использования данной функции
inline
void setStdoutBinaryMode()
{
#if defined(_WIN32) || defined(WIN32)
    #if defined(_MSC_VER)
        _setmode(_fileno(stdout), _O_BINARY);
    #else
        setmode(fileno(stdout), _O_BINARY);
    #endif
#endif

}

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
inline
void serverWriteMessage(const std::string &messageText)
{
    std::cout << "Content-Length: " << messageText.size() << "\r\n\r\n";
    std::cout << messageText << std::flush; // После данных ничего записывать не нужно, никаких переводов строки и тп
}


//----------------------------------------------------------------------------

} // namespace io
} // namespace lsp
} // namespace marty
// marty::lsp::io::

//--------------------------------------------------------------------------------------------------------------------
