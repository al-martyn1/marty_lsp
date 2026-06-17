@set UINT8=--underlaying-type=std::uint8_t
@set UINT16=--underlaying-type=std::uint16_t
@set UINT32=--underlaying-type=std::uint32_t
@set UINT=--underlaying-type=unsigned
@set INT=--underlaying-type=int

@set HEX2=--hex-width=2
@set HEX4=--hex-width=4
@set HEX6=--hex-width=6
@set HEX8=--hex-width=8

@rem set EXTRA=--enum-flags=extra
@set FLAGENUM_EXTRA=--enum-flags=serialize-set,deserialize-set

@set FLAGS=--enum-flags=flags
@set DECL=--enum-flags=type-decl
@set CLS=--enum-flags=enum-class
@set DECLCLS=--enum-flags=type-decl,enum-class
@set FMTHEX=--enum-flags=fmt-hex
@set SERIALIZE=--enum-flags=serialize,deserialize,lowercase

@set TPL_OVERRIDE=--override-template-parameter=EnumNameFormat:$(ENAMNAME)
@set GEN_OPTS=--enum-values-style=CamelStyle

@set VALUES_CAMEL=--enum-values-style=CamelStyle
@set SERIALIZE_CAMEL=--serialize-style=CamelStyle
@set VALUES_PASCAL=--enum-values-style=PascalStyle
@set SERIALIZE_PASCAL=--serialize-style=PascalStyle
@set SERIALIZE_HYPHEN=--serialize-style=HyphenStyle

@set TR=--tr-lang=en-US,ru-RU --tr-template-language=en-US --tr-template-output=./tr/en-US.json --translation=./enum-translations.json
@set MD=--md-output-path=../md_/_enums

@set SNIPPETOPTIONS_GEN_FLAGS=--enum-flags=0 --enum-flags=type-decl,serialize,deserialize,lowercase,enum-class,fmt-hex %VALUES_CAMEL% %SERIALIZE_PASCAL% --enum-serialize-style=All

@set FLAGS=--enum-flags=flags

@rem call %~dp0\tr.bat
@rem umba-tr --help > umba-tr-help.txt
@rem umba-tr --lang-tag-format=langTag --overwrite --scan=./tr --include-files=*.json enum-translations.json >umba-tr.log 2>&1

@rem %TR% %MD%
umba-enum-gen %GEN_OPTS% %TPL_OVERRIDE% %SNIPPETOPTIONS_GEN_FLAGS%                  ^
    %UINT% -E=MarkupKind                     -F=@MarkupKind.txt                     ^
    %UINT% -E=ResourceOperationKind          -F=@ResourceOperationKind.txt          ^
    %UINT% -E=FailureHandlingKind            -F=@FailureHandlingKind.txt            ^
    %UINT% -E=DocumentDiagnosticReportKind   -F=@DocumentDiagnosticReportKind.txt   ^
    %UINT% -E=TraceSetting                   -F=@TraceSetting.txt                   ^
    %UINT% -E=PositionEncodingKind           -F=@PositionEncodingKind.txt           ^
    %UINT% -E=TokenFormat                    -F=@TokenFormat.txt                    ^
    %UINT% -E=FoldingRangeKind               -F=@FoldingRangeKind.txt               ^
    %UINT% -E=FileOperationPatternKind       -F=@FileOperationPatternKind.txt       ^
    %FLAGS% %FLAGENUM_EXTRA%                                                        ^
    %UINT%  %HEX2% -E=WatchKind              -F=@WatchKind.txt                      ^
    %UINT%  %HEX2% -E=SemanticTokenModifiers -F=@SemanticTokenModifiers.txt         ^
..\enums_generated.h

