/*! \file
    \author Alexander Martynov (Marty AKA al-martyn1) <amart@mail.ru>
    \copyright (c) 2026 Alexander Martynov
    \brief LSP Enum types
 */

#pragma once

//--------------------------------------------------------------------------------------------------------------------
#include "nlohmann/json.hpp"
#include "basics.h"
#include "utils.h"
#include "enums_generated.h"

//
#include <cstdint>
#include <string>
#include <map>
#include <type_traits>


//--------------------------------------------------------------------------------------------------------------------
// https://chat.deepseek.com/share/zy8ij22lwcd5mc86sq

//--------------------------------------------------------------------------------------------------------------------
// marty::lsp
namespace marty {
namespace lsp {

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
template<typename EnumType>
void enum_to_json_as_underlying_type(json &j, EnumType e)
{
    using T = std::underlying_type_t<EnumType>;
    j = static_cast<T>(e);
}

//----------------------------------------------------------------------------
template<typename EnumType>
void enum_from_json_as_underlying_type(const json &j, EnumType &e)
{
    using T = std::underlying_type_t<EnumType>;
    e = static_cast<EnumType>(j.get<T>());
}

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
#define MARTY_LSP_JSON_ENUM_AS_UNDERLYING_TYPE_CONVERSIONS(EnumType) \
            inline                                                   \
            void to_json(json& j, EnumType e)                        \
            {                                                        \
                enum_to_json_as_underlying_type(j, e);               \
            }                                                        \
                                                                     \
            inline                                                   \
            void from_json(const json& j, EnumType &e)               \
            {                                                        \
                enum_from_json_as_underlying_type(j, e);             \
            }
 
//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
template<typename EnumType>
void enum_to_json_as_string(json &j, EnumType e)
{
    auto str = enum_serialize(e);
    j = utils::tolower_copy(str);
}

//----------------------------------------------------------------------------
template<typename EnumType>
void enum_from_json_as_string(const json &j, EnumType &e)
{
    auto str = j.get<std::string>();
    enum_deserialize(e, str);
}

//----------------------------------------------------------------------------
template<typename EnumType>
void enum_to_json_as_string_ex(json &j, EnumType e, std::map<std::string, std::string> &serializeExMap) // Для фиксов того, что генератор не смог нагенерить
{
    auto str = enum_serialize(e);
    str = utils::tolower_copy(str);
    auto it = serializeExMap.find(str);
    if (it==serializeExMap.end())
        j = str;
    else 
        j = it->second;
}

//----------------------------------------------------------------------------
template<typename EnumType>
void enum_from_json_as_string_ex(const json &j, EnumType &e, std::map<std::string, std::string> &deserializeExMap) // Для фиксов того, что генератор не смог нагенерить
{
    auto str = j.get<std::string>();
    str = utils::tolower_copy(str);
    auto it = deserializeExMap.find(str);
    if (it==deserializeExMap.end())
        enum_deserialize(e, str);
    else
        enum_deserialize(e, it->second);
}

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
#define MARTY_LSP_JSON_ENUM_AS_STRING_CONVERSIONS(EnumType) \
            inline                                         \
            void to_json(json& j, EnumType e)              \
            {                                              \
                enum_to_json_as_string(j, e);              \
            }                                              \
                                                           \
            inline                                         \
            void from_json(const json& j, EnumType &e)     \
            {                                              \
                enum_from_json_as_string(j, e);            \
            }

#define MARTY_LSP_JSON_ENUM_AS_STRING_CONVERSIONS_EX(EnumType, serializeMap, deserializeMap) \
            inline                                                 \
            void to_json(json& j, EnumType e)                      \
            {                                                      \
                enum_to_json_as_string_ex(j, e, serializeMap);     \
            }                                                      \
                                                                   \
            inline                                                 \
            void from_json(const json& j, EnumType &e)             \
            {                                                      \
                enum_from_json_as_string_ex(j, e, deserializeMap); \
            }

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
MARTY_LSP_JSON_ENUM_AS_STRING_CONVERSIONS(MarkupKind)
MARTY_LSP_JSON_ENUM_AS_STRING_CONVERSIONS(ResourceOperationKind)
MARTY_LSP_JSON_ENUM_AS_STRING_CONVERSIONS(FailureHandlingKind)
MARTY_LSP_JSON_ENUM_AS_STRING_CONVERSIONS(DocumentDiagnosticReportKind)
MARTY_LSP_JSON_ENUM_AS_STRING_CONVERSIONS(TraceSetting)
MARTY_LSP_JSON_ENUM_AS_STRING_CONVERSIONS(TokenFormat)
MARTY_LSP_JSON_ENUM_AS_STRING_CONVERSIONS(FoldingRangeKind)
MARTY_LSP_JSON_ENUM_AS_STRING_CONVERSIONS(FileOperationPatternKind)

MARTY_LSP_JSON_ENUM_AS_STRING_CONVERSIONS_EX( PositionEncodingKind
                                            {{"utf8", "utf-8"}, {"utf16", "utf-16"}, {"utf32", "utf-32"}} // serializeMap
                                            {{"utf-8", "utf8"}, {"utf-16", "utf16"}, {"utf-32", "utf32"}} // deserializeMap
                                            )
// Для WatchKind сгенерены сериализация/десериализация в строки, но генерация там использовалась чисто ради флагов
// А на самом деле этот enum сериализуется как обычное число
MARTY_LSP_JSON_ENUM_AS_UNDERLYING_TYPE_CONVERSIONS(WatchKind)

// Аналогично WatchKind
MARTY_LSP_JSON_ENUM_AS_UNDERLYING_TYPE_CONVERSIONS(SemanticTokenModifiers)


//----------------------------------------------------------------------------



//--------------------------------------------------------------------------------------------------------------------
enum class ErrorCodes : int
{
    // Defined by JSON-RPC
    parseError              = -32700,
    invalidRequest          = -32600,
    methodNotFound          = -32601,
    invalidParams           = -32602,
    internalError           = -32603,

    // Defined by LSP
    serverNotInitialized    = -32002,
    unknownErrorCode        = -32001,

    // Additional
    requestCancelled        = -32800,
    contentModified         = -32801
};

MARTY_LSP_JSON_ENUM_AS_UNDERLYING_TYPE_CONVERSIONS(ErrorCodes)

//--------------------------------------------------------------------------------------------------------------------



//--------------------------------------------------------------------------------------------------------------------
enum class CompletionItemKind : int
{
    text            = 1,
    method          = 2,
    function        = 3,
    constructor     = 4,
    field           = 5,
    variable        = 6,
    class_          = 7,
    interface       = 8,
    module          = 9,
    property        = 10,
    unit            = 11,
    value           = 12,
    enum_           = 13,
    keyword         = 14,
    snippet         = 15,
    color           = 16,
    file            = 17,
    reference       = 18,
    folder          = 19,
    enumMember      = 20,
    constant        = 21,
    struct_         = 22,
    event           = 23,
    operator_       = 24,
    typeParameter   = 25
};

MARTY_LSP_JSON_ENUM_AS_UNDERLYING_TYPE_CONVERSIONS(CompletionItemKind)

//--------------------------------------------------------------------------------------------------------------------



//--------------------------------------------------------------------------------------------------------------------
enum class InsertTextFormat : int
{
    plainText = 1,
    snippet   = 2
};

MARTY_LSP_JSON_ENUM_AS_UNDERLYING_TYPE_CONVERSIONS(InsertTextFormat)

//--------------------------------------------------------------------------------------------------------------------



//--------------------------------------------------------------------------------------------------------------------
enum class TextDocumentSyncKind : int
{
    none        = 0,
    full        = 1,
    incremental = 2
};

MARTY_LSP_JSON_ENUM_AS_UNDERLYING_TYPE_CONVERSIONS(TextDocumentSyncKind)

//--------------------------------------------------------------------------------------------------------------------



//--------------------------------------------------------------------------------------------------------------------
enum class DiagnosticSeverity : int
{
    error       = 1,
    warning     = 2,
    information = 3,
    hint        = 4
};

MARTY_LSP_JSON_ENUM_AS_UNDERLYING_TYPE_CONVERSIONS(DiagnosticSeverity)

//--------------------------------------------------------------------------------------------------------------------



//--------------------------------------------------------------------------------------------------------------------
enum class SymbolKind : int
{
    file            = 1,
    module          = 2,
    namespace_      = 3,
    package         = 4,
    class_          = 5,
    method          = 6,
    property        = 7,
    field           = 8,
    constructor     = 9,
    enum_           = 10,
    interface       = 11,
    function        = 12,
    variable        = 13,
    constant        = 14,
    string_         = 15,
    number          = 16,
    boolean         = 17,
    array           = 18,
    object          = 19,
    key             = 20,
    null_           = 21,
    enumMember      = 22,
    struct_         = 23,
    event           = 24,
    operator_       = 25,
    typeParameter   = 26
};

using DocumentSymbolKind = SymbolKind;

MARTY_LSP_JSON_ENUM_AS_UNDERLYING_TYPE_CONVERSIONS(SymbolKind)

//--------------------------------------------------------------------------------------------------------------------



//--------------------------------------------------------------------------------------------------------------------
enum class FileChangeType : int
{
    created = 1,
    changed = 2,
    deleted = 3
};

MARTY_LSP_JSON_ENUM_AS_UNDERLYING_TYPE_CONVERSIONS(FileChangeType)

//--------------------------------------------------------------------------------------------------------------------

// // SemanticTokenTypes - открытый список строк, не надо его переводить в перечисления
//   namespace — пространство имён
//   type — обобщённый тип (используется как fallback для типов, которые нельзя отнести к более конкретным, например, class или enum)
//   class — класс
//   enum — перечисление
//   interface — интерфейс
//   struct — структура
//   typeParameter — параметр типа (например, <T> в дженериках)
//   parameter — параметр функции или метода
//   variable — переменная
//   property — свойство
//   enumMember — член перечисления
//   event — событие
//   function — функция (свободная, не принадлежащая классу)
//   method — метод (принадлежит классу/структуре)
//   macro — макрос
//   keyword — ключевое слово языка (например, if, for, while)
//   modifier — модификатор доступа или другой модификатор (например, public, private, static)
//   comment — комментарий
//   string — строковый литерал
//   number — числовой литерал
//   regexp — регулярное выражение
//   operator — оператор (например, +, -, *, /)

// // CodeActionKind
//   "" (Пустая строка): Пустой вид.
//   "quickfix": Базовый вид для действий быстрого исправления.
//   "refactor": Базовый вид для действий рефакторинга.
//   "refactor.extract": Базовый вид для действий извлечения (например, извлечение метода, переменной).
//   "refactor.inline": Базовый вид для действий встраивания (например, встраивание функции, константы).
//   "refactor.rewrite": Базовый вид для действий переписывания (например, добавление параметра, перемещение метода).
//   "source": Базовый вид для действий над исходным кодом (применяются ко всему файлу).
//   "source.organizeImports": Действие для организации импортов.
//   "source.fixAll": Действие для автоматического исправления всех ошибок.

//--------------------------------------------------------------------------------------------------------------------
enum class CompletionTriggerKind : int
{
    invoked                          = 1,
    triggerCharacter                 = 2,
    triggerForIncompleteCompletions  = 3
};

MARTY_LSP_JSON_ENUM_AS_UNDERLYING_TYPE_CONVERSIONS(CompletionTriggerKind)

//--------------------------------------------------------------------------------------------------------------------



//--------------------------------------------------------------------------------------------------------------------
enum class DiagnosticTag : int
{
    unnecessary  = 1,
    deprecated   = 2
};

MARTY_LSP_JSON_ENUM_AS_UNDERLYING_TYPE_CONVERSIONS(DiagnosticTag)

//--------------------------------------------------------------------------------------------------------------------



//--------------------------------------------------------------------------------------------------------------------
enum class SymbolTag : int
{
    deprecated = 1
};

MARTY_LSP_JSON_ENUM_AS_UNDERLYING_TYPE_CONVERSIONS(SymbolTag)

//--------------------------------------------------------------------------------------------------------------------



//--------------------------------------------------------------------------------------------------------------------
enum class CodeActionTriggerKind : int
{
    invoked   = 1,
    automatic = 2
};

MARTY_LSP_JSON_ENUM_AS_UNDERLYING_TYPE_CONVERSIONS(CodeActionTriggerKind)

//--------------------------------------------------------------------------------------------------------------------




//--------------------------------------------------------------------------------------------------------------------
enum class DocumentHighlightKind : int
{
    text  = 1,
    read  = 2,
    write = 3
};

MARTY_LSP_JSON_ENUM_AS_UNDERLYING_TYPE_CONVERSIONS(DocumentHighlightKind)

//----------------------------------------------------------------------------



//--------------------------------------------------------------------------------------------------------------------
enum class CompletionItemTag : int
{
    deprecated = 1
};

MARTY_LSP_JSON_ENUM_AS_UNDERLYING_TYPE_CONVERSIONS(CompletionItemTag)

//----------------------------------------------------------------------------



//--------------------------------------------------------------------------------------------------------------------
enum class InsertTextMode : int
{
    asIs               = 1,
    adjustIndentation  = 2
};

MARTY_LSP_JSON_ENUM_AS_UNDERLYING_TYPE_CONVERSIONS(InsertTextMode)

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
enum class InitializeErrorCodes : int
{
    unknownProtocolVersion = 1
};

MARTY_LSP_JSON_ENUM_AS_UNDERLYING_TYPE_CONVERSIONS(InitializeErrorCodes)

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
enum class MessageType : int
{
    error   = 1,
    warning = 2,
    info    = 3,
    log     = 4
};

MARTY_LSP_JSON_ENUM_AS_UNDERLYING_TYPE_CONVERSIONS(MessageType)

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
enum class SignatureHelpTriggerKind : int
{
    invoked          = 1,
    triggerCharacter = 2,
    contentChange    = 3
};

MARTY_LSP_JSON_ENUM_AS_UNDERLYING_TYPE_CONVERSIONS(SignatureHelpTriggerKind)

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
enum class InlayHintKind : int
{
    type      = 1,
    parameter = 2
};

MARTY_LSP_JSON_ENUM_AS_UNDERLYING_TYPE_CONVERSIONS(InlayHintKind)

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
enum class MonikerKind : int
{
    import_ = 1,
    export_ = 2,
    local  = 3
};

MARTY_LSP_JSON_ENUM_AS_UNDERLYING_TYPE_CONVERSIONS(MonikerKind)

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
enum class NotebookCellKind : int
{
    markup = 1,
    code   = 2
};

MARTY_LSP_JSON_ENUM_AS_UNDERLYING_TYPE_CONVERSIONS(NotebookCellKind)

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
enum class TextDocumentSaveReason : int
{
    manual     = 1,
    afterDelay = 2,
    focusOut   = 3
};

MARTY_LSP_JSON_ENUM_AS_UNDERLYING_TYPE_CONVERSIONS(TextDocumentSaveReason)

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
enum class UniquenessLevel : int
{
    document = 1,
    project  = 2,
    group    = 3,
    scheme   = 4,
    global   = 5
};

MARTY_LSP_JSON_ENUM_AS_UNDERLYING_TYPE_CONVERSIONS(UniquenessLevel)

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
enum class PrepareSupportDefaultBehavior : int
{
    identifier = 1
};

MARTY_LSP_JSON_ENUM_AS_UNDERLYING_TYPE_CONVERSIONS(PrepareSupportDefaultBehavior)

//----------------------------------------------------------------------------








//--------------------------------------------------------------------------------------------------------------------
} // namespace lsp
} // namespace marty
