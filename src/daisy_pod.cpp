#include "daisy_pod.h"

#ifndef SAMPLE_RATE
//#define SAMPLE_RATE DSY_AUDIO_SAMPLE_RATE
#define SAMPLE_RATE 48014.f
#endif

using namespace daisy;

// # Rev3 and Rev4 with newest pinout.
// Compatible with Seed Rev3 and Rev4
constexpr Pin SW_1_PIN = seed::D27;
constexpr Pin SW_2_PIN = seed::D28;

constexpr Pin ENC_A_PIN     = seed::D26;
constexpr Pin ENC_B_PIN     = seed::D25;
constexpr Pin ENC_CLICK_PIN = seed::D13;

constexpr Pin LED_1_R_PIN = seed::D20;
constexpr Pin LED_1_G_PIN = seed::D19;
constexpr Pin LED_1_B_PIN = seed::D18;
constexpr Pin LED_2_R_PIN = seed::D17;
constexpr Pin LED_2_G_PIN = seed::D24;
constexpr Pin LED_2_B_PIN = seed::D23;

constexpr Pin KNOB_1_PIN = seed::D21;
constexpr Pin KNOB_2_PIN = seed::D15;

/*
// Leaving in place until older hardware is totally deprecated.
#ifndef SEED_REV2

// Rev2 Pinout
// Compatible with Seed rev1 and rev2

#define SW_1_PIN 28
#define SW_2_PIN 29

#define ENC_A_PIN 27
#define ENC_B_PIN 26
#define ENC_CLICK_PIN 14

#define LED_1_R_PIN 21
#define LED_1_G_PIN 20
#define LED_1_B_PIN 19
#define LED_2_R_PIN 0
#define LED_2_G_PIN 25
#define LED_2_B_PIN 24

#else

// Rev1 Pinout
// Compatible with Seed rev1 and rev2

#define SW_1_PIN 29
#define SW_2_PIN 28

#define ENC_A_PIN 27
#define ENC_B_PIN 26
#define ENC_CLICK_PIN 1

#define LED_1_R_PIN 21
#define LED_1_G_PIN 20
#define LED_1_B_PIN 19
#define LED_2_R_PIN 0
#define LED_2_G_PIN 25
#define LED_2_B_PIN 24

#endif
*/


using namespace daisy;

void DaisyPod::Init(bool boost)
{
    // Set Some numbers up for accessors.
    // Initialize the hardware.
    seed.Configure();
    seed.Init(boost);
    InitButtons();
    InitEncoder();
    InitLeds();
    InitKnobs();
    InitMidi();
    SetAudioBlockSize(48);
}

void DaisyPod::DelayMs(size_t del)
{
    seed.DelayMs(del);
}


void DaisyPod::SetHidUpdateRates()
{
    for(int i = 0; i < KNOB_LAST; i++)
    {
        knobs[i]->SetSampleRate(AudioCallbackRate());
    }
}

void DaisyPod::StartAudio(AudioHandle::InterleavingAudioCallback cb)
{
    seed.StartAudio(cb);
}

void DaisyPod::StartAudio(AudioHandle::AudioCallback cb)
{
    seed.StartAudio(cb);
}

void DaisyPod::ChangeAudioCallback(AudioHandle::InterleavingAudioCallback cb)
{
    seed.ChangeAudioCallback(cb);
}

void DaisyPod::ChangeAudioCallback(AudioHandle::AudioCallback cb)
{
    seed.ChangeAudioCallback(cb);
}

void DaisyPod::StopAudio()
{
    seed.StopAudio();
}

void DaisyPod::SetAudioBlockSize(size_t size)
{
    seed.SetAudioBlockSize(size);
    SetHidUpdateRates();
}

size_t DaisyPod::AudioBlockSize()
{
    return seed.AudioBlockSize();
}

void DaisyPod::SetAudioSampleRate(SaiHandle::Config::SampleRate samplerate)
{
    seed.SetAudioSampleRate(samplerate);
    SetHidUpdateRates();
}

float DaisyPod::AudioSampleRate()
{
    return seed.AudioSampleRate();
}

float DaisyPod::AudioCallbackRate()
{
    return seed.AudioCallbackRate();
}

void DaisyPod::StartAdc()
{
    seed.adc.Start();
}

void DaisyPod::StopAdc()
{
    seed.adc.Stop();
}


void DaisyPod::ProcessAnalogControls()
{
    knob1.Process();
    knob2.Process();
}

float DaisyPod::GetKnobValue(Knob k)
{
    size_t idx;
    idx = k < KNOB_LAST ? k : KNOB_1;
    return knobs[idx]->Value();
}

void DaisyPod::ProcessDigitalControls()
{
    encoder.Debounce();
    button1.Debounce();
    button2.Debounce();
}

void DaisyPod::ClearLeds()
{
    // Using Color
    Color c;
    c.Init(Color::PresetColor::OFF);
    led1.SetColor(c);
    led2.SetColor(c);
    // Without
    // led1.Set(0.0f, 0.0f, 0.0f);
    // led2.Set(0.0f, 0.0f, 0.0f);
}

void DaisyPod::UpdateLeds()
{
    led1.Update();
    led2.Update();
}

void DaisyPod::InitButtons()
{
    // button1
    button1.Init(SW_1_PIN);
    // button2
    button2.Init(SW_2_PIN);

    buttons[BUTTON_1] = &button1;
    buttons[BUTTON_2] = &button2;
}

void DaisyPod::InitEncoder()
{
    encoder.Init(ENC_A_PIN, ENC_B_PIN, ENC_CLICK_PIN);
}

void DaisyPod::InitLeds()
{
    // LEDs are just going to be on/off for now.
    // TODO: Add PWM support
    led1.Init(LED_1_R_PIN, LED_1_G_PIN, LED_1_B_PIN, true);

    led2.Init(LED_2_R_PIN, LED_2_G_PIN, LED_2_B_PIN, true);

    ClearLeds();
    UpdateLeds();
}
void DaisyPod::InitKnobs()
{
    // Configure the ADC channels using the desired pin
    AdcChannelConfig knob_init[KNOB_LAST];
    knob_init[KNOB_1].InitSingle(KNOB_1_PIN);
    knob_init[KNOB_2].InitSingle(KNOB_2_PIN);
    // Initialize with the knob init struct w/ 2 members
    // Set Oversampling to 32x
    seed.adc.Init(knob_init, KNOB_LAST);
    // Make an array of pointers to the knobs.
    knobs[KNOB_1] = &knob1;
    knobs[KNOB_2] = &knob2;
    for(int i = 0; i < KNOB_LAST; i++)
    {
        knobs[i]->Init(seed.adc.GetPtr(i), seed.AudioCallbackRate());
    }
}
void DaisyPod::InitMidi()
{
    MidiUartHandler::Config midi_config;
    midi.Init(midi_config);
}
