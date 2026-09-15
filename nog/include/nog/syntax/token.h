#pragma once

#include "nog/source.h"
#include "nog/syntax/symbol.h"
#include "nog/util/num.h"

#include <cstddef>
namespace nog {

    enum class KeywordKind {
        If,
        Else,
        Match,
        For,
        Break,
        Return,
        Default,
        Const,
        Namespace,
        Enum,
        Set,
        Struct,
        Interface,
        Func,
        Type,
        Public,
        Protected,
        Private,
        With,
        Use,
        As,
        Is,
        let,
        Mut,
        Spool,
        Defer,
        Static,
        Dyn,
        Auto,
        Of,
        Import,
        New,
        Delete,
        Typecast,
        Sizeof,
        Typeof,
        Library,
        Program,
        Null,
        This,
        From,
        Shared,
        Virtual,
        Abstract,
        Override,
        Constant,
        Extend,
        Impl,
        Constructor,
        Destructor,
        Alias,
        Include,
        True,
        False,
        Panic,
        Package,
        Mod,
        Safe,
        Unsafe,
        Variant
    };

    std::ostream& operator<<(std::ostream& output, KeywordKind kind);

    enum class TokenKind {
        None,
        Invalid,
        Eof,
        Whitespace,
        Newline,
        Comment,
        Identifier,
        Keyword,
        Literal,
        ParenOpen,
        ParenClose,
        BraceOpen,
        BraceClose,
        BracketOpen,
        BracketClose,
        Slash,
        Dot,
        DotDot,
        DotDotDot,
        Comma,
        Colon,
        Semicolon,
        Tilde,
        TildeEq,
        Bang,
        BangEq,
        Eq,
        EqEq,
        EqGt,
        Asterisk,
        AsteriskEq,
        AsteriskAsterisk,
        AsteriskAsteriskEq,
        Percent,
        PercentEq,
        Question,
        QuestionEq,
        Plus,
        PlusPlus,
        PlusEq,
        Minus,
        MinusMinus,
        MinusEq,
        Amp,
        AmpAmp,
        AmpEq,
        Pipe,
        PipePipe,
        PipeEq,
        Caret,
        CaretEq,
        Lt,
        LtLt,
        LtLtEq,
        LtEq,
        Gt,
        GtGt,
        GtGtEq,
        GtEq,
        Hash,
        HashEq,
        LtEqGt,
        Underscore,
        Backslash,
        At,
        Dollar
    };

    std::ostream& operator<<(std::ostream& output, TokenKind kind);

    enum class NewlineType {
        CarriageReturn,
        LineFeed,
    };

    enum class CommentType {
        Line,
        Block,
        Doc,
    };

    enum class LiteralType {
        None,
        String,
        Decimal,
        DecimalHex,
        DecimalBin,
        DecimalOct,
        DecimalSci,
        DecimalSciFp,
        FloatingPoint,
        FloatingPointSci,
        FloatingPointSciFp,
        Char,
        CharHex,
        CharBin,
        CharOct,
        CharUnicode,
        BoolLiteral,
        BoolLiteralInt
    };

    typedef uint64_t EncodedTokenDetails;

    struct WhitespaceDetails {
        size_t length;

        explicit WhitespaceDetails(size_t raw_data) : length{raw_data} {}

        EncodedTokenDetails as_encoded() const { return this->length; }
    };

    struct NewlineDetails {
        NewlineType newline_ty;

        explicit NewlineDetails(size_t raw_data) : newline_ty{static_cast<NewlineType>(raw_data)} {}
        EncodedTokenDetails as_encoded() const { return static_cast<EncodedTokenDetails>(this->newline_ty); }
    };

    struct CommentDetails {
        CommentType comment_ty;
        size_t length;

        explicit CommentDetails(size_t raw_data)
            : comment_ty{static_cast<CommentType>(read_at_offset<uint32_t>(&raw_data, 0))},
              length{read_at_offset<uint32_t>(&raw_data, 4)} {}

        EncodedTokenDetails as_encoded() const {
            IntConverter conv;
            conv.two_parts1 = static_cast<uint32_t>(this->comment_ty);
            conv.two_parts2 = static_cast<uint32_t>(this->length);
            return conv.u64;
        }
    };

    struct LiteralDetails {
        LiteralType literal_ty;
        Symbol sym;

        explicit LiteralDetails(size_t raw_data)
            : literal_ty{static_cast<LiteralType>(read_at_offset<uint32_t>(&raw_data, 0))},
              sym{read_at_offset<uint32_t>(&raw_data, 4)} {}

        EncodedTokenDetails as_encoded() const {
            IntConverter conv;
            conv.two_parts1 = static_cast<uint32_t>(this->literal_ty);
            conv.two_parts2 = static_cast<uint32_t>(this->sym.index());
            return conv.u64;
        }
    };

    struct IdentifierDetails {
        Symbol sym;

        explicit IdentifierDetails(size_t raw_data) : sym{read_at_offset<uint32_t>(&raw_data, 0)} {}

        EncodedTokenDetails as_encoded() const { return this->sym.index(); }
    };

    struct KeywordDetails {
        KeywordKind keyword_kind;
        explicit KeywordDetails(size_t raw_data)
            : keyword_kind{static_cast<KeywordKind>(read_at_offset<uint32_t>(&raw_data, 0))} {}

        EncodedTokenDetails as_encoded() const { return static_cast<EncodedTokenDetails>(this->keyword_kind); }
    };

    class Token {
    private:
        EncodedTokenDetails token_data;
        TokenKind tkn_kind;
        Span sp;

    public:
        static const Token& END_OF_FILE;
        static const Token& INVALID;

        inline static Token make_dummy(TokenKind kind) { return Token(kind, 0, Span::INVALID); }

        Token(TokenKind kind, EncodedTokenDetails encoded_token_data, Span span)
            : tkn_kind{kind}, token_data{encoded_token_data}, sp{span} {}

        Token() : Token(TokenKind::Invalid, 0, Span::INVALID) {}
        TokenKind kind() const { return this->tkn_kind; }
        Span span() const { return this->sp; }
        AbsolutePos start_pos() const { return this->sp.start_pos(); }
        AbsolutePos end_pos() const { return this->sp.end_pos(); }
        size_t length() const { return this->sp.length(); }

        bool is_trivia() const {
            switch (this->kind()) {
                case TokenKind::Newline:
                case TokenKind::Whitespace:
                case TokenKind::Comment:
                    return true;
                default:
                    return false;
            }
        }

        bool has_details() const {
            switch (this->kind()) {
                case TokenKind::Identifier:
                case TokenKind::Keyword:
                case TokenKind::Comment:
                case TokenKind::Literal:
                case TokenKind::Whitespace:
                case TokenKind::Newline:
                    return true;
                default:
                    return false;
            }
        }

        WhitespaceDetails whitespace_details() const {
            NOG_ASSERT(this->kind() == TokenKind::Whitespace,
                       "trying to get whitespace details from non-whitespace token");
            return WhitespaceDetails(this->token_data);
        }

        CommentDetails comment_details() const {
            NOG_ASSERT(this->kind() == TokenKind::Comment,
                       "trying to get comment details from non-whitespace token kind");
            return CommentDetails(this->token_data);
        }

        NewlineDetails newline_details() const {
            NOG_ASSERT(this->kind() == TokenKind::Newline,
                       "trying to get newline details from non-newline token kind");
            return NewlineDetails(this->token_data);
        }

        LiteralDetails literal_details() const {
            NOG_ASSERT(this->kind() == TokenKind::Literal,
                       "trying to get literal details from non-literal token kind");
            return LiteralDetails(this->token_data);
        }

        IdentifierDetails identifier_details() const {
            NOG_ASSERT(this->kind() == TokenKind::Identifier,
                       "trying to get identifier details from non-identifier token kind");
            return IdentifierDetails(this->token_data);
        }

        KeywordDetails keyword_details() const {
            NOG_ASSERT(this->kind() == TokenKind::Keyword,
                       "trying to get keyword details from non-keyword token kind");
            return KeywordDetails(this->token_data);
        }

        friend std::ostream& operator<<(std::ostream& output, const Token& tok) {
            return output << "[Token kind:" << tok.kind() << ", span:" << tok.span()
                          << ", details:" << tok.token_data << "]";
        }
    };

} // namespace nog
