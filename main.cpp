#include <iostream>
#include <vector>
#include <regex>
#include <fstream>
#include <sstream>

enum TokenType {
    Keyword,
    Identifier,
    String,
    Number,
    HexadecimalNumber,
    DecimalNumber,
    Punctuation,
    Operator,
    Comment,
    PreprocessorDirective,
    Unknown
};

struct Token {
    std::string value;
    TokenType type;
};


bool isKeyword(const std::string& token) {
    std::vector<std::string> keywords = {
            "abstract", "as", "base", "bool", "break", "byte", "case", "catch", "char", "checked",
            "class", "const", "continue", "decimal", "default", "delegate", "do", "double", "else",
            "enum", "event", "explicit", "extern", "false", "finally", "fixed", "float", "for",
            "foreach", "goto", "if", "implicit", "in", "int", "interface", "internal", "is", "lock",
            "long", "namespace", "new", "null", "object", "operator", "out", "override", "params",
            "private", "protected", "public", "readonly", "ref", "return", "sbyte", "sealed", "short",
            "sizeof", "stackalloc", "static", "string", "struct", "switch", "this", "throw", "true",
            "try", "typeof", "uint", "ulong", "unchecked", "unsafe", "ushort", "using", "virtual",
            "void", "volatile", "while"
    };
    return std::find(keywords.begin(), keywords.end(), token) != keywords.end();
}

bool isIdentifier(const std::string& token) {
    std::regex identifier_regex("[_a-zA-Z][_a-zA-Z0-9]*");
    return std::regex_match(token, identifier_regex);
}

bool isStringConstant(const std::string& token) {
    std::regex string_regex("\"([^\"]|\\\\.)*\"");
    return std::regex_match(token, string_regex);
}

bool isHexadecimalNumber(const std::string& token) {
    std::regex hexPattern("^0[xX][0-9a-fA-F]+$");
    return std::regex_match(token, hexPattern);
}

bool isDecimalNumber(const std::string& token) {
    std::regex decimalPattern("^\\d*\\.\\d+$");
    return std::regex_match(token, decimalPattern);
}

bool isNumericConstant(const std::string& token) {
    std::regex numeric_regex("[0-9]+");
    return std::regex_match(token, numeric_regex);
}

bool isOperator(const std::string& token) {
    std::vector<std::string> operators = { "+", "-", "*", "/", "%", "&", "|", "^", "!", "~", "++", "--", "<<", ">>", "==", "!=", ">", "<", ">=", "<=", "&&", "||", "??", "?:", "=>", "=", "+=", "-=", "*=", "/=", "%=", "&=", "|=", "^=", "<<=", ">>=", "[]", "()", ".", "->" };
    return std::find(operators.begin(), operators.end(), token) != operators.end();
}

bool isDelimiter(const std::string& token) {
    std::vector<std::string> delimiters = { ",", ";", "{", "}", "(", ")", "[", "]", ":", "?" };
    return std::find(delimiters.begin(), delimiters.end(), token) != delimiters.end();
}

bool isPreprocessorDirective(const std::string& token) {
    std::vector<std::string> preprocessorKeywords = {
            "#if", "#elif", "#else", "#endif", "#define", "#undef", "#warning", "#error",
            "#line", "#region", "#endregion", "#pragma"
    };

    return std::find(preprocessorKeywords.begin(), preprocessorKeywords.end(), token) != preprocessorKeywords.end();
}

std::vector<Token> tokenize(const std::string& code) {
    std::vector<Token> tokens;
    std::string token;
    bool isString = false;
    bool isSingleLineComment = false;
    bool isMultiLineComment = false;

    // Iterate over the code character by character
    for (size_t i = 0; i < code.length(); ++i) {
        char c = code[i];

        // Handle comments
        if (c == '/' && !isString && !isMultiLineComment) {
            if (i + 1 < code.length()) {
                if (code[i + 1] == '/') isSingleLineComment = true;
                else if (code[i + 1] == '*') isMultiLineComment = true;
            }
        }

        // Handle end of comments
        if (isSingleLineComment && c == '\n') {
            isSingleLineComment = false;
            tokens.push_back({token, Comment});
            token.clear();
        }
        if (isMultiLineComment && c == '*' && i + 1 < code.length() && code[i + 1] == '/') {
            isMultiLineComment = false;
            token += "*/";
            tokens.push_back({token, Comment});
            token.clear();
            ++i;
            continue;
        }

        if (isSingleLineComment || isMultiLineComment) {
            token += c;
            continue;
        }

        // Handle strings
        if (c == '\"') {
            if (isString) {
                token += c;
                tokens.push_back({token, String });
                token.clear();
            }
            else {
                if (!token.empty()) {
                    tokens.push_back({token, Unknown });
                    token.clear();
                }
            }
            isString = !isString;
        }
        else if (isString) {
            token += c;
        }
        else if (std::isspace(c)) {
            if (!token.empty()) {
                tokens.push_back({token, Unknown });
                token.clear();
            }
        }
        else if (std::ispunct(c) && c != '.' && c != '#') {
            if (!token.empty()) {
                tokens.push_back({token, Unknown });
                token.clear();
            }
            tokens.push_back({std::string(1, c), Unknown });
        }
        else {
            token += c;
        }
    }

    if (!token.empty()) {
        tokens.push_back({token, Unknown });
    }

    for (Token &t: tokens) {
        if (isKeyword(t.value)) t.type = Keyword;
        else if (isPreprocessorDirective(t.value)) t.type = PreprocessorDirective;
        else if (isIdentifier(t.value)) t.type = Identifier;
        else if (isHexadecimalNumber(t.value)) t.type = HexadecimalNumber;
        else if (isDecimalNumber(t.value)) t.type = DecimalNumber;
        else if (isNumericConstant(t.value)) t.type = Number;
        else if (isOperator(t.value)) t.type = Operator;
        else if (isDelimiter(t.value)) t.type = Punctuation;
        else if (isStringConstant(t.value)) t.type = String;
    }

    return tokens;
}



void displayTokens(const std::vector<Token>& tokens) {
    for (const Token& token : tokens) {
        std::cout << token.value << " - ";
        switch (token.type) {
            case Keyword: std::cout << "Keyword"; break;
            case Identifier: std::cout << "Identifier"; break;
            case String: std::cout << "String Constant"; break;
            case Number: std::cout << "Numeric Constant"; break;
            case HexadecimalNumber: std::cout << "Hexadecimal Number"; break;
            case DecimalNumber: std::cout << "Decimal Number"; break;
            case Operator: std::cout << "Operator"; break;
            case Punctuation: std::cout << "Delimiter"; break;
            case PreprocessorDirective: std::cout << "Preprocessor Directive"; break;
            case Comment: std::cout << "Comment"; break;
            case Unknown: std::cout << "Unknown"; break;
        }
        std::cout << std::endl;
    }
}

int main() {
    std::ifstream inputFile("..//test.cs");

    if (!inputFile.is_open())
    {
        std::cerr << "Не вдалося відкрити файл" << std::endl;
        return 1;
    }

    std::stringstream buffer;
    buffer << inputFile.rdbuf();
    std::string code = buffer.str();

    inputFile.close();

    std::vector<Token> tokens = tokenize(code);
    displayTokens(tokens);
    return 0;
}