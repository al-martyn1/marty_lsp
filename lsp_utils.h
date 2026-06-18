/*! \file
    \author Alexander Martynov (Marty AKA al-martyn1) <amart@mail.ru>
    \copyright (c) 2026 Alexander Martynov
    \brief LSP utilities: URI/path conversion, workspace helpers

    Repository: https://github.com/al-martyn1/marty_lsp
 */

#pragma once

//--------------------------------------------------------------------------------------------------------------------
#include <string>
#include <vector>
#include <optional>
#include <regex>
#include <stdexcept>
#include <cstdlib>
#include <cctype>
#include <algorithm>

#if defined(_WIN32) || defined(WIN32)
    #include <windows.h>
#else
    #include <unistd.h>
    #include <limits.h>
#endif

#include "basics.h"
#include "lsp_types.h"

//--------------------------------------------------------------------------------------------------------------------

namespace marty {
namespace lsp {
namespace utils {

//--------------------------------------------------------------------------------------------------------------------

//--------------------------------------------------
// Вспомогательные функции для URL-кодирования/декодирования
//--------------------------------------------------
inline bool isUnreserved(char c)
{
    // Разрешённые символы: буквы, цифры, - _ . ~
    return (c >= 'A' && c <= 'Z') ||
           (c >= 'a' && c <= 'z') ||
           (c >= '0' && c <= '9') ||
           c == '-' || c == '_' || c == '.' || c == '~';
}

//--------------------------------------------------
inline std::string percentEncode(unsigned char c)
{
    static const char hex[] = "0123456789ABCDEF";
    std::string result = "%";
    result.push_back(hex[c >> 4]);
    result.push_back(hex[c & 0xF]);
    return result;
}

//--------------------------------------------------
inline std::string urlEncode(const std::string& s)
{
    std::string result;
    result.reserve(s.size() * 3);
    for (unsigned char c : s)
    {
        if (isUnreserved(static_cast<char>(c)))
            result.push_back(static_cast<char>(c));
        else
            result += percentEncode(c);
    }
    return result;
}

//--------------------------------------------------
inline std::string urlDecode(const std::string& s)
{
    std::string result;
    result.reserve(s.size());
    for (size_t i = 0; i < s.size(); ++i)
    {
        if (s[i] == '%' && i + 2 < s.size())
        {
            unsigned int value;
            std::string hex = s.substr(i + 1, 2);
            if (sscanf(hex.c_str(), "%02X", &value) == 1)
            {
                result.push_back(static_cast<char>(value));
                i += 2;
                continue;
            }
        }
        result.push_back(s[i]);
    }
    return result;
}

//--------------------------------------------------------------------------------------------------------------------

//--------------------------------------------------
// Преобразование пути к URI (file:// схема)
//--------------------------------------------------
inline std::string pathToUri(const std::string& path)
{
    if (path.empty())
        throw std::invalid_argument("pathToUri: empty path");

    std::string p = path;
#if defined(_WIN32) || defined(WIN32)
    // Заменяем обратные слеши на прямые для URI
    std::replace(p.begin(), p.end(), '\\', '/');

    // Проверяем, является ли путь UNC (начинается с двух слешей)
    if (p.size() >= 2 && p[0] == '/' && p[1] == '/')
    {
        // UNC: //server/share/path -> file://server/share/path
        // Убираем начальные "//", оставляем server/share/path
        std::string unc = p.substr(2);
        // Кодируем всё, что не является зарезервированными символами, но не кодируем слеши
        // Мы закодируем только части, но для простоты закодируем всё, кроме '/'?
        // В URI можно оставить слеши как есть.
        // Просто кодируем каждый сегмент, но слеши оставляем.
        std::string encoded;
        size_t pos = 0;
        while (pos < unc.size())
        {
            size_t nextSlash = unc.find('/', pos);
            if (nextSlash == std::string::npos)
            {
                encoded += urlEncode(unc.substr(pos));
                break;
            }
            else
            {
                encoded += urlEncode(unc.substr(pos, nextSlash - pos));
                encoded += '/';
                pos = nextSlash + 1;
            }
        }
        return "file://" + encoded;
    }
    else
    {
        // Проверяем, начинается ли путь с буквы диска и двоеточия (например, C:/)
        if (p.size() >= 2 && isalpha(p[0]) && p[1] == ':')
        {
            // Если после двоеточия нет слеша, добавляем его (например, C:file -> C:/file)
            if (p.size() == 2)
                p += '/';
            else if (p[2] != '/')
                p.insert(p.begin() + 2, '/');

            // Для дисков: file:///C:/path
            return "file:///" + urlEncode(p);
        }
        else if (p.size() >= 1 && p[0] == '/')
        {
            // Абсолютный путь без буквы диска: /path (относительно текущего диска)
            // В LSP такие пути не рекомендуются, но можно преобразовать в file:///C:/path,
            // но мы не знаем текущий диск. Выбросим исключение.
            throw std::invalid_argument("pathToUri: path without drive letter not supported on Windows");
        }
        else
        {
            throw std::invalid_argument("pathToUri: invalid Windows path format");
        }
    }
#else
    // Unix: путь должен начинаться с '/'
    if (p[0] != '/')
        throw std::invalid_argument("pathToUri: path must be absolute (start with '/')");

    // Для Unix просто file:///path (но у нас file:// + /path)
    return "file://" + urlEncode(p);
#endif
}

//--------------------------------------------------
// Преобразование URI в путь (file://)
//--------------------------------------------------
inline std::string uriToPath(const std::string& uri)
{
    const std::string prefix = "file://";
    if (uri.compare(0, prefix.size(), prefix) != 0)
        throw std::invalid_argument("uriToPath: URI does not start with file://");

    std::string rest = uri.substr(prefix.size());

#if defined(_WIN32) || defined(WIN32)
    // Проверяем, является ли URI UNC (содержит сервер после file://)
    // Форма: file://server/share/path
    if (rest.size() >= 1 && rest[0] != '/')
    {
        // Это UNC: server/share/path
        // Декодируем части, но не слеши
        std::string decoded;
        size_t pos = 0;
        while (pos < rest.size())
        {
            size_t nextSlash = rest.find('/', pos);
            if (nextSlash == std::string::npos)
            {
                decoded += urlDecode(rest.substr(pos));
                break;
            }
            else
            {
                decoded += urlDecode(rest.substr(pos, nextSlash - pos));
                decoded += '\\';
                pos = nextSlash + 1;
            }
        }
        // Восстанавливаем UNC: \\server\share\path
        return "\\\\" + decoded;
    }
    else
    {
        // Форма: file:///C:/path (или file:///path)
        if (rest.size() >= 1 && rest[0] == '/')
        {
            // Убираем ведущий слеш
            rest = rest.substr(1);
        }

        // Проверяем, начинается ли с диска
        if (rest.size() >= 2 && isalpha(rest[0]) && rest[1] == ':')
        {
            // Преобразуем прямые слеши в обратные
            std::replace(rest.begin(), rest.end(), '/', '\\');
            return rest;
        }
        else
        {
            // Путь без диска – не поддерживается
            throw std::invalid_argument("uriToPath: URI without drive letter not supported on Windows");
        }
    }
#else
    // Unix: rest должен начинаться с '/'
    if (rest.empty() || rest[0] != '/')
        throw std::invalid_argument("uriToPath: invalid file URI format for Unix");

    return urlDecode(rest);
#endif
}

//--------------------------------------------------------------------------------------------------------------------

//--------------------------------------------------
// Получить корневую папку из параметров инициализации (URI)
//--------------------------------------------------
inline std::string getRootUriFromParams(const InitializeParams& params)
{
    if (params.rootUri && !params.rootUri->empty())
        return *params.rootUri;
    else if (params.rootPath && !params.rootPath->empty())
        return pathToUri(*params.rootPath);
    else
        throw std::runtime_error("No rootUri or rootPath provided");
}

//--------------------------------------------------
// Создать или скорректировать workspaceFolders из параметров
//--------------------------------------------------
inline std::vector<WorkspaceFolder> ensureWorkspaceFolders(const InitializeParams& params)
{
    // Если workspaceFolders присутствует (даже пустой) — используем его
    if (params.workspaceFolders)
        return *params.workspaceFolders;

    // Иначе создаём из rootUri/rootPath
    std::string uri = getRootUriFromParams(params);

    WorkspaceFolder wf;
    wf.uri = uri;
    size_t lastSlash = uri.find_last_of('/');
    if (lastSlash != std::string::npos && lastSlash + 1 < uri.size())
        wf.name = uri.substr(lastSlash + 1);
    else
        wf.name = "root";

    return std::vector<WorkspaceFolder>{wf};
}

//--------------------------------------------------------------------------------------------------------------------

} // namespace utils
} // namespace lsp
} // namespace marty

//--------------------------------------------------------------------------------------------------------------------
