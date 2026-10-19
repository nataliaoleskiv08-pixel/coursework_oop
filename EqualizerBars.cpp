#include "EqualizerBars.h"

#include "Theme.h"

using namespace System;
using namespace System::Drawing;
using namespace System::Windows::Forms;

namespace SongEditor {

EqualizerBars::EqualizerBars() {
    levels_ = gcnew array<float>(kBarCount);
    targets_ = gcnew array<float>(kBarCount);
    random_ = gcnew Random();
    active_ = false;

    for (int i = 0; i < kBarCount; ++i) {
        levels_[i] = 0.08f;
        targets_[i] = 0.08f;
    }

    this->DoubleBuffered = true;
    this->SetStyle(ControlStyles::ResizeRedraw, true);
    this->SetStyle(ControlStyles::SupportsTransparentBackColor, true);
    this->BackColor = Color::Transparent;

    timer_ = gcnew System::Windows::Forms::Timer();
    timer_->Interval = 80;
    timer_->Tick += gcnew EventHandler(this, &EqualizerBars::OnTimerTick);
    timer_->Start();
}

EqualizerBars::~EqualizerBars() {
    timer_->Stop();
    delete timer_;
}

void EqualizerBars::SetActive(bool active) { active_ = active; }

Color EqualizerBars::Lerp(Color from, Color to, float amount) {
    return Color::FromArgb(
        static_cast<int>(from.R + (to.R - from.R) * amount),
        static_cast<int>(from.G + (to.G - from.G) * amount),
        static_cast<int>(from.B + (to.B - from.B) * amount));
}

void EqualizerBars::OnTimerTick(Object^ sender, EventArgs^ e) {
    for (int i = 0; i < kBarCount; ++i) {
        if (active_) {
            if (random_->NextDouble() < 0.4) {
                targets_[i] = 0.2f + static_cast<float>(random_->NextDouble()) * 0.8f;
            }
        } else {
            targets_[i] = 0.08f;
        }
        levels_[i] += (targets_[i] - levels_[i]) * 0.5f;
    }
    this->Invalidate();
}

void EqualizerBars::OnPaint(PaintEventArgs^ e) {
    if (this->Width < kBarCount * 3 || this->Height < 4) {
        return;
    }
    Graphics^ graphics = e->Graphics;
    const float slot = static_cast<float>(this->Width) / kBarCount;
    const float barWidth = slot * 0.7f;

    for (int i = 0; i < kBarCount; ++i) {
        const float t = static_cast<float>(i) / (kBarCount - 1);
        Color color = t < 0.5f
                          ? Lerp(Theme::Magenta(), Theme::Orange(), t * 2.0f)
                          : Lerp(Theme::Orange(), Theme::Yellow(), (t - 0.5f) * 2.0f);
        SolidBrush^ brush = gcnew SolidBrush(color);
        const float height = levels_[i] * this->Height;
        graphics->FillRectangle(brush, i * slot + (slot - barWidth) / 2.0f,
                                this->Height - height, barWidth, height);
        delete brush;
    }
}

}  // namespace SongEditor
