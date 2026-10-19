#include "Song.h"

#include <algorithm>
#include <istream>
#include <map>
#include <ostream>
#include <stdexcept>
#include <utility>

#include "TextUtils.h"

namespace {

/// Зчитує рядок виду «<ключ> значення». Порожні рядки пропускаються.
bool ReadHeaderValue(std::istream& in, const std::string& key,
                     std::string& value) {
    std::string line;
    while (std::getline(in, line)) {
        if (line.compare(0, 3, "\xEF\xBB\xBF") == 0) {
            line.erase(0, 3);  // BOM на початку UTF-8 файлу
        }
        line = text_utils::Trim(line);
        if (line.empty()) {
            continue;
        }
        if (line.compare(0, key.size(), key) != 0) {
            return false;
        }
        value = text_utils::Trim(line.substr(key.size()));
        return true;
    }
    return false;
}

struct SonnetPattern {
    const char* scheme;
    const char* type;
};

// Схеми записані в нормалізованому вигляді (літери за порядком появи).
const SonnetPattern kSonnetPatterns[] = {
    {"ABBAABBACDCDCD", u8"Італійський (петрарківський)"},
    {"ABBAABBACDECDE", u8"Італійський (петрарківський)"},
    {"ABBAABBACDEDCE", u8"Італійський (петрарківський)"},
    {"ABABCDCDEFEFGG", u8"Англійський (шекспірівський)"},
    {"ABABBCBCCDCDEE", u8"Спенсерівський"},
    {"ABBAABBACCDEED", u8"Французький"},
    {"ABBAABBACCDEDE", u8"Французький"},
};

}  // namespace

Song::Song() = default;

Song::Song(std::string author, std::string title, std::vector<Verse> verses)
    : author_(std::move(author)),
      title_(std::move(title)),
      verses_(std::move(verses)) {
    RenumberVerses();
}

Song::Song(const Song& other)
    : author_(other.author_), title_(other.title_), verses_(other.verses_) {}

const std::string& Song::GetAuthor() const { return author_; }

const std::string& Song::GetTitle() const { return title_; }

std::size_t Song::GetVerseCount() const { return verses_.size(); }

const std::vector<Verse>& Song::GetVerses() const { return verses_; }

const Verse& Song::GetVerse(std::size_t index) const {
    return verses_.at(index);
}

std::vector<std::size_t> Song::FindWordInVerse(std::size_t verseIndex,
                                               const std::string& word) const {
    return verses_.at(verseIndex).FindWord(word);
}

std::vector<LineReference> Song::FindWordInSong(const std::string& word) const {
    std::vector<LineReference> result;
    for (std::size_t v = 0; v < verses_.size(); ++v) {
        for (const std::size_t l : verses_[v].FindWord(word)) {
            result.push_back({v, l, verses_[v].GetLine(l)});
        }
    }
    return result;
}

void Song::ReplaceLine(std::size_t verseIndex, std::size_t lineIndex,
                       const std::string& newLine) {
    verses_.at(verseIndex).ReplaceLine(lineIndex, newLine);
}

void Song::SwapVerses(std::size_t first, std::size_t second) {
    if (first >= verses_.size() || second >= verses_.size()) {
        throw std::out_of_range(u8"Номер куплета поза межами пісні");
    }
    std::swap(verses_[first], verses_[second]);
    RenumberVerses();
}

void Song::MoveVerse(std::size_t from, std::size_t to) {
    if (from >= verses_.size() || to >= verses_.size()) {
        throw std::out_of_range(u8"Номер куплета поза межами пісні");
    }
    if (from < to) {
        std::rotate(verses_.begin() + from, verses_.begin() + from + 1,
                    verses_.begin() + to + 1);
    } else if (to < from) {
        std::rotate(verses_.begin() + to, verses_.begin() + from,
                    verses_.begin() + from + 1);
    }
    RenumberVerses();
}

std::vector<LineReference> Song::GetLastLines() const {
    std::vector<LineReference> result;
    for (std::size_t v = 0; v < verses_.size(); ++v) {
        if (verses_[v].GetLineCount() == 0) {
            continue;
        }
        const std::size_t last = verses_[v].GetLineCount() - 1;
        result.push_back({v, last, verses_[v].GetLastLine()});
    }
    return result;
}

SonnetAnalysis Song::AnalyzeSonnet() const {
    std::vector<std::string> lines;
    for (const Verse& verse : verses_) {
        lines.insert(lines.end(), verse.GetLines().begin(),
                     verse.GetLines().end());
    }

    if (lines.size() != 14) {
        return {false,
                u8"Не є сонетом: рядків " + std::to_string(lines.size()) +
                    u8" (потрібно 14)",
                ""};
    }

    // Нормалізована схема римування: літери призначаються за порядком появи ключа.
    std::map<std::string, char> letterByKey;
    std::string scheme;
    for (std::size_t i = 0; i < lines.size(); ++i) {
        std::string key = text_utils::RhymeKey(lines[i]);
        if (key.empty()) {
            key = "#" + std::to_string(i);  // рядок без літер римуватися не може
        }
        auto it = letterByKey.find(key);
        if (it == letterByKey.end()) {
            const char letter = static_cast<char>('A' + letterByKey.size());
            it = letterByKey.emplace(key, letter).first;
        }
        scheme.push_back(it->second);
    }

    for (const SonnetPattern& pattern : kSonnetPatterns) {
        if (scheme == pattern.scheme) {
            return {true, std::string(u8"Сонет: ") + pattern.type, scheme};
        }
    }
    return {true, u8"Сонет (14 рядків), але з нестандартною схемою римування",
            scheme};
}

std::vector<std::string> Song::GetSortedVowelWords() const {
    std::vector<std::string> words;
    for (const Verse& verse : verses_) {
        for (const std::string& line : verse.GetLines()) {
            for (const std::string& word : text_utils::ExtractWords(line)) {
                const std::u32string symbols = text_utils::ToUtf32(word);
                if (!symbols.empty() && text_utils::IsVowel(symbols.front())) {
                    words.push_back(text_utils::ToLower(word));
                }
            }
        }
    }
    SelectionSort(words);
    return words;
}

void Song::RenumberVerses() {
    for (std::size_t i = 0; i < verses_.size(); ++i) {
        verses_[i].SetNumber(static_cast<int>(i) + 1);
    }
}

// Метод простої вибірки: на кожному кроці знаходимо мінімум серед
// необроблених елементів і ставимо його на поточну позицію.
void Song::SelectionSort(std::vector<std::string>& words) {
    for (std::size_t i = 0; i + 1 < words.size(); ++i) {
        std::size_t minIndex = i;
        for (std::size_t j = i + 1; j < words.size(); ++j) {
            if (text_utils::CompareAlphabetical(words[j], words[minIndex]) < 0) {
                minIndex = j;
            }
        }
        if (minIndex != i) {
            std::swap(words[i], words[minIndex]);
        }
    }
}

// Формат файлу:
//   Автор: ...
//   Назва: ...
//   Куплетів: N
//   <порожній рядок>
//   #1 ... (куплети, розділені порожніми рядками)
std::istream& operator>>(std::istream& in, Song& song) {
    std::string author;
    std::string title;
    std::string countText;
    if (!ReadHeaderValue(in, u8"Автор:", author) ||
        !ReadHeaderValue(in, u8"Назва:", title) ||
        !ReadHeaderValue(in, u8"Куплетів:", countText)) {
        in.setstate(std::ios::failbit);
        return in;
    }

    std::size_t count = 0;
    try {
        count = std::stoul(countText);
    } catch (const std::exception&) {
        in.setstate(std::ios::failbit);
        return in;
    }

    std::vector<Verse> verses;
    verses.reserve(count);
    for (std::size_t i = 0; i < count; ++i) {
        Verse verse;
        if (!(in >> verse)) {
            return in;
        }
        verses.push_back(std::move(verse));
    }

    song = Song(std::move(author), std::move(title), std::move(verses));
    return in;
}

std::ostream& operator<<(std::ostream& out, const Song& song) {
    out << u8"Автор: " << song.author_ << '\n'
        << u8"Назва: " << song.title_ << '\n'
        << u8"Куплетів: " << song.verses_.size() << "\n\n";
    for (const Verse& verse : song.verses_) {
        out << verse;
    }
    return out;
}
