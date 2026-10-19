#pragma once

#using <System.dll>
#using <System.Drawing.dll>
#using <System.Windows.Forms.dll>

namespace SongEditor {

/// Кольорова таблиця для меню, панелі інструментів та рядка стану.
public ref class MusicColorTable : public System::Windows::Forms::ProfessionalColorTable {
public:
    // ---- Рядок меню
    virtual property System::Drawing::Color MenuStripGradientBegin {
        System::Drawing::Color get() override { return C(52, 17, 110); }
    }
    virtual property System::Drawing::Color MenuStripGradientEnd {
        System::Drawing::Color get() override { return C(110, 36, 160); }
    }
    virtual property System::Drawing::Color MenuItemSelected {
        System::Drawing::Color get() override { return C(255, 214, 230); }
    }
    virtual property System::Drawing::Color MenuItemSelectedGradientBegin {
        System::Drawing::Color get() override { return C(233, 30, 99); }
    }
    virtual property System::Drawing::Color MenuItemSelectedGradientEnd {
        System::Drawing::Color get() override { return C(255, 152, 0); }
    }
    virtual property System::Drawing::Color MenuItemPressedGradientBegin {
        System::Drawing::Color get() override { return C(38, 14, 77); }
    }
    virtual property System::Drawing::Color MenuItemPressedGradientMiddle {
        System::Drawing::Color get() override { return C(60, 25, 110); }
    }
    virtual property System::Drawing::Color MenuItemPressedGradientEnd {
        System::Drawing::Color get() override { return C(80, 35, 140); }
    }
    virtual property System::Drawing::Color MenuItemBorder {
        System::Drawing::Color get() override { return C(255, 64, 129); }
    }
    virtual property System::Drawing::Color MenuBorder {
        System::Drawing::Color get() override { return C(180, 100, 200); }
    }

    // ---- Випадаючі меню
    virtual property System::Drawing::Color ToolStripDropDownBackground {
        System::Drawing::Color get() override { return C(255, 250, 240); }
    }
    virtual property System::Drawing::Color ImageMarginGradientBegin {
        System::Drawing::Color get() override { return C(245, 235, 255); }
    }
    virtual property System::Drawing::Color ImageMarginGradientMiddle {
        System::Drawing::Color get() override { return C(245, 235, 255); }
    }
    virtual property System::Drawing::Color ImageMarginGradientEnd {
        System::Drawing::Color get() override { return C(245, 235, 255); }
    }
    virtual property System::Drawing::Color SeparatorDark {
        System::Drawing::Color get() override { return C(210, 190, 235); }
    }
    virtual property System::Drawing::Color SeparatorLight {
        System::Drawing::Color get() override { return C(255, 255, 255); }
    }

    // ---- Панель інструментів
    virtual property System::Drawing::Color ToolStripGradientBegin {
        System::Drawing::Color get() override { return C(250, 242, 255); }
    }
    virtual property System::Drawing::Color ToolStripGradientMiddle {
        System::Drawing::Color get() override { return C(243, 232, 255); }
    }
    virtual property System::Drawing::Color ToolStripGradientEnd {
        System::Drawing::Color get() override { return C(255, 236, 245); }
    }
    virtual property System::Drawing::Color ToolStripBorder {
        System::Drawing::Color get() override { return C(200, 170, 230); }
    }
    virtual property System::Drawing::Color GripDark {
        System::Drawing::Color get() override { return C(180, 140, 220); }
    }
    virtual property System::Drawing::Color GripLight {
        System::Drawing::Color get() override { return C(255, 255, 255); }
    }
    virtual property System::Drawing::Color ButtonSelectedGradientBegin {
        System::Drawing::Color get() override { return C(255, 243, 205); }
    }
    virtual property System::Drawing::Color ButtonSelectedGradientMiddle {
        System::Drawing::Color get() override { return C(255, 224, 130); }
    }
    virtual property System::Drawing::Color ButtonSelectedGradientEnd {
        System::Drawing::Color get() override { return C(255, 204, 80); }
    }
    virtual property System::Drawing::Color ButtonSelectedHighlight {
        System::Drawing::Color get() override { return C(255, 224, 130); }
    }
    virtual property System::Drawing::Color ButtonSelectedBorder {
        System::Drawing::Color get() override { return C(255, 152, 0); }
    }
    virtual property System::Drawing::Color ButtonPressedGradientBegin {
        System::Drawing::Color get() override { return C(255, 204, 128); }
    }
    virtual property System::Drawing::Color ButtonPressedGradientMiddle {
        System::Drawing::Color get() override { return C(255, 183, 77); }
    }
    virtual property System::Drawing::Color ButtonPressedGradientEnd {
        System::Drawing::Color get() override { return C(255, 167, 38); }
    }
    virtual property System::Drawing::Color ButtonPressedHighlight {
        System::Drawing::Color get() override { return C(255, 183, 77); }
    }
    virtual property System::Drawing::Color ButtonPressedBorder {
        System::Drawing::Color get() override { return C(230, 81, 0); }
    }

    // ---- Рядок стану
    virtual property System::Drawing::Color StatusStripGradientBegin {
        System::Drawing::Color get() override { return C(110, 36, 160); }
    }
    virtual property System::Drawing::Color StatusStripGradientEnd {
        System::Drawing::Color get() override { return C(52, 17, 110); }
    }

private:
    static System::Drawing::Color C(int red, int green, int blue) {
        return System::Drawing::Color::FromArgb(red, green, blue);
    }
};

}  // namespace SongEditor
