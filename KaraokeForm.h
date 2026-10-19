#pragma once

#using <System.dll>
#using <System.Drawing.dll>
#using <System.Windows.Forms.dll>

#include "EqualizerBars.h"
#include "GradientPanel.h"
#include "NoteProgressBar.h"
#include "Song.h"

namespace SongEditor {

/// Інтерактивний режим «Караоке»: поточний рядок великим яскравим шрифтом,
/// наступний — приглушеним сірим кольором. Рядки перемикаються автоматично.
public ref class KaraokeForm : public System::Windows::Forms::Form {
public:
    explicit KaraokeForm(const Song* song);

protected:
    virtual void OnFormClosing(System::Windows::Forms::FormClosingEventArgs^ e) override;

private:
    enum class State { Idle, Playing, Paused, Finished };

    ref class LyricLine {
    public:
        int VerseNumber;
        bool IsFirstInVerse;
        System::String^ Text;
    };

    literal int kVersePauseMs = 1500;

    void BuildLyrics(const Song* song);
    void BuildInterface(System::String^ songTitle);

    void OnStart(System::Object^ sender, System::EventArgs^ e);
    void OnPause(System::Object^ sender, System::EventArgs^ e);
    void OnReset(System::Object^ sender, System::EventArgs^ e);
    void OnTick(System::Object^ sender, System::EventArgs^ e);

    void ShowCurrentLine();
    void ShowIdleScreen();
    void ScheduleNextTick();
    void Finish();
    void UpdateButtons();

    System::Collections::Generic::List<LyricLine^>^ lines_;
    int verseTotal_;
    int currentIndex_;
    State state_;
    System::String^ verseText_;

    GradientPanel^ stage_;
    System::Windows::Forms::Label^ verseLabel_;
    System::Windows::Forms::Label^ currentLabel_;
    System::Windows::Forms::Label^ nextLabel_;
    NoteProgressBar^ progressBar_;
    EqualizerBars^ equalizer_;
    System::Windows::Forms::NumericUpDown^ intervalNumeric_;
    System::Windows::Forms::Button^ startButton_;
    System::Windows::Forms::Button^ pauseButton_;
    System::Windows::Forms::Button^ resetButton_;
    System::Windows::Forms::Timer^ timer_;
    System::Windows::Forms::ToolTip^ toolTip_;
};

}  // namespace SongEditor
