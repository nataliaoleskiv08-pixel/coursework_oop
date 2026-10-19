#pragma once

#using <System.dll>
#using <System.Drawing.dll>
#using <System.Windows.Forms.dll>

namespace SongEditor {

/// Анімований «еквалайзер»: стовпчики танцюють, поки активний, і затихають, коли ні.
public ref class EqualizerBars : public System::Windows::Forms::Control {
public:
    EqualizerBars();

    void SetActive(bool active);

protected:
    ~EqualizerBars();
    virtual void OnPaint(System::Windows::Forms::PaintEventArgs^ e) override;

private:
    void OnTimerTick(System::Object^ sender, System::EventArgs^ e);
    static System::Drawing::Color Lerp(System::Drawing::Color from,
                                       System::Drawing::Color to, float amount);

    literal int kBarCount = 16;

    cli::array<float>^ levels_;
    cli::array<float>^ targets_;
    System::Random^ random_;
    System::Windows::Forms::Timer^ timer_;
    bool active_;
};

}  // namespace SongEditor
