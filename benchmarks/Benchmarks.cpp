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
    BENCHMARK_ADVANCED ("Naive clip 200 hz")
    (Catch::Benchmark::Chronometer meter)
    {
        HardClipper clipper;
        auto buffer = juce::AudioBuffer<float> (1, 2048);
        clipper.prepareToPlay (buffer.getNumSamples(), 44100);
        auto freq = 100.0;
        clipper.setThreshold (0.3f);
        for (int i = 0; i < buffer.getNumSamples(); ++i)
        {
            buffer.setSample (0,i, std::cos(static_cast<double>(i) * freq * 2 * juce::MathConstants<double>::pi /44100.0));
        }
        clipper.processBlock (buffer);

        // due to complex construction logic of the editor, let's measure open/close together
        meter.measure ([&] (int /* i */) {
            clipper.processBlock (buffer);
        });
    };
    BENCHMARK_ADVANCED ("2 point blamp 200 hz")
    (Catch::Benchmark::Chronometer meter)
    {
        Blamp2Point clipper;
        auto buffer = juce::AudioBuffer<float> (1, 2048);
        clipper.prepareToPlay (buffer.getNumSamples(), 44100);
        auto freq = 100.0;
        clipper.setThreshold (0.3f);
        for (int i = 0; i < buffer.getNumSamples(); ++i)
        {
            buffer.setSample (0,i, std::cos(static_cast<double>(i) * freq * 2 * juce::MathConstants<double>::pi /44100.0));
        }
        clipper.processBlock (buffer);

        // due to complex construction logic of the editor, let's measure open/close together
        meter.measure ([&] (int /* i */) {
            clipper.processBlock (buffer);
        });
    };
    BENCHMARK_ADVANCED ("4 point blamp 200 hz")
    (Catch::Benchmark::Chronometer meter)
    {
        Blamp4Point clipper;
        auto buffer = juce::AudioBuffer<float> (1, 2048);
        clipper.prepareToPlay (buffer.getNumSamples(), 44100);
        auto freq = 100.0;
        clipper.setThreshold (0.3f);
        for (int i = 0; i < buffer.getNumSamples(); ++i)
        {
            buffer.setSample (0,i, std::cos(static_cast<double>(i) * freq * 2 * juce::MathConstants<double>::pi /44100.0));
        }
        clipper.processBlock (buffer);

        // due to complex construction logic of the editor, let's measure open/close together
        meter.measure ([&] (int /* i */) {
            clipper.processBlock (buffer);
        });
    };
    BENCHMARK_ADVANCED ("2 point oversample 200 hz")
    (Catch::Benchmark::Chronometer meter)
    {
        Oversampler2Times clipper;
        auto buffer = juce::AudioBuffer<float> (1, 2048);
        clipper.prepareToPlay (buffer.getNumSamples(), 44100);
        auto freq = 100.0;
        clipper.setThreshold (0.3f);
        for (int i = 0; i < buffer.getNumSamples(); ++i)
        {
            buffer.setSample (0,i, std::cos(static_cast<double>(i) * freq * 2 * juce::MathConstants<double>::pi /44100.0));
        }
        clipper.processBlock (buffer);

        // due to complex construction logic of the editor, let's measure open/close together
        meter.measure ([&] (int /* i */) {
            clipper.processBlock (buffer);
        });
    };
    BENCHMARK_ADVANCED ("4 point oversample 200 hz")
    (Catch::Benchmark::Chronometer meter)
    {
        Oversampler4Times clipper;
        auto buffer = juce::AudioBuffer<float> (1, 2048);
        clipper.prepareToPlay (buffer.getNumSamples(), 44100);
        auto freq = 100.0;
        clipper.setThreshold (0.3f);
        for (int i = 0; i < buffer.getNumSamples(); ++i)
        {
            buffer.setSample (0,i, std::cos(static_cast<double>(i) * freq * 2 * juce::MathConstants<double>::pi /44100.0));
        }
        clipper.processBlock (buffer);

        // due to complex construction logic of the editor, let's measure open/close together
        meter.measure ([&] (int /* i */) {
            clipper.processBlock (buffer);
        });
    };
    BENCHMARK_ADVANCED ("Naive clip 3000 hz")
    (Catch::Benchmark::Chronometer meter)
    {
        HardClipper clipper;
        auto buffer = juce::AudioBuffer<float> (1, 2048);
        clipper.prepareToPlay (buffer.getNumSamples(), 44100);
        auto freq = 3000.0;
        clipper.setThreshold (0.3f);
        for (int i = 0; i < buffer.getNumSamples(); ++i)
        {
            buffer.setSample (0,i, std::cos(static_cast<double>(i) * freq * 2 * juce::MathConstants<double>::pi /44100.0));
        }
        clipper.processBlock (buffer);

        // due to complex construction logic of the editor, let's measure open/close together
        meter.measure ([&] (int /* i */) {
            clipper.processBlock (buffer);
        });
    };
    BENCHMARK_ADVANCED ("2 point blamp 3000 hz")
    (Catch::Benchmark::Chronometer meter)
    {
        Blamp2Point clipper;
        auto buffer = juce::AudioBuffer<float> (1, 2048);
        clipper.prepareToPlay (buffer.getNumSamples(), 44100);
        auto freq = 3000.0;
        clipper.setThreshold (0.3f);
        for (int i = 0; i < buffer.getNumSamples(); ++i)
        {
            buffer.setSample (0,i, std::cos(static_cast<double>(i) * freq * 2 * juce::MathConstants<double>::pi /44100.0));
        }
        clipper.processBlock (buffer);

        // due to complex construction logic of the editor, let's measure open/close together
        meter.measure ([&] (int /* i */) {
            clipper.processBlock (buffer);
        });
    };
    BENCHMARK_ADVANCED ("4 point blamp 3000 hz")
    (Catch::Benchmark::Chronometer meter)
    {
        Blamp4Point clipper;
        auto buffer = juce::AudioBuffer<float> (1, 2048);
        clipper.prepareToPlay (buffer.getNumSamples(), 44100);
        auto freq = 3000.0;
        clipper.setThreshold (0.3f);
        for (int i = 0; i < buffer.getNumSamples(); ++i)
        {
            buffer.setSample (0,i, std::cos(static_cast<double>(i) * freq * 2 * juce::MathConstants<double>::pi /44100.0));
        }
        clipper.processBlock (buffer);

        // due to complex construction logic of the editor, let's measure open/close together
        meter.measure ([&] (int /* i */) {
            clipper.processBlock (buffer);
        });
    };
    BENCHMARK_ADVANCED ("2 point oversample 3000 hz")
    (Catch::Benchmark::Chronometer meter)
    {
        Oversampler2Times clipper;
        auto buffer = juce::AudioBuffer<float> (1, 2048);
        clipper.prepareToPlay (buffer.getNumSamples(), 44100);
        auto freq = 3000.0;
        clipper.setThreshold (0.3f);
        for (int i = 0; i < buffer.getNumSamples(); ++i)
        {
            buffer.setSample (0,i, std::cos(static_cast<double>(i) * freq * 2 * juce::MathConstants<double>::pi /44100.0));
        }
        clipper.processBlock (buffer);

        // due to complex construction logic of the editor, let's measure open/close together
        meter.measure ([&] (int /* i */) {
            clipper.processBlock (buffer);
        });
    };
    BENCHMARK_ADVANCED ("4 point oversample 3000 hz")
    (Catch::Benchmark::Chronometer meter)
    {
        Oversampler4Times clipper;
        auto buffer = juce::AudioBuffer<float> (1, 2048);
        clipper.prepareToPlay (buffer.getNumSamples(), 44100);
        auto freq = 3000.0;
        clipper.setThreshold (0.3f);
        for (int i = 0; i < buffer.getNumSamples(); ++i)
        {
            buffer.setSample (0,i, std::cos(static_cast<double>(i) * freq * 2 * juce::MathConstants<double>::pi /44100.0));
        }
        clipper.processBlock (buffer);

        // due to complex construction logic of the editor, let's measure open/close together
        meter.measure ([&] (int /* i */) {
            clipper.processBlock (buffer);
        });
    };
}
