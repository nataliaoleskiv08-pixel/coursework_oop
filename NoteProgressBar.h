#pragma once

#using <System.dll>
#using <System.Drawing.dll>
#using <System.Windows.Forms.dll>

namespace SongEditor {

/// Заокруглена смуга прогресу з яскравим градієнтом (малиновий → помаранчевий → жовтий).
public ref class NoteProgressBar : public System::Windows::Forms::Control {
public:
    NoteProgressBar();

    property int Maximum {
        int get() { return maximum_; }
        void set(int value) {
            maximum_ = value < 1 ? 1 : value;
            this->Invalidate();
        }
    }

    property int Value {
        int get() { return value_; }
        void set(int value) {
            value_ = value;
            this->Invalidate();
        }
    }

protected:
    virtual void OnPaint(System::Windows::Forms::PaintEventArgs^ e) override;

private:
    static System::Drawing::Drawing2D::GraphicsPath^ RoundedRectangle(
        System::Drawing::Rectangle area, int radius);

    int maximum_;
    int value_;
};

}  // namespace SongEditor
