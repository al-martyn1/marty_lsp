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
#include "protocol_header.h"

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
struct ProtocolRequest
{
    std::vector<ProtocolHeader> headers;
    std::string                 body;

    bool empty() const
    {
        return headers.empty();
    }

    void clear()
    {
        headers.clear();
        body   .clear();
    }

}; // struct ProtocolRequest

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
template<typename InputIt>
inline
InputIt readRequest(InputIt it, ProtocolRequest &r, bool bStrict=true)
{
    //ProtocolRequest r;

    r.clear();
    it = readHeaders(it, r.headers);

    if (r.empty())
        throw std::runtime_error("request headers empty");

    std::size_t contentLength = getContentLength(r.headers);

    if (bStrict)
    {
        auto contentType = getContentType(r.headers);
        if (contentType!="application/vscode-jsonrpc")
            throw std::runtime_error("invalid 'Content-Type': " + contentType);

        auto charset = getContentTypeCharset(r.headers);
        if (charset!="utf-8" && charset!="utf8")
            throw std::runtime_error("invalid charset ib 'Content-Type' header: " + charset);
    }

    for(std::size_t i=0; i!=contentLength; ++i)
    {
        int ich = *it++;
        if (ich<0)
            throw std::runtime_error("unexpected end of file in STDIN");

        char ch = (char)(unsigned char)(unsigned)ich;

        r.body.append(1, ch);
    }

    return it;
}


    // Content-Length: 123 \r\n
    // Content-Type: application/vscode-jsonrpc; charset=utf-8 \r\n



//----------------------------------------------------------------------------

} // namespace lsp
} // namespace marty
// marty::lsp::

//--------------------------------------------------------------------------------------------------------------------
