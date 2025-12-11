#include "dsp/Oversampler2Times.h"
#include "dsp/Blamp2Point.h"
#include "dsp/Blamp4Point.h"
#include "dsp/HardClipper.h"
#include "dsp/Oversampler4Times.h"

TEST_CASE ("Boot performance")
{
    BENCHMARK_ADVANCED ("Processor constructor")
    (Catch::Benchmark::Chronometer meter)
    {
        std::vector<Catch::Benchmark::storage_for<PluginProcessor>> storage (size_t (meter.runs()));
        meter.measure ([&] (int i) { storage[(size_t) i].construct(); });
    };

    BENCHMARK_ADVANCED ("Processor destructor")
    (Catch::Benchmark::Chronometer meter)
    {
        std::vector<Catch::Benchmark::destructable_object<PluginProcessor>> storage (size_t (meter.runs()));
        for (auto& s : storage)
            s.construct();
        meter.measure ([&] (int i) { storage[(size_t) i].destruct(); });
    };

    BENCHMARK_ADVANCED ("Editor open and close")
    (Catch::Benchmark::Chronometer meter)
    {
        PluginProcessor plugin;

        // due to complex construction logic of the editor, let's measure open/close together
        meter.measure ([&] (int /* i */) {
            auto editor = plugin.createEditorIfNeeded();
            plugin.editorBeingDeleted (editor);
            delete editor;
            return plugin.getActiveEditor();
        });
    };
}

TEST_CASE ("Anti-aliasing method performance")
{

    HardClipper clipper;
    Blamp2Point blamp;
    Blamp4Point blamp4;
    Blamp4PointCubic blamp4Cubic;
    Oversampler2Times oversampler2;
    Oversampler4Times oversampler4;

    std::vector<HardClipper*> hardClippers {
        &clipper,
        &blamp,
        &blamp4,
        &blamp4Cubic,
        &oversampler2,
        &oversampler4
    };

    std::vector<std::string> names {
        "Naive clip",
        "Blamp 2",
        "Blamp 4",
        "Blamp 4 Cubic",
        "Oversampler2",
        "Oversampler4",
    };

    std::vector<float> frequencies {
        30.123, 300.123, 3000.123, 10000.123
    };

    for (auto c = 0; c < hardClippers.size(); ++c)
    {
        for (auto f = 0; f < frequencies.size(); ++f)
        {
            BENCHMARK_ADVANCED (names[c] + " at " + std::to_string(static_cast<int>(frequencies[f])) + " Hz")
            (Catch::Benchmark::Chronometer meter)
            {
                auto buffer = juce::AudioBuffer<float> (1, 2048);
                hardClippers[c]->prepareToPlay (buffer.getNumSamples(), 44100);
                auto freq = frequencies[f];
                hardClippers[c]->setThreshold (0.3f);
                for (int i = 0; i < buffer.getNumSamples(); ++i)
                {
                    buffer.setSample (0,i, std::cos(static_cast<double>(i) * freq * 2 * juce::MathConstants<double>::pi /44100.0));
                }
                hardClippers[c]->processBlock (buffer);

                // due to complex construction logic of the editor, let's measure open/close together
                meter.measure ([&] (int /* i */) {
                    hardClippers[c]->processBlock (buffer);
                });
            };
        }
        BENCHMARK_ADVANCED (names[c] + " white noise")
        (Catch::Benchmark::Chronometer meter)
        {
            auto buffer = juce::AudioBuffer<float> (1, 2048);
            hardClippers[c]->prepareToPlay (buffer.getNumSamples(), 44100);
            juce::Random random;
            hardClippers[c]->setThreshold (0.3f);
            for (int i = 0; i < buffer.getNumSamples(); ++i)
            {
                buffer.setSample (0,i, random.nextFloat() * 2 - 1);
            }
            hardClippers[c]->processBlock (buffer);

            // due to complex construction logic of the editor, let's measure open/close together
            meter.measure ([&] (int /* i */) {
                hardClippers[c]->processBlock (buffer);
            });
        };
    }
}
