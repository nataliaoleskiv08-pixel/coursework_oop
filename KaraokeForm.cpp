#include "KaraokeForm.h"

#include <string>

#include "Theme.h"
#include "Utf8Convert.h"

using namespace System;
using namespace System::Collections::Generic;
using namespace System::Windows::Forms;

namespace SongEditor {

KaraokeForm::KaraokeForm(const Song* song) {
    lines_ = gcnew List<LyricLine^>();
    verseTotal_ = 0;
    currentIndex_ = 0;
    state_ = State::Idle;
    verseText_ = "";

    BuildLyrics(song);
    BuildInterface(ToManagedString(song->GetTitle()));
    ShowIdleScreen();
    UpdateButtons();
}

void KaraokeForm::BuildLyrics(const Song* song) {
    verseTotal_ = static_cast<int>(song->GetVerseCount());
    for (const Verse& verse : song->GetVerses()) {
        bool first = true;
        for (const std::string& line : verse.GetLines()) {
            LyricLine^ item = gcnew LyricLine();
            item->VerseNumber = verse.GetNumber();
            item->IsFirstInVerse = first;
            item->Text = ToManagedString(line);
            lines_->Add(item);
            first = false;
        }
    }
}

void KaraokeForm::BuildInterface(String^ songTitle) {
    this->SuspendLayout();
    this->Text = "♫ Караоке — " + songTitle;
    this->ClientSize = System::Drawing::Size(980, 620);
    this->MinimumSize = System::Drawing::Size(820, 540);
    this->StartPosition = FormStartPosition::CenterParent;
    this->ShowInTaskbar = false;
    this->MinimizeBox = false;
    this->BackColor = Theme::DeepPurple();
    this->Icon = Theme::MakeAppIcon();
    this->Font = gcnew System::Drawing::Font("Segoe UI", 10.0f);

    // ---- «Сцена»: темно-фіолетовий градієнт з нотами
    stage_ = gcnew GradientPanel();
    stage_->Dock = DockStyle::Fill;
    stage_->StartColor = System::Drawing::Color::FromArgb(22, 8, 48);
    stage_->MiddleColor = System::Drawing::Color::FromArgb(62, 22, 112);
    stage_->EndColor = System::Drawing::Color::FromArgb(128, 26, 108);
    stage_->Angle = 90.0f;
    stage_->ShowNotes = true;
    stage_->NotesAlpha = 22;

    // ---- Верхній напис: куплет і номер рядка
    verseLabel_ = gcnew Label();
    verseLabel_->Dock = DockStyle::Top;
    verseLabel_->Height = 60;
    verseLabel_->BackColor = System::Drawing::Color::Transparent;
    verseLabel_->ForeColor = System::Drawing::Color::FromArgb(232, 218, 255);
    verseLabel_->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
    verseLabel_->Font = gcnew System::Drawing::Font("Segoe UI", 15.0f, System::Drawing::FontStyle::Bold);

    // ---- Поточний (яскравий) і наступний (приглушений сірий) рядки
    currentLabel_ = gcnew Label();
    currentLabel_->Dock = DockStyle::Fill;
    currentLabel_->AutoSize = false;
    currentLabel_->BackColor = System::Drawing::Color::Transparent;
    currentLabel_->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
    currentLabel_->Font = gcnew System::Drawing::Font("Segoe UI", 36.0f, System::Drawing::FontStyle::Bold);

    nextLabel_ = gcnew Label();
    nextLabel_->Dock = DockStyle::Fill;
    nextLabel_->AutoSize = false;
    nextLabel_->BackColor = System::Drawing::Color::Transparent;
    nextLabel_->ForeColor = System::Drawing::Color::FromArgb(160, 160, 172);
    nextLabel_->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
    nextLabel_->Font = gcnew System::Drawing::Font("Segoe UI", 20.0f);

    TableLayoutPanel^ center = gcnew TableLayoutPanel();
    center->Dock = DockStyle::Fill;
    center->BackColor = System::Drawing::Color::Transparent;
    center->ColumnCount = 1;
    center->RowCount = 2;
    center->ColumnStyles->Add(gcnew ColumnStyle(SizeType::Percent, 100.0f));
    center->RowStyles->Add(gcnew RowStyle(SizeType::Percent, 62.0f));
    center->RowStyles->Add(gcnew RowStyle(SizeType::Percent, 38.0f));
    center->Padding = System::Windows::Forms::Padding(24, 0, 24, 0);
    center->Controls->Add(currentLabel_, 0, 0);
    center->Controls->Add(nextLabel_, 0, 1);

    // ---- Прогрес
    progressBar_ = gcnew NoteProgressBar();
    progressBar_->Dock = DockStyle::Top;
    progressBar_->Height = 20;
    progressBar_->Maximum = lines_->Count > 0 ? lines_->Count : 1;

    // ---- Кнопки керування
    startButton_ = gcnew Button();
    startButton_->Text = "Старт";
    startButton_->Size = System::Drawing::Size(160, 48);
    startButton_->Margin = System::Windows::Forms::Padding(3, 3, 12, 3);
    Theme::StyleButton(startButton_, Theme::Green(), 0xE768, "►");
    startButton_->Click += gcnew EventHandler(this, &KaraokeForm::OnStart);

    pauseButton_ = gcnew Button();
    pauseButton_->Text = "Пауза";
    pauseButton_->Size = System::Drawing::Size(150, 48);
    pauseButton_->Margin = System::Windows::Forms::Padding(3, 3, 12, 3);
    Theme::StyleButton(pauseButton_, Theme::Amber(), 0xE769, "II");
    pauseButton_->Click += gcnew EventHandler(this, &KaraokeForm::OnPause);

    resetButton_ = gcnew Button();
    resetButton_->Text = "Скинути";
    resetButton_->Size = System::Drawing::Size(150, 48);
    resetButton_->Margin = System::Windows::Forms::Padding(3, 3, 12, 3);
    Theme::StyleButton(resetButton_, Theme::Coral(), 0xE72C, "↺");
    resetButton_->Click += gcnew EventHandler(this, &KaraokeForm::OnReset);

    Label^ intervalLabel = gcnew Label();
    intervalLabel->Text = "Інтервал між рядками, с:";
    intervalLabel->AutoSize = true;
    intervalLabel->BackColor = System::Drawing::Color::Transparent;
    intervalLabel->ForeColor = System::Drawing::Color::FromArgb(232, 218, 255);
    intervalLabel->Margin = System::Windows::Forms::Padding(18, 15, 3, 0);

    intervalNumeric_ = gcnew NumericUpDown();
    intervalNumeric_->DecimalPlaces = 1;
    intervalNumeric_->Increment = Decimal(0.5);
    intervalNumeric_->Minimum = Decimal(1);
    intervalNumeric_->Maximum = Decimal(15);
    intervalNumeric_->Value = Decimal(3);
    intervalNumeric_->Width = 80;
    intervalNumeric_->Font = gcnew System::Drawing::Font("Segoe UI", 12.0f);
    intervalNumeric_->BackColor = System::Drawing::Color::FromArgb(52, 28, 96);
    intervalNumeric_->ForeColor = System::Drawing::Color::White;
    intervalNumeric_->Margin = System::Windows::Forms::Padding(3, 10, 3, 0);

    FlowLayoutPanel^ controls = gcnew FlowLayoutPanel();
    controls->Dock = DockStyle::Fill;
    controls->BackColor = System::Drawing::Color::Transparent;
    controls->Padding = System::Windows::Forms::Padding(16, 12, 8, 8);
    controls->Controls->Add(startButton_);
    controls->Controls->Add(pauseButton_);
    controls->Controls->Add(resetButton_);
    controls->Controls->Add(intervalLabel);
    controls->Controls->Add(intervalNumeric_);

    // ---- Еквалайзер, що «танцює» під час співу
    equalizer_ = gcnew EqualizerBars();
    equalizer_->Dock = DockStyle::Right;
    equalizer_->Width = 230;

    Panel^ bottomPanel = gcnew Panel();
    bottomPanel->Dock = DockStyle::Bottom;
    bottomPanel->Height = 136;
    bottomPanel->BackColor = System::Drawing::Color::Transparent;
    // Заповнювач додається першим, далі — елементи, закріплені по краях.
    bottomPanel->Controls->Add(controls);
    bottomPanel->Controls->Add(equalizer_);
    bottomPanel->Controls->Add(progressBar_);

    // ---- Підказки
    toolTip_ = gcnew System::Windows::Forms::ToolTip();
    toolTip_->SetToolTip(startButton_, "Запустити або продовжити показ тексту");
    toolTip_->SetToolTip(pauseButton_, "Поставити показ на паузу");
    toolTip_->SetToolTip(resetButton_, "Повернутися до початку пісні");
    toolTip_->SetToolTip(intervalNumeric_,
                         "Тривалість показу одного рядка (між куплетами — додаткова пауза)");
    toolTip_->SetToolTip(nextLabel_, "Наступний рядок — підказка для співу");
    toolTip_->SetToolTip(progressBar_, "Прогрес виконання пісні");

    stage_->Controls->Add(center);
    stage_->Controls->Add(bottomPanel);
    stage_->Controls->Add(verseLabel_);
    this->Controls->Add(stage_);

    timer_ = gcnew System::Windows::Forms::Timer();
    timer_->Tick += gcnew EventHandler(this, &KaraokeForm::OnTick);

    this->ResumeLayout(false);
}

void KaraokeForm::OnFormClosing(FormClosingEventArgs^ e) {
    timer_->Stop();
    equalizer_->SetActive(false);
    Form::OnFormClosing(e);
}

void KaraokeForm::OnStart(Object^ sender, EventArgs^ e) {
    if (lines_->Count == 0) {
        return;
    }
    if (state_ == State::Finished) {
        OnReset(sender, e);
    }
    if (state_ == State::Idle) {
        currentIndex_ = 0;
    }
    state_ = State::Playing;
    ShowCurrentLine();
    ScheduleNextTick();
    UpdateButtons();
}

void KaraokeForm::OnPause(Object^ sender, EventArgs^ e) {
    if (state_ != State::Playing) {
        return;
    }
    timer_->Stop();
    state_ = State::Paused;
    verseLabel_->Text = verseText_ + "   —   пауза";
    UpdateButtons();
}

void KaraokeForm::OnReset(Object^ sender, EventArgs^ e) {
    timer_->Stop();
    currentIndex_ = 0;
    state_ = State::Idle;
    ShowIdleScreen();
    UpdateButtons();
}

void KaraokeForm::OnTick(Object^ sender, EventArgs^ e) {
    timer_->Stop();
    if (state_ != State::Playing) {
        return;
    }
    if (currentIndex_ + 1 >= lines_->Count) {
        Finish();
        return;
    }
    ++currentIndex_;
    ShowCurrentLine();
    ScheduleNextTick();
}

void KaraokeForm::ShowCurrentLine() {
    LyricLine^ current = lines_[currentIndex_];
    verseText_ = "♪  Куплет " + Convert::ToString(current->VerseNumber) + " з " +
                 Convert::ToString(verseTotal_) + "   ·   рядок " +
                 Convert::ToString(currentIndex_ + 1) + " з " +
                 Convert::ToString(lines_->Count) + "  ♪";
    verseLabel_->Text = verseText_;

    // Кожен куплет має свій яскравий колір тексту.
    currentLabel_->ForeColor = Theme::NoteColor(current->VerseNumber - 1);
    currentLabel_->Text = current->Text;

    if (currentIndex_ + 1 < lines_->Count) {
        nextLabel_->Text = lines_[currentIndex_ + 1]->Text;
    } else {
        nextLabel_->Text = "(це останній рядок)";
    }
    progressBar_->Value = currentIndex_ + 1;
}

void KaraokeForm::ShowIdleScreen() {
    verseText_ = "♫  Готово до співу  ♫";
    verseLabel_->Text = verseText_;
    currentLabel_->ForeColor = System::Drawing::Color::White;
    if (lines_->Count == 0) {
        currentLabel_->Text = "Текст пісні порожній";
        nextLabel_->Text = "";
    } else {
        currentLabel_->Text = "Натисніть «Старт»";
        nextLabel_->Text = lines_[0]->Text;
    }
    progressBar_->Value = 0;
}

void KaraokeForm::ScheduleNextTick() {
    int delay = static_cast<int>(Decimal::ToDouble(intervalNumeric_->Value) * 1000.0);
    const bool nextStartsVerse =
        currentIndex_ + 1 < lines_->Count && lines_[currentIndex_ + 1]->IsFirstInVerse;
    if (nextStartsVerse) {
        delay += kVersePauseMs;  // коротка перерва між куплетами
    }
    timer_->Interval = delay;
    timer_->Start();
}

void KaraokeForm::Finish() {
    timer_->Stop();
    state_ = State::Finished;
    verseText_ = "♫  Пісню завершено  ♫";
    verseLabel_->Text = verseText_;
    currentLabel_->ForeColor = Theme::Yellow();
    currentLabel_->Text = "Кінець пісні";
    nextLabel_->Text = "";
    UpdateButtons();
    MessageBox::Show(this,
                     "Пісня закінчилась. Дякуємо за виконання!\n"
                     "Натисніть «Скинути» або «Старт», щоб заспівати ще раз.",
                     "Караоке", MessageBoxButtons::OK, MessageBoxIcon::Information);
}

void KaraokeForm::UpdateButtons() {
    startButton_->Enabled = lines_->Count > 0 && state_ != State::Playing;
    if (state_ == State::Paused) {
        startButton_->Text = "Продовжити";
    } else {
        startButton_->Text = "Старт";
    }
    pauseButton_->Enabled = state_ == State::Playing;
    resetButton_->Enabled = state_ != State::Idle;
    equalizer_->SetActive(state_ == State::Playing);

    // Недоступні кнопки приглушуються, щоб було видно поточний стан.
    startButton_->BackColor = startButton_->Enabled ? Theme::Green()
                                                    : System::Drawing::Color::FromArgb(90, 80, 110);
    pauseButton_->BackColor = pauseButton_->Enabled ? Theme::Amber()
                                                    : System::Drawing::Color::FromArgb(90, 80, 110);
    resetButton_->BackColor = resetButton_->Enabled ? Theme::Coral()
                                                    : System::Drawing::Color::FromArgb(90, 80, 110);
}

}  // namespace SongEditor
