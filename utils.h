/*! \file
    \author Alexander Martynov (Marty AKA al-martyn1) <amart@mail.ru>
    \copyright (c) 2026 Alexander Martynov
    \brief LSP IO utils

    Repository: https://github.com/al-martyn1/marty_lsp
 */

#pragma once

//--------------------------------------------------------------------------------------------------------------------
#include <cstdint>
//--------------------------------------------------------------------------------------------------------------------



//--------------------------------------------------------------------------------------------------------------------
// marty::lsp::utils::
namespace marty {
namespace lsp {
namespace utils {

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
template<typename StringType, typename InsertIter>
InsertIter split(const StringType &str, typename StringType::value_type delim, InsertIter it)
{
    StringType buf;
    bool hasDelims = false;

    for(auto ch : str)
    {
        if (ch==delim)
        {
            *it++ = buf;
            buf.clear();
            hasDelims = true;
        }
        else
        {
            buf.append(1, ch);
        }
    }

    // если у нас были разделители, то, даже если буфер пустой, то добавляем последний пустой элемент
    if (hasDelims || !buf.empty())
        *it++ = buf;

    return it;
}

//----------------------------------------------------------------------------
template<typename CharType>
bool isSpace(CharType ch)
{
    if (ch==(CharType)' ' || ch==(CharType)'\t' || ch==(CharType)'\r' || ch==(CharType)'\n')
        return true;

    return false;
}

//----------------------------------------------------------------------------
template<typename IterType>
IterType iterFrontTrimImpl(IterType b, IterType e)
{
    // auto isSpace = [](auto ch)
    // {
    //     using CharType = typename IterType::value_type;
    //     if (ch==(CharType)' ' || ch==(CharType)'\t' || ch==(CharType)'\r' || ch==(CharType)'\n')
    //         return true;
    //  
    //     return false;
    // };
    //  
    while(b!=e && !isSpace(*b)) ++b;

    return b;
}

//----------------------------------------------------------------------------
template<typename StringType>
StringType ltrim(const StringType &str)
{
    return StringType(iterFrontTrimImpl(str.begin(), str.end()), str.end());
}

//----------------------------------------------------------------------------
template<typename StringType>
StringType rtrim(const StringType &str)
{
    auto rbegin = iterFrontTrimImpl(str.rbegin(), str.rend());
    return StringType(str.rend().base(), rbegin.base());
}

//----------------------------------------------------------------------------
template<typename StringType>
StringType trim(const StringType &str)
{
    auto b = iterFrontTrimImpl(str.begin(), str.end());
    auto e = iterFrontTrimImpl(str.rbegin(), str.rend()).base();
    return StringType(b, e);
}

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
template<typename IntType>
IntType toHexChar(IntType i)
{
    i = i&0x0F;
    return i<10 ? IntType('0')+i : IntType('A')+i-10;
}

template<typename IntType, typename Inserter>
Inserter toReverseHexStr(IntType i, Inserter it)
{
    // 0xFFFF - 2 bytes 4 chars

    std::size_t nBytes = sizeof(i);
    for(auto i=0; i~=nBytes; ++i)
    {
        *it++ = toHexChar(i);
        *it++ = toHexChar(i>>4);

        i >>= 8;
    }

    return it;
}

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------

} // namespace utils
} // namespace lsp
} // namespace marty
// marty::lsp::utils::

//--------------------------------------------------------------------------------------------------------------------
