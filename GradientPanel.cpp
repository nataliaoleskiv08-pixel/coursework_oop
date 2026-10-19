#include "GradientPanel.h"

#include "Theme.h"

using namespace System;
using namespace System::Drawing;
using namespace System::Drawing::Drawing2D;
using namespace System::Windows::Forms;

namespace SongEditor {

GradientPanel::GradientPanel() {
    StartColor = Theme::DeepPurple();
    MiddleColor = Color::Empty;
    EndColor = Theme::Purple();
    Angle = 90.0f;
    ShowNotes = false;
    NotesAlpha = 28;

    this->DoubleBuffered = true;
    this->SetStyle(ControlStyles::ResizeRedraw, true);
}

void GradientPanel::OnPaint(PaintEventArgs^ e) {
    const Rectangle area = this->ClientRectangle;
    if (area.Width <= 0 || area.Height <= 0) {
        Panel::OnPaint(e);
        return;
    }

    const Color start = StartColor;
    const Color middle = MiddleColor;
    const Color end = EndColor;

    LinearGradientBrush^ brush = gcnew LinearGradientBrush(area, start, end, Angle);
    if (!middle.IsEmpty) {
        ColorBlend^ blend = gcnew ColorBlend(3);
        blend->Colors = gcnew array<Color>{start, middle, end};
        blend->Positions = gcnew array<float>{0.0f, 0.5f, 1.0f};
        brush->InterpolationColors = blend;
    }
    e->Graphics->FillRectangle(brush, area);
    delete brush;

    if (ShowNotes) {
        Theme::PaintNotes(e->Graphics, area, NotesAlpha);
    }
    Panel::OnPaint(e);
}

}  // namespace SongEditor
