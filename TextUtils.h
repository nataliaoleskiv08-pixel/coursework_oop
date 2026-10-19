#ifndef TEXT_UTILS_H
#define TEXT_UTILS_H

#include <string>
#include <vector>

/// Допоміжні функції для роботи з UTF-8 текстом (українська та латиниця).
namespace text_utils {

std::u32string ToUtf32(const std::string& utf8);
std::string ToUtf8(const std::u32string& utf32);

char32_t ToLower(char32_t symbol);
std::string ToLower(const std::string& utf8);

bool IsLetter(char32_t symbol);
bool IsVowel(char32_t symbol);

/// Видаляє пробільні символи з обох кінців рядка.
std::string Trim(const std::string& text);

/// Розбиває рядок на слова (літери та апостроф усередині слова).
std::vector<std::string> ExtractWords(const std::string& text);

/// Порівняння за українським алфавітом: <0, 0 або >0.
int CompareAlphabetical(const std::string& left, const std::string& right);

/// Ключ рими: останні дві літери останнього слова рядка (у нижньому регістрі).
std::string RhymeKey(const std::string& line);

}  // namespace text_utils

#endif  // TEXT_UTILS_H
