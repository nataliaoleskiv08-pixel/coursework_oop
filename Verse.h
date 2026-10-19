#ifndef VERSE_H
#define VERSE_H

#include <cstddef>
#include <iosfwd>
#include <string>
#include <vector>

/// Куплет пісні: порядковий номер та масив рядків.
///
/// Рядки зберігаються в std::vector<std::string>: куплет має невелику
/// фіксовану послідовність рядків із частим доступом за індексом.
class Verse {
public:
    Verse();
    Verse(int number, std::vector<std::string> lines);
    Verse(const Verse& other);
    Verse& operator=(const Verse& other) = default;
    ~Verse() = default;

    int GetNumber() const;
    void SetNumber(int number);

    const std::vector<std::string>& GetLines() const;
    std::size_t GetLineCount() const;
    const std::string& GetLine(std::size_t index) const;
    const std::string& GetLastLine() const;

    /// Індекси рядків куплета, що містять слово (без урахування регістру).
    std::vector<std::size_t> FindWord(const std::string& word) const;
    bool ContainsWord(const std::string& word) const;

    /// Замінює рядок куплета. Кидає std::out_of_range при хибному індексі.
    void ReplaceLine(std::size_t index, const std::string& newLine);

    friend std::istream& operator>>(std::istream& in, Verse& verse);
    friend std::ostream& operator<<(std::ostream& out, const Verse& verse);

private:
    int number_;
    std::vector<std::string> lines_;
};

#endif  // VERSE_H
