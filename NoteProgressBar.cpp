#include "NoteProgressBar.h"

#include "Theme.h"

using namespace System;
using namespace System::Drawing;
using namespace System::Drawing::Drawing2D;
using namespace System::Windows::Forms;

namespace SongEditor {

NoteProgressBar::NoteProgressBar() {
    maximum_ = 1;
    value_ = 0;
    this->Height = 18;
    this->DoubleBuffered = true;
    this->SetStyle(ControlStyles::ResizeRedraw, true);
    this->SetStyle(ControlStyles::SupportsTransparentBackColor, true);
    this->BackColor = Color::Transparent;
}

GraphicsPath^ NoteProgressBar::RoundedRectangle(Rectangle area, int radius) {
    GraphicsPath^ path = gcnew GraphicsPath();
    const int diameter = radius * 2;
    path->AddArc(area.Left, area.Top, diameter, diameter, 180.0f, 90.0f);
    path->AddArc(area.Right - diameter, area.Top, diameter, diameter, 270.0f, 90.0f);
    path->AddArc(area.Right - diameter, area.Bottom - diameter, diameter, diameter, 0.0f, 90.0f);
    path->AddArc(area.Left, area.Bottom - diameter, diameter, diameter, 90.0f, 90.0f);
    path->CloseFigure();
    return path;
}

void NoteProgressBar::OnPaint(PaintEventArgs^ e) {
    Graphics^ graphics = e->Graphics;
    graphics->SmoothingMode = SmoothingMode::AntiAlias;

    const Rectangle track(2, 3, this->Width - 5, this->Height - 7);
    if (track.Width < 10 || track.Height < 4) {
        return;
    }
    const int radius = track.Height / 2;

    // Доріжка
    GraphicsPath^ trackPath = RoundedRectangle(track, radius);
    SolidBrush^ trackBrush = gcnew SolidBrush(Color::FromArgb(70, 255, 255, 255));
    graphics->FillPath(trackBrush, trackPath);
    delete trackBrush;
    delete trackPath;

    // Заповнена частина
    const int filledWidth = static_cast<int>(
        static_cast<double>(track.Width) * value_ / maximum_);
    if (filledWidth >= track.Height) {
        const Rectangle filled(track.Left, track.Top, filledWidth, track.Height);
        LinearGradientBrush^ brush = gcnew LinearGradientBrush(
            filled, Theme::Magenta(), Theme::Yellow(), 0.0f);
        ColorBlend^ blend = gcnew ColorBlend(3);
        blend->Colors = gcnew array<Color>{Theme::Magenta(), Theme::Orange(), Theme::Yellow()};
        blend->Positions = gcnew array<float>{0.0f, 0.55f, 1.0f};
        brush->InterpolationColors = blend;

        GraphicsPath^ filledPath = RoundedRectangle(filled, radius);
        graphics->FillPath(brush, filledPath);
        delete filledPath;
        delete brush;
    }
}

}  // namespace SongEditor
