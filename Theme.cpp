#include "Theme.h"

#include "MusicColorTable.h"

using namespace System;
using namespace System::Drawing;
using namespace System::Drawing::Drawing2D;
using namespace System::Drawing::Text;
using namespace System::Windows::Forms;

namespace SongEditor {

namespace {

/// Малює один символ по центру квадрата size x size.
void DrawGlyphCentered(Graphics^ graphics, int glyphCode, String^ fallback,
                       Brush^ brush, int size) {
    String^ text = nullptr;
    System::Drawing::Font^ font = nullptr;
    if (Theme::HasIconFont()) {
        text = gcnew String(static_cast<wchar_t>(glyphCode), 1);
        font = gcnew System::Drawing::Font("Segoe MDL2 Assets", size * 0.5f,
                                           FontStyle::Regular, GraphicsUnit::Pixel);
    } else {
        text = fallback;
        font = gcnew System::Drawing::Font("Segoe UI Symbol", size * 0.5f,
                                           FontStyle::Bold, GraphicsUnit::Pixel);
    }
    StringFormat^ format = gcnew StringFormat();
    format->Alignment = StringAlignment::Center;
    format->LineAlignment = StringAlignment::Center;
    graphics->DrawString(text, font, brush,
                         RectangleF(0.0f, 0.0f, static_cast<float>(size), static_cast<float>(size)),
                         format);
    delete format;
    delete font;
}

}  // namespace

Color Theme::NoteColor(int index) {
    switch (((index % 5) + 5) % 5) {
        case 0: return Color::FromArgb(255, 213, 79);   // жовтий
        case 1: return Color::FromArgb(255, 128, 171);  // рожевий
        case 2: return Color::FromArgb(128, 222, 234);  // блакитний
        case 3: return Color::FromArgb(165, 214, 167);  // м'ятний
        default: return Color::FromArgb(255, 171, 64);  // помаранчевий
    }
}

Color Theme::VerseColor(int index) {
    switch (((index % 4) + 4) % 4) {
        case 0: return Magenta();
        case 1: return Purple();
        case 2: return Teal();
        default: return OrangeDark();
    }
}

bool Theme::HasIconFont() {
    for each (FontFamily^ family in FontFamily::Families) {
        if (family->Name == "Segoe MDL2 Assets") {
            return true;
        }
    }
    return false;
}

Bitmap^ Theme::MakeBadgeIcon(int glyphCode, String^ fallback, Color background, int size) {
    Bitmap^ bitmap = gcnew Bitmap(size, size);
    Graphics^ graphics = Graphics::FromImage(bitmap);
    graphics->SmoothingMode = SmoothingMode::AntiAlias;
    graphics->TextRenderingHint = TextRenderingHint::AntiAlias;

    SolidBrush^ fill = gcnew SolidBrush(background);
    graphics->FillEllipse(fill, 1, 1, size - 2, size - 2);
    DrawGlyphCentered(graphics, glyphCode, fallback, Brushes::White, size);

    delete fill;
    delete graphics;
    return bitmap;
}

Bitmap^ Theme::MakeGlyphImage(int glyphCode, String^ fallback, Color foreground, int size) {
    Bitmap^ bitmap = gcnew Bitmap(size, size);
    Graphics^ graphics = Graphics::FromImage(bitmap);
    graphics->SmoothingMode = SmoothingMode::AntiAlias;
    graphics->TextRenderingHint = TextRenderingHint::AntiAlias;

    SolidBrush^ brush = gcnew SolidBrush(foreground);
    DrawGlyphCentered(graphics, glyphCode, fallback, brush, size);

    delete brush;
    delete graphics;
    return bitmap;
}

Icon^ Theme::MakeAppIcon() {
    const int size = 32;
    Bitmap^ bitmap = gcnew Bitmap(size, size);
    Graphics^ graphics = Graphics::FromImage(bitmap);
    graphics->SmoothingMode = SmoothingMode::AntiAlias;
    graphics->TextRenderingHint = TextRenderingHint::AntiAlias;

    LinearGradientBrush^ fill = gcnew LinearGradientBrush(
        Rectangle(0, 0, size, size), Magenta(), Orange(), 45.0f);
    graphics->FillEllipse(fill, 0, 0, size - 1, size - 1);

    System::Drawing::Font^ font = gcnew System::Drawing::Font(
        "Segoe UI Symbol", 20.0f, FontStyle::Bold, GraphicsUnit::Pixel);
    StringFormat^ format = gcnew StringFormat();
    format->Alignment = StringAlignment::Center;
    format->LineAlignment = StringAlignment::Center;
    graphics->DrawString("♫", font, Brushes::White,
                         RectangleF(0.0f, 0.0f, static_cast<float>(size), static_cast<float>(size)),
                         format);

    delete format;
    delete font;
    delete fill;
    delete graphics;
    return Icon::FromHandle(bitmap->GetHicon());
}

void Theme::StyleButton(Button^ button, Color color, int glyphCode, String^ fallback) {
    button->FlatStyle = FlatStyle::Flat;
    button->FlatAppearance->BorderSize = 0;
    button->BackColor = color;
    button->ForeColor = Color::White;
    button->FlatAppearance->MouseOverBackColor = ControlPaint::Light(color, 0.35f);
    button->FlatAppearance->MouseDownBackColor = ControlPaint::Dark(color, 0.1f);
    button->Font = gcnew System::Drawing::Font("Segoe UI", 11.0f, FontStyle::Bold);
    button->Image = MakeGlyphImage(glyphCode, fallback, Color::White, 26);
    button->ImageAlign = ContentAlignment::MiddleLeft;
    button->TextAlign = ContentAlignment::MiddleCenter;
    button->TextImageRelation = TextImageRelation::ImageBeforeText;
    button->Cursor = Cursors::Hand;
    button->UseVisualStyleBackColor = false;
}

ToolStripProfessionalRenderer^ Theme::CreateRenderer() {
    ToolStripProfessionalRenderer^ renderer =
        gcnew ToolStripProfessionalRenderer(gcnew MusicColorTable());
    renderer->RoundedEdges = false;
    return renderer;
}

void Theme::PaintNotes(Graphics^ graphics, Rectangle area, int alpha) {
    array<String^>^ notes = gcnew array<String^>{"♪", "♫", "♬", "♩", "♪", "♫", "♬", "♩"};
    array<float>^ xs = gcnew array<float>{0.03f, 0.17f, 0.31f, 0.46f, 0.61f, 0.74f, 0.86f, 0.95f};
    array<float>^ ys = gcnew array<float>{0.10f, 0.70f, 0.25f, 0.80f, 0.05f, 0.60f, 0.30f, 0.75f};
    array<float>^ scales = gcnew array<float>{1.0f, 0.6f, 0.8f, 0.5f, 0.9f, 0.55f, 1.0f, 0.6f};

    const float basis = static_cast<float>(Math::Min(area.Height, 150));
    graphics->TextRenderingHint = TextRenderingHint::AntiAlias;
    SolidBrush^ brush = gcnew SolidBrush(Color::FromArgb(alpha, 255, 255, 255));
    for (int i = 0; i < notes->Length; ++i) {
        const float size = basis * scales[i];
        System::Drawing::Font^ font = gcnew System::Drawing::Font(
            "Segoe UI Symbol", size, FontStyle::Regular, GraphicsUnit::Pixel);
        const float x = area.Left + (area.Width - size) * xs[i];
        const float y = area.Top + (area.Height - size) * ys[i];
        graphics->DrawString(notes[i], font, brush, x, y);
        delete font;
    }
    delete brush;
}

}  // namespace SongEditor
