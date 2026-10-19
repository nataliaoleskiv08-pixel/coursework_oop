#pragma once

#using <System.dll>
#using <System.Drawing.dll>
#using <System.Windows.Forms.dll>

#include "EqualizerBars.h"
#include "GradientPanel.h"
#include "Song.h"

namespace SongEditor {

/// Головне вікно програми «Редактор пісень».
/// Інтерфейс створюється програмно (без дизайнера), модель — клас Song (native C++).
public ref class MainForm : public System::Windows::Forms::Form {
public:
    MainForm();

protected:
    ~MainForm();
    !MainForm();
    virtual void OnFormClosing(System::Windows::Forms::FormClosingEventArgs^ e) override;

private:
    // Побудова інтерфейсу
    void BuildInterface();
    void BuildBanner();
    System::Windows::Forms::ToolStripMenuItem^ CreateMenuItem(
        System::String^ text, System::String^ tip, System::Windows::Forms::Keys shortcut,
        System::EventHandler^ handler);
    System::Windows::Forms::ToolStripButton^ CreateToolButton(
        System::String^ text, System::String^ tip, int glyphCode, System::String^ fallback,
        System::Drawing::Color badgeColor, System::EventHandler^ handler);
    System::Windows::Forms::ContextMenuStrip^ CreateContextMenu();

    // Службові події
    void OnFormLoad(System::Object^ sender, System::EventArgs^ e);
    void OnItemHover(System::Object^ sender, System::EventArgs^ e);
    void OnItemLeave(System::Object^ sender, System::EventArgs^ e);

    // Команди меню / панелі інструментів
    void OnOpenFile(System::Object^ sender, System::EventArgs^ e);
    void OnSaveFile(System::Object^ sender, System::EventArgs^ e);
    void OnSaveFileAs(System::Object^ sender, System::EventArgs^ e);
    void OnExit(System::Object^ sender, System::EventArgs^ e);
    void OnSearchInVerse(System::Object^ sender, System::EventArgs^ e);
    void OnSearchInSong(System::Object^ sender, System::EventArgs^ e);
    void OnReplaceLine(System::Object^ sender, System::EventArgs^ e);
    void OnSwapVerses(System::Object^ sender, System::EventArgs^ e);
    void OnMoveVerse(System::Object^ sender, System::EventArgs^ e);
    void OnShowLastLines(System::Object^ sender, System::EventArgs^ e);
    void OnAnalyzeSonnet(System::Object^ sender, System::EventArgs^ e);
    void OnSortVowelWords(System::Object^ sender, System::EventArgs^ e);
    void OnStartKaraoke(System::Object^ sender, System::EventArgs^ e);
    void OnClearResults(System::Object^ sender, System::EventArgs^ e);
    void OnAbout(System::Object^ sender, System::EventArgs^ e);

    // Допоміжні методи
    bool LoadSong(System::String^ path);
    bool SaveSong(System::String^ path);
    bool SaveCurrent();
    bool SaveAs();
    bool MaybeSave();
    bool EnsureSongLoaded();
    void MarkModified();
    void RefreshView();
    void UpdateBanner();
    void AppendStyled(System::String^ text, System::Drawing::Font^ font,
                      System::Drawing::Color color,
                      System::Windows::Forms::HorizontalAlignment alignment);
    void AppendResult(System::String^ title,
                      System::Collections::Generic::List<System::String^>^ lines);
    void SetStatus(System::String^ text);

    // Дані
    Song* song_;
    System::String^ currentFilePath_;
    bool isModified_;

    // Елементи інтерфейсу
    System::Windows::Forms::MenuStrip^ menuStrip_;
    System::Windows::Forms::ToolStrip^ toolStrip_;
    System::Windows::Forms::StatusStrip^ statusStrip_;
    System::Windows::Forms::ToolStripStatusLabel^ statusLabel_;
    System::Windows::Forms::SplitContainer^ splitContainer_;
    System::Windows::Forms::RichTextBox^ songBox_;
    System::Windows::Forms::TextBox^ resultsBox_;

    GradientPanel^ banner_;
    System::Windows::Forms::Label^ bannerSubtitle_;
    EqualizerBars^ bannerEqualizer_;

    System::Drawing::Font^ titleFont_;
    System::Drawing::Font^ authorFont_;
    System::Drawing::Font^ symbolFont_;
    System::Drawing::Font^ verseHeaderFont_;
    System::Drawing::Font^ lineFont_;
};

}  // namespace SongEditor
