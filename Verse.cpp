#include "Verse.h"

#include <istream>
#include <ostream>
#include <stdexcept>
#include <utility>

#include "TextUtils.h"

Verse::Verse() : number_(0) {}

Verse::Verse(int number, std::vector<std::string> lines)
    : number_(number), lines_(std::move(lines)) {}

Verse::Verse(const Verse& other)
    : number_(other.number_), lines_(other.lines_) {}

int Verse::GetNumber() const { return number_; }

void Verse::SetNumber(int number) { number_ = number; }

const std::vector<std::string>& Verse::GetLines() const { return lines_; }

std::size_t Verse::GetLineCount() const { return lines_.size(); }

const std::string& Verse::GetLine(std::size_t index) const {
    return lines_.at(index);
}

const std::string& Verse::GetLastLine() const {
    if (lines_.empty()) {
        throw std::out_of_range(u8"Куплет не містить рядків");
    }
    return lines_.back();
}

std::vector<std::size_t> Verse::FindWord(const std::string& word) const {
    std::vector<std::size_t> result;
    const std::string target = text_utils::ToLower(text_utils::Trim(word));
    if (target.empty()) {
        return result;
    }
    for (std::size_t i = 0; i < lines_.size(); ++i) {
        for (const std::string& candidate : text_utils::ExtractWords(lines_[i])) {
            if (text_utils::ToLower(candidate) == target) {
                result.push_back(i);
                break;
            }
        }
    }
    return result;
}

bool Verse::ContainsWord(const std::string& word) const {
    return !FindWord(word).empty();
}

void Verse::ReplaceLine(std::size_t index, const std::string& newLine) {
    lines_.at(index) = newLine;
}

// Формат куплета у файлі:
//   #<номер>
//   рядок 1
//   рядок 2
//   (порожній рядок — кінець куплета)
std::istream& operator>>(std::istream& in, Verse& verse) {
    std::string line;
    bool headerFound = false;
    while (std::getline(in, line)) {
        line = text_utils::Trim(line);
        if (!line.empty()) {
            headerFound = true;
            break;
        }
    }
    if (!headerFound || line[0] != '#') {
        in.setstate(std::ios::failbit);
        return in;
    }

    int number = 0;
    try {
        number = std::stoi(line.substr(1));
    } catch (const std::exception&) {
        in.setstate(std::ios::failbit);
        return in;
    }

    std::vector<std::string> lines;
    while (std::getline(in, line)) {
        line = text_utils::Trim(line);
        if (line.empty()) {
            break;
        }
        lines.push_back(line);
    }
    if (in.fail() && in.eof()) {
        in.clear(std::ios::eofbit);  // кінець файлу після останнього куплета — не помилка
    }

    verse = Verse(number, std::move(lines));
    return in;
}

std::ostream& operator<<(std::ostream& out, const Verse& verse) {
    out << '#' << verse.number_ << '\n';
    for (const std::string& line : verse.lines_) {
        out << line << '\n';
    }
    out << '\n';
    return out;
}
