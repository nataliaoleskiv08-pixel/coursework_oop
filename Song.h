#ifndef SONG_H
#define SONG_H

#include <cstddef>
#include <iosfwd>
#include <string>
#include <vector>

#include "Verse.h"

/// Посилання на рядок пісні (куплет + рядок) разом із його текстом.
struct LineReference {
    std::size_t verseIndex;
    std::size_t lineIndex;
    std::string text;
};

/// Результат аналізу пісні на відповідність сонету.
struct SonnetAnalysis {
    bool isSonnet;
    std::string type;
    std::string rhymeScheme;
};

/// Пісня: автор, назва та куплети.
///
/// Куплети зберігаються у std::vector<Verse>: потрібен швидкий доступ за
/// індексом, а переставляння виконуються стандартними алгоритмами (std::rotate,
/// std::swap). Кількість куплетів не зберігається окремо, а обчислюється
/// з розміру вектора, тому вона завжди узгоджена з даними.
class Song {
public:
    Song();
    Song(std::string author, std::string title, std::vector<Verse> verses);
    Song(const Song& other);
    Song& operator=(const Song& other) = default;
    ~Song() = default;

    const std::string& GetAuthor() const;
    const std::string& GetTitle() const;
    std::size_t GetVerseCount() const;
    const std::vector<Verse>& GetVerses() const;
    const Verse& GetVerse(std::size_t index) const;

    // a. Пошук
    std::vector<std::size_t> FindWordInVerse(std::size_t verseIndex,
                                             const std::string& word) const;
    std::vector<LineReference> FindWordInSong(const std::string& word) const;

    // b. Заміна рядка куплета
    void ReplaceLine(std::size_t verseIndex, std::size_t lineIndex,
                     const std::string& newLine);

    // c. Переставляння куплетів
    void SwapVerses(std::size_t first, std::size_t second);
    void MoveVerse(std::size_t from, std::size_t to);

    // d. Останні рядки всіх куплетів
    std::vector<LineReference> GetLastLines() const;

    // e. Сонет
    SonnetAnalysis AnalyzeSonnet() const;

    // f. Слова на голосну, відсортовані методом простої вибірки
    std::vector<std::string> GetSortedVowelWords() const;

    friend std::istream& operator>>(std::istream& in, Song& song);
    friend std::ostream& operator<<(std::ostream& out, const Song& song);

private:
    void RenumberVerses();
    static void SelectionSort(std::vector<std::string>& words);

    std::string author_;
    std::string title_;
    std::vector<Verse> verses_;
};

#endif  // SONG_H
