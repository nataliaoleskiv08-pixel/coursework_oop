#pragma once

#using <System.dll>

#include <string>

namespace SongEditor {

/// Перетворення System::String^ -> std::string (UTF-8).
inline std::string ToStdString(System::String^ text) {
    if (System::String::IsNullOrEmpty(text)) {
        return std::string();
    }
    cli::array<System::Byte>^ bytes = System::Text::Encoding::UTF8->GetBytes(text);
    cli::pin_ptr<System::Byte> pinned = &bytes[0];
    unsigned char* raw = pinned;
    return std::string(reinterpret_cast<const char*>(raw),
                       static_cast<std::size_t>(bytes->Length));
}

/// Перетворення std::string (UTF-8) -> System::String^.
inline System::String^ ToManagedString(const std::string& text) {
    if (text.empty()) {
        return System::String::Empty;
    }
    cli::array<System::Byte>^ bytes =
        gcnew cli::array<System::Byte>(static_cast<int>(text.size()));
    System::Runtime::InteropServices::Marshal::Copy(
        System::IntPtr(const_cast<char*>(text.data())), bytes, 0, bytes->Length);
    return System::Text::Encoding::UTF8->GetString(bytes);
}

}  // namespace SongEditor
