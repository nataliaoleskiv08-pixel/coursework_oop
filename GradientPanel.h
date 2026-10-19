#pragma once

#using <System.dll>
#using <System.Drawing.dll>
#using <System.Windows.Forms.dll>

namespace SongEditor {

/// Панель із градієнтним тлом (2–3 кольори) та необов'язковим візерунком із нот.
public ref class GradientPanel : public System::Windows::Forms::Panel {
public:
    GradientPanel();

    property System::Drawing::Color StartColor;
    property System::Drawing::Color MiddleColor;  // Color::Empty — без середнього кольору
    property System::Drawing::Color EndColor;
    property float Angle;
    property bool ShowNotes;
    property int NotesAlpha;

protected:
    virtual void OnPaint(System::Windows::Forms::PaintEventArgs^ e) override;
};

}  // namespace SongEditor
