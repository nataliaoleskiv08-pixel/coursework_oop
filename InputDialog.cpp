#include "InputDialog.h"

#include "GradientPanel.h"
#include "Theme.h"

using namespace System;
using namespace System::Windows::Forms;

namespace SongEditor {

InputDialog::InputDialog(String^ title, String^ prompt, String^ initialText) {
    this->Text = title;
    this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedDialog;
    this->StartPosition = FormStartPosition::CenterParent;
    this->MaximizeBox = false;
    this->MinimizeBox = false;
    this->ShowInTaskbar = false;
    this->ClientSize = System::Drawing::Size(460, 196);
    this->BackColor = System::Drawing::Color::FromArgb(250, 246, 255);
    this->Font = gcnew System::Drawing::Font("Segoe UI", 10.0f);

    // Яскрава шапка з назвою вікна
    GradientPanel^ header = gcnew GradientPanel();
    header->Dock = DockStyle::Top;
    header->Height = 54;
    header->StartColor = Theme::Violet();
    header->EndColor = Theme::Magenta();
    header->Angle = 0.0f;
    header->ShowNotes = true;
    header->NotesAlpha = 40;

    Label^ headerLabel = gcnew Label();
    headerLabel->Text = "♪  " + title;
    headerLabel->AutoSize = true;
    headerLabel->BackColor = System::Drawing::Color::Transparent;
    headerLabel->ForeColor = System::Drawing::Color::White;
    headerLabel->Font = gcnew System::Drawing::Font("Segoe UI", 14.0f, System::Drawing::FontStyle::Bold);
    headerLabel->Location = System::Drawing::Point(16, 12);
    header->Controls->Add(headerLabel);

    promptLabel_ = gcnew Label();
    promptLabel_->Text = prompt;
    promptLabel_->ForeColor = Theme::Ink();
    promptLabel_->Location = System::Drawing::Point(18, 70);
    promptLabel_->Size = System::Drawing::Size(424, 24);

    textBox_ = gcnew TextBox();
    textBox_->Text = initialText;
    textBox_->Font = gcnew System::Drawing::Font("Segoe UI", 12.0f);
    textBox_->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
    textBox_->Location = System::Drawing::Point(18, 98);
    textBox_->Size = System::Drawing::Size(424, 30);
    textBox_->SelectAll();

    okButton_ = gcnew Button();
    okButton_->Text = "ОК";
    okButton_->DialogResult = System::Windows::Forms::DialogResult::OK;
    okButton_->Location = System::Drawing::Point(194, 146);
    okButton_->Size = System::Drawing::Size(120, 38);
    Theme::StyleButton(okButton_, Theme::Green(), 0xE73E, "✓");

    cancelButton_ = gcnew Button();
    cancelButton_->Text = "Скасувати";
    cancelButton_->DialogResult = System::Windows::Forms::DialogResult::Cancel;
    cancelButton_->Location = System::Drawing::Point(322, 146);
    cancelButton_->Size = System::Drawing::Size(120, 38);
    Theme::StyleButton(cancelButton_, Theme::Coral(), 0xE711, "✕");

    this->AcceptButton = okButton_;
    this->CancelButton = cancelButton_;

    this->Controls->Add(header);
    this->Controls->Add(promptLabel_);
    this->Controls->Add(textBox_);
    this->Controls->Add(okButton_);
    this->Controls->Add(cancelButton_);
}

bool InputDialog::AskText(IWin32Window^ owner, String^ title, String^ prompt,
                          String^% value) {
    InputDialog^ dialog = gcnew InputDialog(title, prompt, value);
    const bool accepted =
        dialog->ShowDialog(owner) == System::Windows::Forms::DialogResult::OK;
    if (accepted) {
        value = dialog->textBox_->Text;
    }
    delete dialog;
    return accepted;
}

bool InputDialog::AskNumber(IWin32Window^ owner, String^ title, String^ prompt,
                            int minimum, int maximum, int defaultValue,
                            int% result) {
    String^ text = Convert::ToString(defaultValue);
    while (true) {
        if (!AskText(owner, title, prompt, text)) {
            return false;
        }
        int parsed = 0;
        if (Int32::TryParse(text->Trim(), parsed) && parsed >= minimum &&
            parsed <= maximum) {
            result = parsed;
            return true;
        }
        MessageBox::Show(owner,
                         "Введіть ціле число від " + Convert::ToString(minimum) + " до " +
                             Convert::ToString(maximum) + ".",
                         "Некоректне значення", MessageBoxButtons::OK,
                         MessageBoxIcon::Warning);
    }
}

}  // namespace SongEditor
