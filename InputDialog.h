#pragma once

#using <System.dll>
#using <System.Drawing.dll>
#using <System.Windows.Forms.dll>

namespace SongEditor {

/// Невелике модальне вікно для введення тексту або цілого числа
/// (у Windows Forms немає готового InputBox).
public ref class InputDialog : public System::Windows::Forms::Form {
public:
    /// Повертає false, якщо користувач натиснув «Скасувати».
    static bool AskText(System::Windows::Forms::IWin32Window^ owner,
                        System::String^ title, System::String^ prompt,
                        System::String^% value);

    /// Запитує ціле число в діапазоні [minimum; maximum].
    static bool AskNumber(System::Windows::Forms::IWin32Window^ owner,
                          System::String^ title, System::String^ prompt,
                          int minimum, int maximum, int defaultValue, int% result);

private:
    InputDialog(System::String^ title, System::String^ prompt,
                System::String^ initialText);

    System::Windows::Forms::Label^ promptLabel_;
    System::Windows::Forms::TextBox^ textBox_;
    System::Windows::Forms::Button^ okButton_;
    System::Windows::Forms::Button^ cancelButton_;
};

}  // namespace SongEditor
