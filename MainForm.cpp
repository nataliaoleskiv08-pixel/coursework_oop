#include "MainForm.h"

#include <fstream>
#include <stdexcept>
#include <vector>
#include <vcclr.h>

#include "InputDialog.h"
#include "KaraokeForm.h"
#include "TextUtils.h"
#include "Theme.h"
#include "Utf8Convert.h"

using namespace System;
using namespace System::Collections::Generic;
using namespace System::Drawing;
using namespace System::Windows::Forms;

namespace SongEditor {

namespace {

/// Зчитує пісню з файлу (шлях може містити українські літери).
bool ReadSongFile(String^ path, Song& song) {
    pin_ptr<const wchar_t> pinned = PtrToStringChars(path);
    const wchar_t* nativePath = pinned;
    std::ifstream in(nativePath);
    Song loaded;
    if (!in || !(in >> loaded)) {
        return false;
    }
    song = loaded;
    return true;
}

bool WriteSongFile(String^ path, const Song& song) {
    pin_ptr<const wchar_t> pinned = PtrToStringChars(path);
    const wchar_t* nativePath = pinned;
    std::ofstream out(nativePath);
    if (!out) {
        return false;
    }
    out << song;
    return out.good();
}

String^ Num(std::size_t value) {
    return Convert::ToString(static_cast<int>(value));
}

Keys Ctrl(Keys key) { return static_cast<Keys>(Keys::Control | key); }

Keys CtrlShift(Keys key) {
    return static_cast<Keys>(Keys::Control | Keys::Shift | key);
}

Keys CtrlAlt(Keys key) { return static_cast<Keys>(Keys::Control | Keys::Alt | key); }

}  // namespace

MainForm::MainForm() {
    song_ = new Song();
    currentFilePath_ = nullptr;
    isModified_ = false;
    BuildInterface();
}

MainForm::~MainForm() { this->!MainForm(); }

MainForm::!MainForm() {
    delete song_;
    song_ = nullptr;
}

// ------------------------------------------------------------ інтерфейс

ToolStripMenuItem^ MainForm::CreateMenuItem(String^ text, String^ tip, Keys shortcut,
                                            EventHandler^ handler) {
    ToolStripMenuItem^ item = gcnew ToolStripMenuItem(text);
    item->ToolTipText = tip;
    item->ShortcutKeys = shortcut;
    item->ForeColor = Theme::Ink();  // пункти випадаючих меню — темний текст на світлому тлі
    item->Click += handler;
    item->MouseEnter += gcnew EventHandler(this, &MainForm::OnItemHover);
    item->MouseLeave += gcnew EventHandler(this, &MainForm::OnItemLeave);
    return item;
}

ToolStripButton^ MainForm::CreateToolButton(String^ text, String^ tip, int glyphCode,
                                            String^ fallback, Color badgeColor,
                                            EventHandler^ handler) {
    ToolStripButton^ button = gcnew ToolStripButton(text);
    button->ToolTipText = tip;
    button->Image = Theme::MakeBadgeIcon(glyphCode, fallback, badgeColor, 32);
    button->DisplayStyle = ToolStripItemDisplayStyle::ImageAndText;
    button->TextImageRelation = TextImageRelation::ImageAboveText;
    button->ForeColor = Theme::Ink();
    button->Padding = System::Windows::Forms::Padding(6, 4, 6, 2);
    button->Click += handler;
    button->MouseEnter += gcnew EventHandler(this, &MainForm::OnItemHover);
    button->MouseLeave += gcnew EventHandler(this, &MainForm::OnItemLeave);
    return button;
}

System::Windows::Forms::ContextMenuStrip^ MainForm::CreateContextMenu() {
    System::Windows::Forms::ContextMenuStrip^ menu =
        gcnew System::Windows::Forms::ContextMenuStrip();
    menu->ShowItemToolTips = true;
    menu->Renderer = Theme::CreateRenderer();
    menu->Font = gcnew System::Drawing::Font("Segoe UI", 10.0f);
    menu->Items->Add(CreateMenuItem("Пошук слова у куплеті…", "Знайти слово в одному з куплетів",
                                    Keys::None, gcnew EventHandler(this, &MainForm::OnSearchInVerse)));
    menu->Items->Add(CreateMenuItem("Пошук слова у пісні…", "Знайти слово в усій пісні",
                                    Keys::None, gcnew EventHandler(this, &MainForm::OnSearchInSong)));
    menu->Items->Add(gcnew ToolStripSeparator());
    menu->Items->Add(CreateMenuItem("Замінити рядок куплета…", "Замінити вибраний рядок куплета іншим текстом",
                                    Keys::None, gcnew EventHandler(this, &MainForm::OnReplaceLine)));
    menu->Items->Add(CreateMenuItem("Поміняти куплети місцями…", "Поміняти місцями два куплети",
                                    Keys::None, gcnew EventHandler(this, &MainForm::OnSwapVerses)));
    menu->Items->Add(CreateMenuItem("Переставити куплет…", "Перемістити куплет на нову позицію",
                                    Keys::None, gcnew EventHandler(this, &MainForm::OnMoveVerse)));
    menu->Items->Add(gcnew ToolStripSeparator());
    menu->Items->Add(CreateMenuItem("Останні рядки куплетів", "Вивести останні рядки всіх куплетів",
                                    Keys::None, gcnew EventHandler(this, &MainForm::OnShowLastLines)));
    menu->Items->Add(CreateMenuItem("Чи є пісня сонетом?", "Визначити, чи є пісня сонетом, та його тип",
                                    Keys::None, gcnew EventHandler(this, &MainForm::OnAnalyzeSonnet)));
    menu->Items->Add(CreateMenuItem("Слова на голосну (сортування)",
                                    "Відсортувати слова на голосну (метод простої вибірки)",
                                    Keys::None, gcnew EventHandler(this, &MainForm::OnSortVowelWords)));
    menu->Items->Add(gcnew ToolStripSeparator());
    menu->Items->Add(CreateMenuItem("Режим «Караоке»", "Відкрити інтерактивний режим «Караоке»",
                                    Keys::None, gcnew EventHandler(this, &MainForm::OnStartKaraoke)));
    menu->Items->Add(CreateMenuItem("Очистити результати", "Очистити панель результатів",
                                    Keys::None, gcnew EventHandler(this, &MainForm::OnClearResults)));
    return menu;
}

void MainForm::BuildBanner() {
    banner_ = gcnew GradientPanel();
    banner_->Dock = DockStyle::Top;
    banner_->Height = 88;
    banner_->StartColor = Theme::Violet();
    banner_->MiddleColor = Theme::Magenta();
    banner_->EndColor = Theme::Orange();
    banner_->Angle = 0.0f;
    banner_->ShowNotes = true;
    banner_->NotesAlpha = 34;

    Label^ noteLabel = gcnew Label();
    noteLabel->Text = "♫";
    noteLabel->AutoSize = false;
    noteLabel->Size = System::Drawing::Size(70, 80);
    noteLabel->Location = System::Drawing::Point(14, 4);
    noteLabel->TextAlign = ContentAlignment::MiddleCenter;
    noteLabel->BackColor = System::Drawing::Color::Transparent;
    noteLabel->ForeColor = System::Drawing::Color::White;
    noteLabel->Font = gcnew System::Drawing::Font("Segoe UI Symbol", 44.0f, FontStyle::Bold);

    Label^ titleLabel = gcnew Label();
    titleLabel->Text = "Редактор пісень";
    titleLabel->AutoSize = true;
    titleLabel->Location = System::Drawing::Point(88, 10);
    titleLabel->BackColor = System::Drawing::Color::Transparent;
    titleLabel->ForeColor = System::Drawing::Color::White;
    titleLabel->Font = gcnew System::Drawing::Font("Segoe UI", 24.0f, FontStyle::Bold);

    bannerSubtitle_ = gcnew Label();
    bannerSubtitle_->AutoSize = true;
    bannerSubtitle_->Location = System::Drawing::Point(92, 58);
    bannerSubtitle_->BackColor = System::Drawing::Color::Transparent;
    bannerSubtitle_->ForeColor = System::Drawing::Color::FromArgb(255, 240, 250);
    bannerSubtitle_->Font = gcnew System::Drawing::Font("Segoe UI", 11.0f);

    bannerEqualizer_ = gcnew EqualizerBars();
    bannerEqualizer_->Size = System::Drawing::Size(250, 64);
    bannerEqualizer_->Location = System::Drawing::Point(ClientSize.Width - 270, 12);
    bannerEqualizer_->Anchor = static_cast<AnchorStyles>(AnchorStyles::Top | AnchorStyles::Right);
    bannerEqualizer_->SetActive(true);

    banner_->Controls->Add(noteLabel);
    banner_->Controls->Add(titleLabel);
    banner_->Controls->Add(bannerSubtitle_);
    banner_->Controls->Add(bannerEqualizer_);
}

void MainForm::BuildInterface() {
    this->SuspendLayout();
    this->Text = "Редактор пісень";
    this->ClientSize = System::Drawing::Size(1180, 800);
    this->MinimumSize = System::Drawing::Size(900, 600);
    this->StartPosition = FormStartPosition::CenterScreen;
    this->BackColor = Theme::Lavender();
    this->Font = gcnew System::Drawing::Font("Segoe UI", 10.0f);
    this->Icon = Theme::MakeAppIcon();

    titleFont_ = gcnew System::Drawing::Font("Segoe UI", 22.0f, FontStyle::Bold);
    authorFont_ = gcnew System::Drawing::Font("Segoe UI", 12.0f, FontStyle::Italic);
    symbolFont_ = gcnew System::Drawing::Font("Segoe UI Symbol", 16.0f, FontStyle::Bold);
    verseHeaderFont_ = gcnew System::Drawing::Font("Segoe UI", 12.0f, FontStyle::Bold);
    lineFont_ = gcnew System::Drawing::Font("Segoe UI", 14.0f);

    // ---- Головне меню (темно-фіолетове, з білим текстом верхнього рівня)
    menuStrip_ = gcnew MenuStrip();
    menuStrip_->ShowItemToolTips = true;
    menuStrip_->Renderer = Theme::CreateRenderer();
    menuStrip_->Font = gcnew System::Drawing::Font("Segoe UI", 10.5f);

    ToolStripMenuItem^ fileMenu = gcnew ToolStripMenuItem("&Файл");
    fileMenu->DropDownItems->Add(CreateMenuItem("&Відкрити…", "Відкрити файл із текстом пісні",
                                                Ctrl(Keys::O), gcnew EventHandler(this, &MainForm::OnOpenFile)));
    fileMenu->DropDownItems->Add(CreateMenuItem("&Зберегти", "Зберегти пісню у поточний файл",
                                                Ctrl(Keys::S), gcnew EventHandler(this, &MainForm::OnSaveFile)));
    fileMenu->DropDownItems->Add(CreateMenuItem("Зберегти &як…", "Зберегти пісню в новий файл",
                                                CtrlShift(Keys::S), gcnew EventHandler(this, &MainForm::OnSaveFileAs)));
    fileMenu->DropDownItems->Add(gcnew ToolStripSeparator());
    fileMenu->DropDownItems->Add(CreateMenuItem("&Вийти", "Закрити програму", Keys::None,
                                                gcnew EventHandler(this, &MainForm::OnExit)));

    ToolStripMenuItem^ editMenu = gcnew ToolStripMenuItem("&Редагування");
    editMenu->DropDownItems->Add(CreateMenuItem("&Замінити рядок куплета…", "Замінити вибраний рядок куплета іншим текстом",
                                                Ctrl(Keys::R), gcnew EventHandler(this, &MainForm::OnReplaceLine)));
    editMenu->DropDownItems->Add(CreateMenuItem("&Поміняти куплети місцями…", "Поміняти місцями два куплети",
                                                Ctrl(Keys::T), gcnew EventHandler(this, &MainForm::OnSwapVerses)));
    editMenu->DropDownItems->Add(CreateMenuItem("П&ереставити куплет…", "Перемістити куплет на нову позицію",
                                                Ctrl(Keys::M), gcnew EventHandler(this, &MainForm::OnMoveVerse)));

    ToolStripMenuItem^ searchMenu = gcnew ToolStripMenuItem("&Пошук");
    searchMenu->DropDownItems->Add(CreateMenuItem("Пошук слова у &куплеті…", "Знайти слово в одному з куплетів",
                                                  Ctrl(Keys::F), gcnew EventHandler(this, &MainForm::OnSearchInVerse)));
    searchMenu->DropDownItems->Add(CreateMenuItem("Пошук слова у &пісні…", "Знайти слово в усій пісні",
                                                  CtrlShift(Keys::F), gcnew EventHandler(this, &MainForm::OnSearchInSong)));

    ToolStripMenuItem^ analysisMenu = gcnew ToolStripMenuItem("&Аналіз");
    analysisMenu->DropDownItems->Add(CreateMenuItem("&Останні рядки куплетів", "Вивести останні рядки всіх куплетів",
                                                    Ctrl(Keys::L), gcnew EventHandler(this, &MainForm::OnShowLastLines)));
    analysisMenu->DropDownItems->Add(CreateMenuItem("Чи є пісня &сонетом?", "Визначити, чи є пісня сонетом, та його тип",
                                                    Ctrl(Keys::E), gcnew EventHandler(this, &MainForm::OnAnalyzeSonnet)));
    analysisMenu->DropDownItems->Add(CreateMenuItem("Слова на &голосну (сортування)",
                                                    "Відсортувати слова на голосну (метод простої вибірки)",
                                                    Ctrl(Keys::B), gcnew EventHandler(this, &MainForm::OnSortVowelWords)));
    analysisMenu->DropDownItems->Add(gcnew ToolStripSeparator());
    analysisMenu->DropDownItems->Add(CreateMenuItem("&Очистити результати", "Очистити панель результатів",
                                                    CtrlAlt(Keys::C), gcnew EventHandler(this, &MainForm::OnClearResults)));

    ToolStripMenuItem^ karaokeMenu = gcnew ToolStripMenuItem("&Караоке");
    karaokeMenu->DropDownItems->Add(CreateMenuItem("&Режим «Караоке»", "Відкрити інтерактивний режим «Караоке»",
                                                   Ctrl(Keys::K), gcnew EventHandler(this, &MainForm::OnStartKaraoke)));

    ToolStripMenuItem^ helpMenu = gcnew ToolStripMenuItem("&Довідка");
    helpMenu->DropDownItems->Add(CreateMenuItem("&Про програму", "Інформація про програму",
                                                Keys::F1, gcnew EventHandler(this, &MainForm::OnAbout)));

    array<ToolStripMenuItem^>^ topMenus = gcnew array<ToolStripMenuItem^>{
        fileMenu, editMenu, searchMenu, analysisMenu, karaokeMenu, helpMenu};
    for each (ToolStripMenuItem^ topMenu in topMenus) {
        topMenu->ForeColor = System::Drawing::Color::White;
        topMenu->Padding = System::Windows::Forms::Padding(8, 4, 8, 4);
        menuStrip_->Items->Add(topMenu);
    }

    // ---- Панель інструментів: кольорові круглі значки (дублює основні пункти меню)
    toolStrip_ = gcnew ToolStrip();
    toolStrip_->ShowItemToolTips = true;
    toolStrip_->GripStyle = ToolStripGripStyle::Hidden;
    toolStrip_->Renderer = Theme::CreateRenderer();
    toolStrip_->ImageScalingSize = System::Drawing::Size(32, 32);
    toolStrip_->Font = gcnew System::Drawing::Font("Segoe UI", 9.0f, FontStyle::Bold);
    toolStrip_->Padding = System::Windows::Forms::Padding(8, 4, 8, 2);

    toolStrip_->Items->Add(CreateToolButton("Відкрити", "Відкрити файл із текстом пісні (Ctrl+O)",
                                            0xE8E5, "♪", Theme::Teal(),
                                            gcnew EventHandler(this, &MainForm::OnOpenFile)));
    toolStrip_->Items->Add(CreateToolButton("Зберегти", "Зберегти пісню у поточний файл (Ctrl+S)",
                                            0xE74E, "♪", Theme::Green(),
                                            gcnew EventHandler(this, &MainForm::OnSaveFile)));
    toolStrip_->Items->Add(gcnew ToolStripSeparator());
    toolStrip_->Items->Add(CreateToolButton("У куплеті", "Знайти слово в одному з куплетів (Ctrl+F)",
                                            0xE721, "?", Theme::Purple(),
                                            gcnew EventHandler(this, &MainForm::OnSearchInVerse)));
    toolStrip_->Items->Add(CreateToolButton("У пісні", "Знайти слово в усій пісні (Ctrl+Shift+F)",
                                            0xE721, "?", Theme::Indigo(),
                                            gcnew EventHandler(this, &MainForm::OnSearchInSong)));
    toolStrip_->Items->Add(gcnew ToolStripSeparator());
    toolStrip_->Items->Add(CreateToolButton("Замінити", "Замінити рядок куплета іншим текстом (Ctrl+R)",
                                            0xE70F, "✎", Theme::Orange(),
                                            gcnew EventHandler(this, &MainForm::OnReplaceLine)));
    toolStrip_->Items->Add(CreateToolButton("Поміняти", "Поміняти місцями два куплети (Ctrl+T)",
                                            0xE8AB, "⇄", Theme::Magenta(),
                                            gcnew EventHandler(this, &MainForm::OnSwapVerses)));
    toolStrip_->Items->Add(gcnew ToolStripSeparator());
    toolStrip_->Items->Add(CreateToolButton("Останні рядки", "Вивести останні рядки всіх куплетів (Ctrl+L)",
                                            0xE8FD, "≡", Theme::Teal(),
                                            gcnew EventHandler(this, &MainForm::OnShowLastLines)));
    toolStrip_->Items->Add(CreateToolButton("Сонет?", "Визначити, чи є пісня сонетом, та його тип (Ctrl+E)",
                                            0xE897, "?", Theme::Amber(),
                                            gcnew EventHandler(this, &MainForm::OnAnalyzeSonnet)));
    toolStrip_->Items->Add(CreateToolButton("Сортувати", "Слова на голосну за алфавітом (Ctrl+B)",
                                            0xE8CB, "↕", Theme::Blue(),
                                            gcnew EventHandler(this, &MainForm::OnSortVowelWords)));
    toolStrip_->Items->Add(gcnew ToolStripSeparator());
    toolStrip_->Items->Add(CreateToolButton("Караоке", "Відкрити режим «Караоке» (Ctrl+K)",
                                            0xE720, "♫", Theme::Coral(),
                                            gcnew EventHandler(this, &MainForm::OnStartKaraoke)));

    // ---- Рядок стану
    statusStrip_ = gcnew StatusStrip();
    statusStrip_->Renderer = Theme::CreateRenderer();
    statusStrip_->SizingGrip = false;
    statusLabel_ = gcnew ToolStripStatusLabel("Готово. Відкрийте файл пісні (Ctrl+O).");
    statusLabel_->ForeColor = System::Drawing::Color::White;
    statusLabel_->Font = gcnew System::Drawing::Font("Segoe UI", 10.0f);
    statusStrip_->Items->Add(statusLabel_);

    // ---- Банер
    BuildBanner();

    // ---- Текст пісні: «паперова» картка на лавандовому тлі
    songBox_ = gcnew RichTextBox();
    songBox_->Dock = DockStyle::Fill;
    songBox_->ReadOnly = true;
    songBox_->BorderStyle = System::Windows::Forms::BorderStyle::None;
    songBox_->BackColor = Theme::Cream();
    songBox_->DetectUrls = false;
    songBox_->ContextMenuStrip = CreateContextMenu();

    Panel^ songHost = gcnew Panel();
    songHost->Dock = DockStyle::Fill;
    songHost->BackColor = Theme::Mist();
    songHost->Padding = System::Windows::Forms::Padding(22, 16, 22, 16);
    songHost->Controls->Add(songBox_);

    // ---- Панель результатів: темна, з яскравою шапкою
    resultsBox_ = gcnew TextBox();
    resultsBox_->Dock = DockStyle::Fill;
    resultsBox_->Multiline = true;
    resultsBox_->ReadOnly = true;
    resultsBox_->BorderStyle = System::Windows::Forms::BorderStyle::None;
    resultsBox_->ScrollBars = System::Windows::Forms::ScrollBars::Vertical;
    resultsBox_->BackColor = Theme::Night();
    resultsBox_->ForeColor = System::Drawing::Color::FromArgb(240, 230, 255);
    resultsBox_->Font = gcnew System::Drawing::Font("Segoe UI", 11.0f);
    resultsBox_->ContextMenuStrip = CreateContextMenu();

    Panel^ resultsHost = gcnew Panel();
    resultsHost->Dock = DockStyle::Fill;
    resultsHost->BackColor = Theme::Night();
    resultsHost->Padding = System::Windows::Forms::Padding(14, 8, 14, 8);
    resultsHost->Controls->Add(resultsBox_);

    GradientPanel^ resultsHeader = gcnew GradientPanel();
    resultsHeader->Dock = DockStyle::Top;
    resultsHeader->Height = 34;
    resultsHeader->StartColor = Theme::Purple();
    resultsHeader->EndColor = Theme::Magenta();
    resultsHeader->Angle = 0.0f;
    resultsHeader->ShowNotes = true;
    resultsHeader->NotesAlpha = 30;

    Label^ resultsTitle = gcnew Label();
    resultsTitle->Text = "♬  Результати";
    resultsTitle->AutoSize = true;
    resultsTitle->Location = System::Drawing::Point(12, 5);
    resultsTitle->BackColor = System::Drawing::Color::Transparent;
    resultsTitle->ForeColor = System::Drawing::Color::White;
    resultsTitle->Font = gcnew System::Drawing::Font("Segoe UI", 11.0f, FontStyle::Bold);
    resultsHeader->Controls->Add(resultsTitle);

    splitContainer_ = gcnew SplitContainer();
    splitContainer_->Dock = DockStyle::Fill;
    splitContainer_->Orientation = System::Windows::Forms::Orientation::Horizontal;
    splitContainer_->BackColor = Theme::Magenta();  // колір розділювача
    splitContainer_->SplitterWidth = 5;
    splitContainer_->Panel1->Controls->Add(songHost);
    splitContainer_->Panel2->Controls->Add(resultsHost);
    splitContainer_->Panel2->Controls->Add(resultsHeader);

    // Заповнювач (Fill) додається першим; далі — панелі по краях знизу вгору:
    // рядок стану, панель інструментів, банер, меню (меню опиняється найвище).
    this->Controls->Add(splitContainer_);
    this->Controls->Add(statusStrip_);
    this->Controls->Add(toolStrip_);
    this->Controls->Add(banner_);
    this->Controls->Add(menuStrip_);
    this->MainMenuStrip = menuStrip_;

    this->Load += gcnew EventHandler(this, &MainForm::OnFormLoad);
    this->ResumeLayout(false);
    this->PerformLayout();
}

void MainForm::OnFormLoad(Object^ sender, EventArgs^ e) {
    splitContainer_->SplitterDistance = splitContainer_->Height * 64 / 100;

    // Автоматичне завантаження song.dat: поточна папка, потім папка програми.
    array<String^>^ candidates = gcnew array<String^>{
        "song.dat", System::IO::Path::Combine(Application::StartupPath, "song.dat")};
    for each (String^ candidate in candidates) {
        if (System::IO::File::Exists(candidate) && LoadSong(candidate)) {
            break;
        }
    }
    RefreshView();
}

void MainForm::OnFormClosing(FormClosingEventArgs^ e) {
    if (!MaybeSave()) {
        e->Cancel = true;
    }
    Form::OnFormClosing(e);
}

void MainForm::OnItemHover(Object^ sender, EventArgs^ e) {
    ToolStripItem^ item = dynamic_cast<ToolStripItem^>(sender);
    if (item != nullptr) {
        SetStatus("♪  " + item->ToolTipText);
    }
}

void MainForm::OnItemLeave(Object^ sender, EventArgs^ e) { SetStatus("Готово"); }

// ------------------------------------------------------------ файли

bool MainForm::LoadSong(String^ path) {
    if (!ReadSongFile(path, *song_)) {
        MessageBox::Show(this, "Не вдалося прочитати файл або він має хибний формат:\n" + path,
                         "Помилка", MessageBoxButtons::OK, MessageBoxIcon::Warning);
        return false;
    }
    currentFilePath_ = path;
    isModified_ = false;
    RefreshView();
    SetStatus("Файл завантажено: " + path);
    return true;
}

bool MainForm::SaveSong(String^ path) {
    if (!WriteSongFile(path, *song_)) {
        MessageBox::Show(this, "Не вдалося записати файл:\n" + path, "Помилка",
                         MessageBoxButtons::OK, MessageBoxIcon::Warning);
        return false;
    }
    currentFilePath_ = path;
    isModified_ = false;
    RefreshView();
    SetStatus("Збережено: " + path);
    return true;
}

bool MainForm::SaveCurrent() {
    if (String::IsNullOrEmpty(currentFilePath_)) {
        return SaveAs();
    }
    return SaveSong(currentFilePath_);
}

bool MainForm::SaveAs() {
    SaveFileDialog^ dialog = gcnew SaveFileDialog();
    dialog->Filter = "Файли пісень (*.dat)|*.dat|Усі файли (*.*)|*.*";
    dialog->DefaultExt = "dat";
    if (String::IsNullOrEmpty(currentFilePath_)) {
        dialog->FileName = "song.dat";
    } else {
        dialog->FileName = currentFilePath_;
    }
    const bool accepted =
        dialog->ShowDialog(this) == System::Windows::Forms::DialogResult::OK;
    String^ chosen = dialog->FileName;
    delete dialog;
    return accepted && SaveSong(chosen);
}

bool MainForm::MaybeSave() {
    if (!isModified_) {
        return true;
    }
    System::Windows::Forms::DialogResult answer = MessageBox::Show(
        this, "Пісню було змінено. Зберегти зміни?", "Незбережені зміни",
        MessageBoxButtons::YesNoCancel, MessageBoxIcon::Question);
    if (answer == System::Windows::Forms::DialogResult::Yes) {
        return SaveCurrent();
    }
    return answer == System::Windows::Forms::DialogResult::No;
}

void MainForm::OnOpenFile(Object^ sender, EventArgs^ e) {
    if (!MaybeSave()) {
        return;
    }
    OpenFileDialog^ dialog = gcnew OpenFileDialog();
    dialog->Filter = "Файли пісень (*.dat)|*.dat|Усі файли (*.*)|*.*";
    const bool accepted =
        dialog->ShowDialog(this) == System::Windows::Forms::DialogResult::OK;
    String^ chosen = dialog->FileName;
    delete dialog;
    if (accepted) {
        LoadSong(chosen);
    }
}

void MainForm::OnSaveFile(Object^ sender, EventArgs^ e) { SaveCurrent(); }

void MainForm::OnSaveFileAs(Object^ sender, EventArgs^ e) { SaveAs(); }

void MainForm::OnExit(Object^ sender, EventArgs^ e) { this->Close(); }

// ------------------------------------------------------------ допоміжні

bool MainForm::EnsureSongLoaded() {
    if (song_->GetVerseCount() == 0) {
        MessageBox::Show(this, "Спочатку відкрийте файл із текстом пісні.", "Немає даних",
                         MessageBoxButtons::OK, MessageBoxIcon::Information);
        return false;
    }
    return true;
}

void MainForm::MarkModified() {
    isModified_ = true;
    RefreshView();
}

void MainForm::SetStatus(String^ text) { statusLabel_->Text = text; }

void MainForm::AppendStyled(String^ text, System::Drawing::Font^ font,
                            System::Drawing::Color color, HorizontalAlignment alignment) {
    // Текст містить лише "\n", тому довжини в RichTextBox і в рядку збігаються.
    const int start = songBox_->TextLength;
    songBox_->AppendText(text);
    songBox_->Select(start, text->Length);
    songBox_->SelectionFont = font;
    songBox_->SelectionColor = color;
    songBox_->SelectionAlignment = alignment;
    songBox_->SelectionIndent = 24;
    songBox_->SelectionRightIndent = 24;
    songBox_->Select(songBox_->TextLength, 0);
}

void MainForm::UpdateBanner() {
    if (song_->GetVerseCount() == 0) {
        bannerSubtitle_->Text = "Відкрийте файл пісні, щоб розпочати";
        return;
    }
    bannerSubtitle_->Text = ToManagedString(song_->GetTitle()) + "   ·   " +
                            ToManagedString(song_->GetAuthor()) + "   ·   куплетів: " +
                            Num(song_->GetVerseCount());
}

void MainForm::RefreshView() {
    String^ name = "Без назви";
    if (!String::IsNullOrEmpty(currentFilePath_)) {
        name = System::IO::Path::GetFileName(currentFilePath_);
    }
    String^ mark = "";
    if (isModified_) {
        mark = " *";
    }
    this->Text = name + mark + " — Редактор пісень";
    UpdateBanner();

    songBox_->Clear();
    if (song_->GetVerseCount() == 0) {
        AppendStyled("\n\n♪  ♫  ♪\n", symbolFont_, Theme::Magenta(), HorizontalAlignment::Center);
        AppendStyled("Файл пісні не завантажено\n", titleFont_, Theme::Purple(),
                     HorizontalAlignment::Center);
        AppendStyled("Відкрийте файл song.dat (Ctrl+O)\n", lineFont_, Theme::Ink(),
                     HorizontalAlignment::Center);
        return;
    }

    AppendStyled("\n" + ToManagedString(song_->GetTitle()) + "\n", titleFont_,
                 Theme::Purple(), HorizontalAlignment::Center);
    AppendStyled("Автор: " + ToManagedString(song_->GetAuthor()) + "\n", authorFont_,
                 Theme::OrangeDark(), HorizontalAlignment::Center);
    AppendStyled("♪  ♫  ♪\n", symbolFont_, Theme::Magenta(), HorizontalAlignment::Center);

    int verseIndex = 0;
    for (const Verse& verse : song_->GetVerses()) {
        AppendStyled("\n♪  Куплет " + Convert::ToString(verse.GetNumber()) + "\n",
                     verseHeaderFont_, Theme::VerseColor(verseIndex),
                     HorizontalAlignment::Left);
        System::Text::StringBuilder^ body = gcnew System::Text::StringBuilder();
        for (const std::string& line : verse.GetLines()) {
            body->Append(ToManagedString(line));
            body->Append("\n");
        }
        AppendStyled(body->ToString(), lineFont_, Theme::Ink(), HorizontalAlignment::Left);
        ++verseIndex;
    }
    songBox_->Select(0, 0);
}

void MainForm::AppendResult(String^ title, List<String^>^ lines) {
    System::Text::StringBuilder^ builder = gcnew System::Text::StringBuilder();
    builder->Append("♪ ");
    builder->Append(title);
    builder->Append(Environment::NewLine);
    for each (String^ line in lines) {
        builder->Append("      ");
        builder->Append(line);
        builder->Append(Environment::NewLine);
    }
    builder->Append(Environment::NewLine);
    resultsBox_->AppendText(builder->ToString());
}

// ------------------------------------------------------------ команди

void MainForm::OnSearchInVerse(Object^ sender, EventArgs^ e) {
    if (!EnsureSongLoaded()) {
        return;
    }
    int verseNumber = 1;
    if (!InputDialog::AskNumber(this, "Пошук у куплеті", "Номер куплета:", 1,
                                static_cast<int>(song_->GetVerseCount()), 1, verseNumber)) {
        return;
    }
    String^ word = "";
    if (!InputDialog::AskText(this, "Пошук у куплеті", "Слово:", word)) {
        return;
    }
    word = word->Trim();
    if (word->Length == 0) {
        return;
    }

    const std::size_t verseIndex = static_cast<std::size_t>(verseNumber - 1);
    const std::vector<std::size_t> found =
        song_->FindWordInVerse(verseIndex, ToStdString(word));

    List<String^>^ lines = gcnew List<String^>();
    for (const std::size_t lineIndex : found) {
        lines->Add("Рядок " + Num(lineIndex + 1) + ": " +
                   ToManagedString(song_->GetVerse(verseIndex).GetLine(lineIndex)));
    }
    if (lines->Count == 0) {
        lines->Add("Слово не знайдено.");
    }
    AppendResult("Пошук «" + word + "» у куплеті " + Num(verseIndex + 1), lines);
}

void MainForm::OnSearchInSong(Object^ sender, EventArgs^ e) {
    if (!EnsureSongLoaded()) {
        return;
    }
    String^ word = "";
    if (!InputDialog::AskText(this, "Пошук у пісні", "Слово:", word)) {
        return;
    }
    word = word->Trim();
    if (word->Length == 0) {
        return;
    }

    List<String^>^ lines = gcnew List<String^>();
    for (const LineReference& match : song_->FindWordInSong(ToStdString(word))) {
        lines->Add("Куплет " + Num(match.verseIndex + 1) + ", рядок " +
                   Num(match.lineIndex + 1) + ": " + ToManagedString(match.text));
    }
    if (lines->Count == 0) {
        lines->Add("Слово не знайдено.");
    }
    AppendResult("Пошук «" + word + "» у пісні", lines);
}

void MainForm::OnReplaceLine(Object^ sender, EventArgs^ e) {
    if (!EnsureSongLoaded()) {
        return;
    }
    int verseNumber = 1;
    if (!InputDialog::AskNumber(this, "Заміна рядка", "Номер куплета:", 1,
                                static_cast<int>(song_->GetVerseCount()), 1, verseNumber)) {
        return;
    }
    const std::size_t verseIndex = static_cast<std::size_t>(verseNumber - 1);
    const Verse& verse = song_->GetVerse(verseIndex);
    if (verse.GetLineCount() == 0) {
        return;
    }
    int lineNumber = 1;
    if (!InputDialog::AskNumber(this, "Заміна рядка", "Номер рядка в куплеті:", 1,
                                static_cast<int>(verse.GetLineCount()), 1, lineNumber)) {
        return;
    }
    const std::size_t lineIndex = static_cast<std::size_t>(lineNumber - 1);

    String^ newLine = ToManagedString(verse.GetLine(lineIndex));
    if (!InputDialog::AskText(this, "Заміна рядка", "Новий текст рядка:", newLine)) {
        return;
    }
    newLine = newLine->Trim();
    if (newLine->Length == 0) {
        return;
    }

    try {
        song_->ReplaceLine(verseIndex, lineIndex, ToStdString(newLine));
    } catch (const std::out_of_range&) {
        MessageBox::Show(this, "Хибний номер куплета або рядка.", "Помилка",
                         MessageBoxButtons::OK, MessageBoxIcon::Warning);
        return;
    }
    MarkModified();

    List<String^>^ lines = gcnew List<String^>();
    lines->Add("Куплет " + Num(verseIndex + 1) + ", рядок " + Num(lineIndex + 1) +
               " → " + newLine);
    AppendResult("Заміна рядка", lines);
}

void MainForm::OnSwapVerses(Object^ sender, EventArgs^ e) {
    if (!EnsureSongLoaded()) {
        return;
    }
    const int count = static_cast<int>(song_->GetVerseCount());
    int first = 1;
    int second = count > 1 ? 2 : 1;
    if (!InputDialog::AskNumber(this, "Переставляння куплетів", "Перший куплет:", 1, count, 1, first) ||
        !InputDialog::AskNumber(this, "Переставляння куплетів", "Другий куплет:", 1, count, second, second)) {
        return;
    }
    if (first == second) {
        MessageBox::Show(this, "Оберіть два різні куплети.", "Переставляння",
                         MessageBoxButtons::OK, MessageBoxIcon::Information);
        return;
    }
    song_->SwapVerses(static_cast<std::size_t>(first - 1), static_cast<std::size_t>(second - 1));
    MarkModified();

    List<String^>^ lines = gcnew List<String^>();
    lines->Add("Куплети " + Convert::ToString(first) + " і " + Convert::ToString(second) +
               " помінялись місцями");
    AppendResult("Переставляння куплетів", lines);
}

void MainForm::OnMoveVerse(Object^ sender, EventArgs^ e) {
    if (!EnsureSongLoaded()) {
        return;
    }
    const int count = static_cast<int>(song_->GetVerseCount());
    int from = 1;
    int to = 1;
    if (!InputDialog::AskNumber(this, "Переміщення куплета", "Який куплет перемістити:", 1, count, 1, from) ||
        !InputDialog::AskNumber(this, "Переміщення куплета", "На яку позицію:", 1, count, 1, to)) {
        return;
    }
    song_->MoveVerse(static_cast<std::size_t>(from - 1), static_cast<std::size_t>(to - 1));
    MarkModified();

    List<String^>^ lines = gcnew List<String^>();
    lines->Add("Куплет " + Convert::ToString(from) + " переміщено на позицію " +
               Convert::ToString(to));
    AppendResult("Переміщення куплета", lines);
}

void MainForm::OnShowLastLines(Object^ sender, EventArgs^ e) {
    if (!EnsureSongLoaded()) {
        return;
    }
    List<String^>^ lines = gcnew List<String^>();
    for (const LineReference& reference : song_->GetLastLines()) {
        lines->Add("Куплет " + Num(reference.verseIndex + 1) + ": " +
                   ToManagedString(reference.text));
    }
    AppendResult("Останні рядки всіх куплетів", lines);
}

void MainForm::OnAnalyzeSonnet(Object^ sender, EventArgs^ e) {
    if (!EnsureSongLoaded()) {
        return;
    }
    const SonnetAnalysis analysis = song_->AnalyzeSonnet();
    List<String^>^ lines = gcnew List<String^>();
    lines->Add(ToManagedString(analysis.type));
    if (!analysis.rhymeScheme.empty()) {
        lines->Add("Схема римування: " + ToManagedString(analysis.rhymeScheme));
    }
    AppendResult("Аналіз на сонет", lines);
}

void MainForm::OnSortVowelWords(Object^ sender, EventArgs^ e) {
    if (!EnsureSongLoaded()) {
        return;
    }
    const std::vector<std::string> words = song_->GetSortedVowelWords();
    String^ separator = ", ";
    System::Text::StringBuilder^ builder = gcnew System::Text::StringBuilder();
    for (std::size_t i = 0; i < words.size(); ++i) {
        if (i > 0) {
            builder->Append(separator);
        }
        builder->Append(ToManagedString(words[i]));
    }

    List<String^>^ lines = gcnew List<String^>();
    lines->Add("Кількість: " + Num(words.size()));
    lines->Add(builder->ToString());
    AppendResult("Слова на голосну (за алфавітом, проста вибірка)", lines);
}

void MainForm::OnStartKaraoke(Object^ sender, EventArgs^ e) {
    if (!EnsureSongLoaded()) {
        return;
    }
    KaraokeForm^ karaoke = gcnew KaraokeForm(song_);
    karaoke->ShowDialog(this);
    delete karaoke;
}

void MainForm::OnClearResults(Object^ sender, EventArgs^ e) { resultsBox_->Clear(); }

void MainForm::OnAbout(Object^ sender, EventArgs^ e) {
    MessageBox::Show(this,
                     "♫  Редактор пісень\n\n"
                     "Робота з текстом пісні: пошук, заміна рядків, переставляння куплетів, "
                     "аналіз сонету, сортування слів та режим «Караоке».\n\n"
                     "Формат файлу: song.dat (UTF-8).",
                     "Про програму", MessageBoxButtons::OK, MessageBoxIcon::Information);
}

}  // namespace SongEditor
