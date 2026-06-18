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
#include <stdexcept>
#include <vector>

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
struct ProtocolHeader
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
    InputIt read(InputIt it, ProtocolHeader &hdr)
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

                    else if (ch==':')
                        st = stWaitText;

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


}; // struct ProtocolHeader

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
// У нас LSP-сервер - единственный клиент на STDIN, поэтому блокирующее чтение - норм
template<typename InputIt>
inline
InputIt readHeaders(InputIt it, std::vector<ProtocolHeader> &headers)
{
    headers.clear();

    while(true)
    {
        ProtocolHeader hdr;
        it = ProtocolHeader::read(it, hdr);
        if (hdr.empty())
        {
            if (headers.empty())
                throw std::runtime_error("missing headers");
            return it;
        }

        headers.push_back(hdr);
    }
}

//----------------------------------------------------------------------------
inline
std::size_t findHeader(const std::vector<ProtocolHeader> &headers, std::string hdrName)
{
    hdrName = utils::tolower_copy(hdrName);

    for(std::size_t i=0; i!=headers.size(); ++i)
    {
        if (utils::tolower_copy(headers[i].name)==hdrName)
            return i;
    }

    return std::size_t(-1);
}

//----------------------------------------------------------------------------
inline
std::size_t getContentLength(const std::vector<ProtocolHeader> &headers)
{
    std::size_t idx = findHeader(headers, "Content-Length");
    if (idx==std::size_t(-1))
        throw std::runtime_error("missing mandatory header 'Content-Length'");

    try
    {
        std::size_t res = (std::size_t)std::stoull(headers[idx].text);
        return res;
    }
    catch(const std::invalid_argument &)
    {
        throw std::runtime_error("invalid value in 'Content-Length' header: '" + headers[idx].text + "'");
    }
    catch(const std::out_of_range &)
    {
        throw std::runtime_error("value in 'Content-Length' header is too big");
    }
    
}

//----------------------------------------------------------------------------
inline
std::string getContentType(const std::vector<ProtocolHeader> &headers, const std::string &defType="application/vscode-jsonrpc")
{
    std::size_t idx = findHeader(headers, "Content-Type");
    if (idx==std::size_t(-1))
        return defType;

    std::vector<std::string> parts;
    utils::split(headers[idx].text, ';', std::back_inserter(parts));
    if (parts.empty())
        return defType;

    for(auto &p: parts)
        p = utils::trim(p);

    if (parts[0].empty())
        return defType;

    return parts[0];
}

//----------------------------------------------------------------------------
inline
std::string getContentTypeCharset(const std::vector<ProtocolHeader> &headers, const std::string &defCharset="utf-8")
{
    std::size_t idx = findHeader(headers, "Content-Type");
    if (idx==std::size_t(-1))
        return defCharset;

    std::vector<std::string> parts;
    utils::split(headers[idx].text, ';', std::back_inserter(parts));

    if (parts.size()<2)
        return defCharset;

    //ProtocolHeader::trim(parts[1]);
    parts[1] = utils::trim(parts[1]);
    if (parts[1].empty())
        return defCharset;

    parts[1] = utils::tolower_copy(parts[1]);

    std::vector<std::string> charsetParts;
    utils::split(parts[1], '=', std::back_inserter(charsetParts));
    // auto charsetParts = headers[idx].split(parts[1], '=');
    // headers[idx].trim(charsetParts);
    if (charsetParts.size()<2)
        return defCharset;

    if (charsetParts[0]!="charset")
        return defCharset;

    if (charsetParts[1].empty())
        return defCharset;

    return charsetParts[1];
}

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------

} // namespace lsp
} // namespace marty
// marty::lsp::

//--------------------------------------------------------------------------------------------------------------------
