function y = getIdealClippedCosineWave(fs, len, L, f0, tShift)
    function b = boxcar(window_size, begin, ending)
        width = ending - begin;
        shift = (begin + ending) / 2;
        width = width * window_size;
        shift = shift * window_size;
        t = (0:window_size - 1) / window_size;
        inverse_t = (-window_size : -1) / window_size;
    
        left = width * sinc(width * t) .* exp(-1j * 2 * pi * shift * t);
        right = width * sinc(width * inverse_t) .* exp(-1j * 2 * pi * shift * inverse_t);
        b = left + right;
    end

    first_crossing = acos(L) / (2 * pi);
    second_crossing = acos(-L) / (2 * pi);
    window_size = 1024;

    cycle_pass = boxcar(window_size, first_crossing, second_crossing) + boxcar(window_size, 1 - second_crossing, 1 - first_crossing);
    cycle_pass_conv = circshift(cycle_pass, 1) * 0.5 + circshift(cycle_pass, -1) * 0.5;
    first_boxcar = L * boxcar(window_size, -first_crossing, first_crossing);
    second_boxcar = -L * boxcar(window_size, second_crossing, 1-second_crossing);
    full_function = cycle_pass_conv + first_boxcar + second_boxcar;
    nyquist = fs / 2;

    t = (0+tShift:len+tShift-1) * f0 / fs * 2 * pi;
    y = t * 0;

    for k = (1:floor(min(window_size / 2, nyquist/f0)))
        mag = real(full_function(k));
        if (abs(mag) > 0.001)
            y = y + mag * cos(t * (k - 1));
        end
    end
    y = y / (window_size/2);
end

fs = 44100;

len = 65536;
window = hann(len);

rootdirectory = "D:\Courses\Digital Sound Synthesis\final project\antialiased-hard-clipper\Analysis-and-graphs\outfiles";

oversampler2SNR = [];
oversampler4SNR = [];
blamp2SNR = [];
blamp4SNR = [];
naiveSNR = [];

freq_tests = (67:50:10017);
% freq_tests = (67:50:3000);

names = ["naive", "oversampler2x", "oversampler4x", "blamp2x", "blamp4x"];
tShift = [1, 0.5, 0.25, 0, -1];
destinations = createArray(length(names),0);
for freq = freq_tests
    disp(freq);
    for algorithm = 1:5
        [y, fs] = audioread(fullfile(rootdirectory, names(algorithm) + "_" + freq + "_0.5_thresh.wav"));

        ideal = getIdealClippedCosineWave(fs, 65536, 0.5, freq, tShift(algorithm));

        signal = mean(power(ideal, 2));
        noise = mean(power(ideal - y', 2));

        signalToNoiseRatio = 10 * log10(signal / noise);
        idealSpectrum = abs(fft(getIdealClippedCosineWave(fs, 65536, 0.5, freq) .* window'));
        idealSpectrum = idealSpectrum(1:65536/2);

        [idealPeakY, idealSpectralPeaks] = findpeaks(idealSpectrum);

        testSpectrum = abs(fft(y .* window));
        testSpectrum = testSpectrum(1:65536/2);
        [testPeakY, testPeaks] = findpeaks(testSpectrum);

        signal = sum(idealSpectrum);
        noise = sum(abs(idealSpectrum - testSpectrum'));

        % signalGain = 0;
        % noiseGain = 0;
        % 
        % for i = 1:length(testPeaks)
        %     peak = testPeaks(i);
        %     if (any(abs(idealSpectralPeaks - peak) < 2))
        %         signalGain = signalGain + testPeakY(i);
        %     else
        %         noiseGain = noiseGain + testPeakY(i);
        %     end
        % end

        signalToNoiseRatio = 20 * log10(signal / noise);
    
        switch algorithm
            case 1
                naiveSNR = [naiveSNR signalToNoiseRatio];
            case 2
                oversampler2SNR = [oversampler2SNR signalToNoiseRatio];
            case 3
                oversampler4SNR = [oversampler4SNR signalToNoiseRatio];
            case 4
                blamp2SNR = [blamp2SNR signalToNoiseRatio];
            case 5
                blamp4SNR = [blamp4SNR signalToNoiseRatio];
            otherwise
                disp("oops");
        end
    end
end

plot(freq_tests, naiveSNR);
hold on;
plot(freq_tests, oversampler2SNR);
hold on;
plot(freq_tests, oversampler4SNR);
hold on;
plot(freq_tests, blamp2SNR);
hold on;
plot(freq_tests, blamp4SNR);
legend(names);
xlabel('Frequency');
ylabel('Signal/Noise Ratio');
title("Accuracy of clipping algorithms for a sine wave clipped at 0.5");