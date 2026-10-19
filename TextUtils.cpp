#include "TextUtils.h"

namespace text_utils {

namespace {

const std::u32string& UkrainianAlphabet() {
    static const std::u32string alphabet = U"абвгґдеєжзиіїйклмнопрстуфхцчшщьюя";
    return alphabet;
}

const std::u32string& Vowels() {
    static const std::u32string vowels = U"аеєиіїоуюяaeiou";
    return vowels;
}

bool IsApostrophe(char32_t symbol) {
    return symbol == U'\'' || symbol == 0x2019 || symbol == 0x02BC;
}

int Rank(char32_t symbol) {
    const char32_t lower = ToLower(symbol);
    const std::size_t position = UkrainianAlphabet().find(lower);
    if (position != std::u32string::npos) {
        return static_cast<int>(position);
    }
    return 100 + static_cast<int>(lower);
}

}  // namespace

std::u32string ToUtf32(const std::string& utf8) {
    std::u32string result;
    std::size_t i = 0;
    while (i < utf8.size()) {
        const unsigned char lead = static_cast<unsigned char>(utf8[i]);
        char32_t codePoint = 0;
        std::size_t length = 1;
        if (lead < 0x80) {
            codePoint = lead;
        } else if ((lead >> 5) == 0x6) {
            codePoint = lead & 0x1F;
            length = 2;
        } else if ((lead >> 4) == 0xE) {
            codePoint = lead & 0x0F;
            length = 3;
        } else if ((lead >> 3) == 0x1E) {
            codePoint = lead & 0x07;
            length = 4;
        } else {
            ++i;  // некоректний байт — пропускаємо
            continue;
        }
        if (i + length > utf8.size()) {
            break;
        }
        for (std::size_t k = 1; k < length; ++k) {
            codePoint = (codePoint << 6) |
                        (static_cast<unsigned char>(utf8[i + k]) & 0x3F);
        }
        result.push_back(codePoint);
        i += length;
    }
    return result;
}

std::string ToUtf8(const std::u32string& utf32) {
    std::string result;
    for (const char32_t cp : utf32) {
        if (cp < 0x80) {
            result.push_back(static_cast<char>(cp));
        } else if (cp < 0x800) {
            result.push_back(static_cast<char>(0xC0 | (cp >> 6)));
            result.push_back(static_cast<char>(0x80 | (cp & 0x3F)));
        } else if (cp < 0x10000) {
            result.push_back(static_cast<char>(0xE0 | (cp >> 12)));
            result.push_back(static_cast<char>(0x80 | ((cp >> 6) & 0x3F)));
            result.push_back(static_cast<char>(0x80 | (cp & 0x3F)));
        } else {
            result.push_back(static_cast<char>(0xF0 | (cp >> 18)));
            result.push_back(static_cast<char>(0x80 | ((cp >> 12) & 0x3F)));
            result.push_back(static_cast<char>(0x80 | ((cp >> 6) & 0x3F)));
            result.push_back(static_cast<char>(0x80 | (cp & 0x3F)));
        }
    }
    return result;
}

char32_t ToLower(char32_t symbol) {
    if (symbol >= U'A' && symbol <= U'Z') {
        return static_cast<char32_t>(symbol + 32);
    }
    if (symbol >= 0x0410 && symbol <= 0x042F) {  // А–Я
        return static_cast<char32_t>(symbol + 0x20);
    }
    if (symbol == 0x0404 || symbol == 0x0406 || symbol == 0x0407) {  // Є І Ї
        return static_cast<char32_t>(symbol + 0x50);
    }
    if (symbol == 0x0490) {  // Ґ
        return 0x0491;
    }
    return symbol;
}

std::string ToLower(const std::string& utf8) {
    std::u32string text = ToUtf32(utf8);
    for (char32_t& symbol : text) {
        symbol = ToLower(symbol);
    }
    return ToUtf8(text);
}

bool IsLetter(char32_t symbol) {
    const char32_t lower = ToLower(symbol);
    return (lower >= U'a' && lower <= U'z') ||
           (lower >= 0x0430 && lower <= 0x044F) || lower == 0x0454 ||
           lower == 0x0456 || lower == 0x0457 || lower == 0x0491;
}

bool IsVowel(char32_t symbol) {
    return Vowels().find(ToLower(symbol)) != std::u32string::npos;
}

std::string Trim(const std::string& text) {
    const char* whitespace = " \t\r\n";
    const std::size_t first = text.find_first_not_of(whitespace);
    if (first == std::string::npos) {
        return "";
    }
    const std::size_t last = text.find_last_not_of(whitespace);
    return text.substr(first, last - first + 1);
}

std::vector<std::string> ExtractWords(const std::string& text) {
    const std::u32string symbols = ToUtf32(text);
    std::vector<std::string> words;
    std::u32string current;

    auto flush = [&]() {
        if (!current.empty()) {
            words.push_back(ToUtf8(current));
            current.clear();
        }
    };

    for (std::size_t i = 0; i < symbols.size(); ++i) {
        if (IsLetter(symbols[i])) {
            current.push_back(symbols[i]);
        } else if (IsApostrophe(symbols[i]) && !current.empty() &&
                   i + 1 < symbols.size() && IsLetter(symbols[i + 1])) {
            current.push_back(symbols[i]);
        } else {
            flush();
        }
    }
    flush();
    return words;
}

int CompareAlphabetical(const std::string& left, const std::string& right) {
    const std::u32string a = ToUtf32(left);
    const std::u32string b = ToUtf32(right);
    const std::size_t common = a.size() < b.size() ? a.size() : b.size();
    for (std::size_t i = 0; i < common; ++i) {
        const int difference = Rank(a[i]) - Rank(b[i]);
        if (difference != 0) {
            return difference;
        }
    }
    if (a.size() == b.size()) {
        return 0;
    }
    return a.size() < b.size() ? -1 : 1;
}

std::string RhymeKey(const std::string& line) {
    std::u32string letters;
    for (const char32_t symbol : ToUtf32(line)) {
        if (IsLetter(symbol)) {
            letters.push_back(ToLower(symbol));
        }
    }
    if (letters.size() > 2) {
        letters = letters.substr(letters.size() - 2);
    }
    return ToUtf8(letters);
}

}  // namespace text_utils
