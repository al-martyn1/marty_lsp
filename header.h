/*! \file
    \author Alexander Martynov (Marty AKA al-martyn1) <amart@mail.ru>
    \copyright (c) 2026 Alexander Martynov
    \brief LSP IO utils

    Repository: https://github.com/al-martyn1/marty_lsp
 */

#pragma once

//--------------------------------------------------------------------------------------------------------------------
//#include "lsp_io.h"
#include "utils.h"

//
#include <algorithm>
#include <iterator>
#include <string>

//--------------------------------------------------------------------------------------------------------------------



//--------------------------------------------------------------------------------------------------------------------
// marty::lsp::
namespace marty {
namespace lsp {

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
// Content-Length: 123 \r\n
// Content-Type: application/vscode-jsonrpc; charset=utf-8 \r\n
// \r\n

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
struct Header
{
    std::string    name;
    std::string    text;

    bool empty() const { return name.empty(); }
    void clear()
    {
        name.clear();
        text.clear();
    }

    template<typename InputIt>
    static
    InputIt read(InputIt it, Header &hdr)
    {

        auto chToHexStr = [](auto ch) -> std::string
        {
            std::string resStr;
            utils::toReverseHexStr(ch, std::back_inserter(resStr));
            std::reverse(resStr.begin(), resStr.end());
            return "0x" + resStr;
        };

        auto mkChStr = [&](char ch) -> std::string
        {
            if ((ch>=0 && ch<=' ') || ch==0x7F)
                return chToHexStr((unsigned char)ch);
            return "'" + std::string(1, ch) + "'";
        };


        enum State
        {
            stWaitName,
            stReadName,
            stWaitSep , // ожидаем разделитель после имени заголовка
            stWaitText,
            stReadText,
            stWaitLf
        };


        auto st = stWaitName;

        hdr.clear();

        int ich = *it++;

        for(; ich>=0; ich=*it++)
        {
            char ch = (char)(unsigned char)(unsigned)ich;

            switch(st)
            {
                case stWaitName:
                {
                    if (ch=='\r')
                        st = stWaitLf;

                    else if (ch=='\n')
                        throw std::runtime_error("unexpected LF in input stream while waiting header name");

                    else if (utils::isSpace(ch))
                        continue;

                    else
                    {
                        hdr.name.append(1, ch);
                        st = stReadName;
                    }

                    break;
                }

                case stReadName:
                {
                    if (ch=='\r')
                        throw std::runtime_error("unexpected CR in input stream while reading header name");

                    else if (ch=='\n')
                        throw std::runtime_error("unexpected LF in input stream while reading header name");

                    else if (!utils::isSpace(ch))
                        hdr.name.append(1, ch);

                    else
                        st = stWaitSep;
                
                    break;
                }

                case stWaitSep:
                {
                    if (ch=='\r')
                        throw std::runtime_error("unexpected CR in input stream while waiting for header/text separator");

                    else if (ch=='\n')
                        throw std::runtime_error("unexpected LF in input stream while waiting for header/text separator");

                    else if (utils::isSpace(ch))
                        continue;

                    else if (ch==':')
                        st = stWaitText;

                    else
                        throw std::runtime_error("unexpected char while reading header '" + hdr.name + "': " + mkChStr(ch) + "");
                
                    break;
                }

                case stWaitText:
                {
                    if (ch=='\r')
                        st = stWaitLf;

                    else if (ch=='\n')
                        throw std::runtime_error("unexpected LF in input stream while waiting header text");

                    else if (utils::isSpace(ch))
                        continue;

                    else
                    {
                        hdr.text.append(1, ch);
                        st = stReadText;
                    }

                    break;
                }

                case stReadText:
                {
                    if (ch=='\r')
                        st = stWaitLf;

                    else if (ch=='\n')
                        throw std::runtime_error("unexpected LF in input stream while reading header text");

                    else
                    {
                        hdr.text.append(1, ch);
                    }

                    break;
                }

                case stWaitLf:
                {
                    if (ch!='\n')
                        throw std::runtime_error("unexpected char (" + mkChStr(ch) + ") in input stream while waiting LF after CR");

                    hdr.text = utils::rtrim(hdr.text);

                    return it;
                };
            };
        }

        if (ich<0)
            throw std::runtime_error("unexpected end of file in input stream");

        return it;    
    }


}; // struct Header

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------

} // namespace lsp
} // namespace marty
// marty::lsp::

//--------------------------------------------------------------------------------------------------------------------
