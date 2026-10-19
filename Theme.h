#pragma once

#using <System.dll>
#using <System.Drawing.dll>
#using <System.Windows.Forms.dll>

namespace SongEditor {

/// Єдина кольорова схема та допоміжні засоби оформлення програми (тема «пісенник»).
public ref class Theme abstract sealed {
public:
    // ---- Палітра
    static System::Drawing::Color DeepPurple() { return System::Drawing::Color::FromArgb(38, 14, 77); }
    static System::Drawing::Color Violet() { return System::Drawing::Color::FromArgb(74, 20, 140); }
    static System::Drawing::Color Purple() { return System::Drawing::Color::FromArgb(94, 53, 177); }
    static System::Drawing::Color Magenta() { return System::Drawing::Color::FromArgb(233, 30, 99); }
    static System::Drawing::Color Orange() { return System::Drawing::Color::FromArgb(255, 152, 0); }
    static System::Drawing::Color OrangeDark() { return System::Drawing::Color::FromArgb(230, 81, 0); }
    static System::Drawing::Color Yellow() { return System::Drawing::Color::FromArgb(255, 213, 79); }
    static System::Drawing::Color Teal() { return System::Drawing::Color::FromArgb(0, 150, 136); }
    static System::Drawing::Color Green() { return System::Drawing::Color::FromArgb(46, 174, 96); }
    static System::Drawing::Color Amber() { return System::Drawing::Color::FromArgb(255, 160, 0); }
    static System::Drawing::Color Coral() { return System::Drawing::Color::FromArgb(244, 67, 54); }
    static System::Drawing::Color Blue() { return System::Drawing::Color::FromArgb(33, 150, 243); }
    static System::Drawing::Color Indigo() { return System::Drawing::Color::FromArgb(63, 81, 181); }
    static System::Drawing::Color Lavender() { return System::Drawing::Color::FromArgb(240, 232, 252); }
    static System::Drawing::Color Mist() { return System::Drawing::Color::FromArgb(228, 215, 250); }
    static System::Drawing::Color Cream() { return System::Drawing::Color::FromArgb(255, 248, 231); }
    static System::Drawing::Color Ink() { return System::Drawing::Color::FromArgb(45, 27, 78); }
    static System::Drawing::Color Night() { return System::Drawing::Color::FromArgb(24, 10, 48); }

    /// Яскраві кольори для тексту на темному тлі (змінюються від куплета до куплета).
    static System::Drawing::Color NoteColor(int index);

    /// Насичені кольори заголовків куплетів на світлому тлі.
    static System::Drawing::Color VerseColor(int index);

    // ---- Значки та кнопки
    /// Чи встановлено шрифт піктограм Windows 10/11 (Segoe MDL2 Assets).
    static bool HasIconFont();

    /// Круглий кольоровий значок із білим символом.
    static System::Drawing::Bitmap^ MakeBadgeIcon(int glyphCode, System::String^ fallback,
                                                  System::Drawing::Color background, int size);

    /// Прозоре зображення з одним символом (для кнопок).
    static System::Drawing::Bitmap^ MakeGlyphImage(int glyphCode, System::String^ fallback,
                                                   System::Drawing::Color foreground, int size);

    /// Піктограма вікна: круг із нотою.
    static System::Drawing::Icon^ MakeAppIcon();

    /// Плоска яскрава кнопка зі значком.
    static void StyleButton(System::Windows::Forms::Button^ button,
                            System::Drawing::Color color, int glyphCode,
                            System::String^ fallback);

    /// Рендерер меню, панелей та рядка стану у кольорах теми.
    static System::Windows::Forms::ToolStripProfessionalRenderer^ CreateRenderer();

    /// Напівпрозорі ноти як декоративний візерунок.
    static void PaintNotes(System::Drawing::Graphics^ graphics,
                           System::Drawing::Rectangle area, int alpha);
};

}  // namespace SongEditor
