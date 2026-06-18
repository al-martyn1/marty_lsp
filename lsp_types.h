/*! \file
    \author Alexander Martynov (Marty AKA al-martyn1) <amart@mail.ru>
    \copyright (c) 2026 Alexander Martynov
    \brief LSP data types (LSP 3.17)

    Repository: https://github.com/al-martyn1/marty_lsp
 */

#pragma once

//--------------------------------------------------------------------------------------------------------------------
#include "nlohmann/json.hpp"
#include "basics.h"
#include "lsp_enums.h"
#include "utils.h"

#include <cstdint>
#include <optional>
#include <string>
#include <vector>
#include <variant>
#include <map>
#include <unordered_map>
#include <functional>

//--------------------------------------------------------------------------------------------------------------------

namespace marty {
namespace lsp {

//--------------------------------------------------------------------------------------------------------------------

//--------------------------------------------------
// MessageId - может быть числом, строкой или отсутствовать
//--------------------------------------------------
struct MessageId
{
    using Variant = std::variant<std::monostate, int64_t, std::string>;

    Variant value;

    MessageId() : value(std::monostate{}) {}
    MessageId(int64_t id) : value(id) {}
    MessageId(const std::string& id) : value(id) {}
    MessageId(const char* id) : value(std::string(id)) {}

    bool empty() const { return std::holds_alternative<std::monostate>(value); }

    std::string toString() const
    {
        if (std::holds_alternative<int64_t>(value))
            return std::to_string(std::get<int64_t>(value));
        else if (std::holds_alternative<std::string>(value))
            return std::get<std::string>(value);
        else
            return "";
    }

    bool operator==(const MessageId& other) const { return value == other.value; }
    bool operator!=(const MessageId& other) const { return !(*this == other); }
};

//--------------------------------------------------
inline void to_json(json& j, const MessageId& id)
{
    if (std::holds_alternative<int64_t>(id.value))
        j = std::get<int64_t>(id.value);
    else if (std::holds_alternative<std::string>(id.value))
        j = std::get<std::string>(id.value);
    else
        j = nullptr; // или не добавлять? но лучше null
}

//--------------------------------------------------
inline void from_json(const json& j, MessageId& id)
{
    if (j.is_number_integer())
        id.value = j.get<int64_t>();
    else if (j.is_string())
        id.value = j.get<std::string>();
    else
        id.value = std::monostate{};
}

//--------------------------------------------------------------------------------------------------------------------

//--------------------------------------------------
// Position
//--------------------------------------------------
struct Position
{
    uint32_t line = 0;
    uint32_t character = 0;
};

//--------------------------------------------------
inline void to_json(json& j, const Position& p)
{
    j = json{
        {"line", p.line},
        {"character", p.character}
    };
}

//--------------------------------------------------
inline void from_json(const json& j, Position& p)
{
    j.at("line").get_to(p.line);
    j.at("character").get_to(p.character);
}

//--------------------------------------------------------------------------------------------------------------------

//--------------------------------------------------
// Range
//--------------------------------------------------
struct Range
{
    Position start;
    Position end;
};

//--------------------------------------------------
inline void to_json(json& j, const Range& r)
{
    j = json{
        {"start", r.start},
        {"end", r.end}
    };
}

//--------------------------------------------------
inline void from_json(const json& j, Range& r)
{
    j.at("start").get_to(r.start);
    j.at("end").get_to(r.end);
}

//--------------------------------------------------------------------------------------------------------------------

//--------------------------------------------------
// Location
//--------------------------------------------------
struct Location
{
    std::string uri;
    Range range;
};

//--------------------------------------------------
inline void to_json(json& j, const Location& loc)
{
    j = json{
        {"uri", loc.uri},
        {"range", loc.range}
    };
}

//--------------------------------------------------
inline void from_json(const json& j, Location& loc)
{
    j.at("uri").get_to(loc.uri);
    j.at("range").get_to(loc.range);
}

//--------------------------------------------------------------------------------------------------------------------

//--------------------------------------------------
// TextDocumentIdentifier
//--------------------------------------------------
struct TextDocumentIdentifier
{
    std::string uri;
};

//--------------------------------------------------
inline void to_json(json& j, const TextDocumentIdentifier& id)
{
    j = json{{"uri", id.uri}};
}

//--------------------------------------------------
inline void from_json(const json& j, TextDocumentIdentifier& id)
{
    j.at("uri").get_to(id.uri);
}

//--------------------------------------------------------------------------------------------------------------------

//--------------------------------------------------
// VersionedTextDocumentIdentifier
//--------------------------------------------------
struct VersionedTextDocumentIdentifier : public TextDocumentIdentifier
{
    int64_t version = 0;
};

//--------------------------------------------------
inline void to_json(json& j, const VersionedTextDocumentIdentifier& id)
{
    j = json{
        {"uri", id.uri},
        {"version", id.version}
    };
}

//--------------------------------------------------
inline void from_json(const json& j, VersionedTextDocumentIdentifier& id)
{
    j.at("uri").get_to(id.uri);
    j.at("version").get_to(id.version);
}

//--------------------------------------------------------------------------------------------------------------------

//--------------------------------------------------
// TextDocumentItem
//--------------------------------------------------
struct TextDocumentItem
{
    std::string uri;
    std::string languageId;
    int64_t version = 0;
    std::string text;
};

//--------------------------------------------------
inline void to_json(json& j, const TextDocumentItem& item)
{
    j = json{
        {"uri", item.uri},
        {"languageId", item.languageId},
        {"version", item.version},
        {"text", item.text}
    };
}

//--------------------------------------------------
inline void from_json(const json& j, TextDocumentItem& item)
{
    j.at("uri").get_to(item.uri);
    j.at("languageId").get_to(item.languageId);
    j.at("version").get_to(item.version);
    j.at("text").get_to(item.text);
}

//--------------------------------------------------------------------------------------------------------------------

//--------------------------------------------------
// TextDocumentPositionParams
//--------------------------------------------------
struct TextDocumentPositionParams
{
    TextDocumentIdentifier textDocument;
    Position position;
};

//--------------------------------------------------
inline void to_json(json& j, const TextDocumentPositionParams& p)
{
    j = json{
        {"textDocument", p.textDocument},
        {"position", p.position}
    };
}

//--------------------------------------------------
inline void from_json(const json& j, TextDocumentPositionParams& p)
{
    j.at("textDocument").get_to(p.textDocument);
    j.at("position").get_to(p.position);
}

//--------------------------------------------------------------------------------------------------------------------

//--------------------------------------------------
// CompletionContext
//--------------------------------------------------
enum class CompletionTriggerKind : int; // уже определён в lsp_enums.h

struct CompletionContext
{
    CompletionTriggerKind triggerKind = CompletionTriggerKind::invoked;
    std::optional<std::string> triggerCharacter;
};

//--------------------------------------------------
inline void to_json(json& j, const CompletionContext& ctx)
{
    j = json{{"triggerKind", ctx.triggerKind}};
    if (ctx.triggerCharacter)
        j["triggerCharacter"] = *ctx.triggerCharacter;
}

//--------------------------------------------------
inline void from_json(const json& j, CompletionContext& ctx)
{
    ctx.triggerKind = j.at("triggerKind").get<CompletionTriggerKind>();
    if (j.contains("triggerCharacter") && j["triggerCharacter"].is_string())
        ctx.triggerCharacter = j["triggerCharacter"].get<std::string>();
}

//--------------------------------------------------------------------------------------------------------------------

//--------------------------------------------------
// CompletionParams
//--------------------------------------------------
struct CompletionParams : public TextDocumentPositionParams
{
    std::optional<CompletionContext> context;
};

//--------------------------------------------------
inline void to_json(json& j, const CompletionParams& p)
{
    j = json{
        {"textDocument", p.textDocument},
        {"position", p.position}
    };
    if (p.context)
        j["context"] = *p.context;
}

//--------------------------------------------------
inline void from_json(const json& j, CompletionParams& p)
{
    j.at("textDocument").get_to(p.textDocument);
    j.at("position").get_to(p.position);
    if (j.contains("context"))
        p.context = j.at("context").get<CompletionContext>();
}

//--------------------------------------------------------------------------------------------------------------------

//--------------------------------------------------
// CompletionItem
//--------------------------------------------------
struct CompletionItem
{
    std::string label;
    std::optional<CompletionItemKind> kind;
    std::optional<std::string> detail;
    std::optional<std::string> documentation;
    std::optional<std::string> insertText;
    // многие другие поля опущены для краткости, но могут быть добавлены
};

//--------------------------------------------------
inline void to_json(json& j, const CompletionItem& item)
{
    j = json{{"label", item.label}};
    if (item.kind) j["kind"] = *item.kind;
    if (item.detail) j["detail"] = *item.detail;
    if (item.documentation) j["documentation"] = *item.documentation;
    if (item.insertText) j["insertText"] = *item.insertText;
}

//--------------------------------------------------
inline void from_json(const json& j, CompletionItem& item)
{
    j.at("label").get_to(item.label);
    if (j.contains("kind")) item.kind = j.at("kind").get<CompletionItemKind>();
    if (j.contains("detail")) item.detail = j.at("detail").get<std::string>();
    if (j.contains("documentation")) item.documentation = j.at("documentation").get<std::string>();
    if (j.contains("insertText")) item.insertText = j.at("insertText").get<std::string>();
}

//--------------------------------------------------------------------------------------------------------------------

//--------------------------------------------------
// CompletionList
//--------------------------------------------------
struct CompletionList
{
    bool isIncomplete = false;
    std::vector<CompletionItem> items;
};

//--------------------------------------------------
inline void to_json(json& j, const CompletionList& list)
{
    j = json{
        {"isIncomplete", list.isIncomplete},
        {"items", list.items}
    };
}

//--------------------------------------------------
inline void from_json(const json& j, CompletionList& list)
{
    j.at("isIncomplete").get_to(list.isIncomplete);
    j.at("items").get_to(list.items);
}

//--------------------------------------------------------------------------------------------------------------------

//--------------------------------------------------
// WorkspaceFolder
//--------------------------------------------------
struct WorkspaceFolder
{
    std::string uri;
    std::string name;
};

//--------------------------------------------------
inline void to_json(json& j, const WorkspaceFolder& wf)
{
    j = json{
        {"uri", wf.uri},
        {"name", wf.name}
    };
}

//--------------------------------------------------
inline void from_json(const json& j, WorkspaceFolder& wf)
{
    j.at("uri").get_to(wf.uri);
    j.at("name").get_to(wf.name);
}

//--------------------------------------------------------------------------------------------------------------------

//--------------------------------------------------
// InitializeParams
//--------------------------------------------------
struct InitializeParams
{
    std::optional<std::string> rootPath;
    std::optional<std::string> rootUri;
    std::optional<std::vector<WorkspaceFolder>> workspaceFolders;
    // многие другие поля (capabilities, trace, ...) опущены для краткости
};

//--------------------------------------------------
inline void to_json(json& j, const InitializeParams& p)
{
    j = json::object();
    if (p.rootPath) j["rootPath"] = *p.rootPath;
    if (p.rootUri) j["rootUri"] = *p.rootUri;
    if (p.workspaceFolders) j["workspaceFolders"] = *p.workspaceFolders;
}

//--------------------------------------------------
inline void from_json(const json& j, InitializeParams& p)
{
    if (j.contains("rootPath") && j["rootPath"].is_string())
        p.rootPath = j["rootPath"].get<std::string>();
    if (j.contains("rootUri") && j["rootUri"].is_string())
        p.rootUri = j["rootUri"].get<std::string>();
    if (j.contains("workspaceFolders") && j["workspaceFolders"].is_array())
        p.workspaceFolders = j["workspaceFolders"].get<std::vector<WorkspaceFolder>>();
}

//--------------------------------------------------------------------------------------------------------------------

//--------------------------------------------------
// ServerCapabilities
//--------------------------------------------------
struct ServerCapabilities
{
    std::optional<TextDocumentSyncKind> textDocumentSync;
    std::optional<bool> completionProvider;
    // другие возможности можно добавлять
};

//--------------------------------------------------
inline void to_json(json& j, const ServerCapabilities& caps)
{
    j = json::object();
    if (caps.textDocumentSync) j["textDocumentSync"] = *caps.textDocumentSync;
    if (caps.completionProvider) j["completionProvider"] = json::object();
}

//--------------------------------------------------
inline void from_json(const json& j, ServerCapabilities& caps)
{
    if (j.contains("textDocumentSync"))
        caps.textDocumentSync = j.at("textDocumentSync").get<TextDocumentSyncKind>();
    if (j.contains("completionProvider"))
        caps.completionProvider = true;
}

//--------------------------------------------------------------------------------------------------------------------

//--------------------------------------------------
// InitializeResult
//--------------------------------------------------
struct InitializeResult
{
    ServerCapabilities capabilities;
    // serverInfo и другие поля опущены
};

//--------------------------------------------------
inline void to_json(json& j, const InitializeResult& res)
{
    j = json{{"capabilities", res.capabilities}};
}

//--------------------------------------------------
inline void from_json(const json& j, InitializeResult& res)
{
    j.at("capabilities").get_to(res.capabilities);
}

//--------------------------------------------------------------------------------------------------------------------



//--------------------------------------------------
// Исключение для ошибок LSP
//--------------------------------------------------
class LspErrorException : public std::runtime_error
{
public:
    ErrorCode code;
    json data;

    LspErrorException(ErrorCode code, const std::string& msg, const json& data_ = json{})
        : std::runtime_error(msg), code(code), data(data_)
    {}
};

//--------------------------------------------------
// Error
//--------------------------------------------------
struct Error
{
    ErrorCode code = ErrorCode::internalError;
    std::string message;
    std::optional<json> data;

    Error() = default;
    Error(ErrorCode code, const std::string& msg, const json& data_ = json{})
        : code(code), message(msg)
    {
        if (!data_.is_null())
            data = data_;
    }

    Error(const LspErrorException &e)
        : code(e.code), message(e.what())
    {
        if (!e.data.is_null())
            data = e.data;
    }

};

//--------------------------------------------------
inline void to_json(json& j, const Error& err)
{
    j = json{
        {"code", static_cast<int>(err.code)},
        {"message", err.message}
    };
    if (err.data)
        j["data"] = *err.data;
}

//--------------------------------------------------
inline void from_json(const json& j, Error& err)
{
    err.code = static_cast<ErrorCode>(j.at("code").get<int>());
    j.at("message").get_to(err.message);
    if (j.contains("data"))
        err.data = j.at("data");
}

//--------------------------------------------------------------------------------------------------------------------

//--------------------------------------------------
// Request (исходящий или входящий)
//--------------------------------------------------
struct Request
{
    MessageId id;
    std::string method;
    json params;
};

//--------------------------------------------------
inline void to_json(json& j, const Request& req)
{
    j = json{
        {"jsonrpc", "2.0"},
        {"method", req.method},
        {"params", req.params}
    };
    if (!req.id.empty())
        j["id"] = req.id;
}

//--------------------------------------------------
inline void from_json(const json& j, Request& req)
{
    // jsonrpc не проверяем
    j.at("method").get_to(req.method);
    if (j.contains("id"))
        req.id = j.at("id").get<MessageId>();
    if (j.contains("params"))
        req.params = j.at("params");
    else
        req.params = json::object();
}

//--------------------------------------------------------------------------------------------------------------------

//--------------------------------------------------
// Notification (только метод и параметры, без id)
//--------------------------------------------------
struct Notification
{
    std::string method;
    json params;
};

//--------------------------------------------------
inline void to_json(json& j, const Notification& notif)
{
    j = json{
        {"jsonrpc", "2.0"},
        {"method", notif.method},
        {"params", notif.params}
    };
}

//--------------------------------------------------
inline void from_json(const json& j, Notification& notif)
{
    j.at("method").get_to(notif.method);
    if (j.contains("params"))
        notif.params = j.at("params");
    else
        notif.params = json::object();
}

//--------------------------------------------------------------------------------------------------------------------

//--------------------------------------------------
// Response (ответ на запрос)
//--------------------------------------------------
struct Response
{
    MessageId id;
    std::optional<json> result;
    std::optional<Error> error;
};

//--------------------------------------------------
inline void to_json(json& j, const Response& resp)
{
    j = json{{"jsonrpc", "2.0"}};
    if (!resp.id.empty())
        j["id"] = resp.id;
    if (resp.result)
        j["result"] = *resp.result;
    if (resp.error)
        j["error"] = *resp.error;
}

//--------------------------------------------------
inline void from_json(const json& j, Response& resp)
{
    if (j.contains("id"))
        resp.id = j.at("id").get<MessageId>();
    if (j.contains("result"))
        resp.result = j.at("result");
    if (j.contains("error"))
        resp.error = j.at("error").get<Error>();
}

//--------------------------------------------------------------------------------------------------------------------

} // namespace lsp
} // namespace marty

//--------------------------------------------------------------------------------------------------------------------
