function y = getIdealClippedCosineWave(fs, len, L, f0)
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

    t = (1:len) * f0 / fs * 2 * pi;
    y = t * 0;

    for k = (1:floor(min(window_size / 2, nyquist/f0)))
        mag = real(full_function(k));
        if (abs(mag) > 0.001)
            y = y + mag * cos(t * (k - 1));
        end
    end
    y = y / (window_size/2);
end

rootdirectory = "D:\Courses\Digital Sound Synthesis\final project\antialiased-hard-clipper\Analysis-and-graphs\outfiles";

fs = 44100;

len = 65536;
window = hann(len);
goodX = [];
goodY = [];
goodColor = [];

aliasX = [];
aliasY = [];
aliasColor = [];
for freq = (50+17:50:20000+17)
    [y, fs] = audioread(fullfile(rootdirectory, "blamp2x_" + freq + "_0.5_thresh.wav"));
    idealSpectrum = abs(fft(getIdealClippedCosineWave(fs, 65536, 0.5, freq) .* window'));
    idealSpectrum = log10(idealSpectrum(1:65536/2));
    [idealPeakY, idealSpectralPeaks] = findpeaks(idealSpectrum);
    
    testSpectrum = abs(fft(y .* window));
    testSpectrum = log10(testSpectrum(1:65536/2));
    [testPeakY, testPeaks] = findpeaks(testSpectrum);

    goodIndexes = arrayfun(@(peak) any(abs(idealSpectralPeaks - peak) < 2), testPeaks);
    
    goodX = [goodX testPeaks(goodIndexes)' * 0 + freq];
    goodY = [goodY testPeakY(goodIndexes)'];
    goodColor = [goodColor testPeakY(goodIndexes)'];
    aliasX = [aliasX testPeaks(~goodIndexes)' * 0 + freq];
    aliasY = [aliasY testPeakY(~goodIndexes)'];
    aliasColor = [aliasColor testPeakY(~goodIndexes)'];
end

% combined = power(rescale([goodColor aliasColor]), 6);
% goodColor = combined(1:length(goodColor));
% aliasColor = combined(length(goodColor)+1:length(combined));
% s = scatter(goodX, goodY, 20, 'blue', 'filled');
% alpha(s, 0.2);
% hold on

s = scatter(aliasX, aliasY, 20, 'red', 'filled');
alpha(s, 0.2);
hold on

len = 65536;
window = hann(len);
goodX = [];
goodY = [];
goodColor = [];

aliasX = [];
aliasY = [];
aliasColor = [];
for freq = (50:50:10000)
    [y, fs] = audioread(fullfile(rootdirectory, "naive_" + freq + "_0.5_thresh.wav"));
    idealSpectrum = abs(fft(getIdealClippedCosineWave(fs, 65536, 0.5, freq) .* window'));
    idealSpectrum = log10(idealSpectrum(1:65536/2));
    [idealPeakY, idealSpectralPeaks] = findpeaks(idealSpectrum);
    
    testSpectrum = abs(fft(y .* window));
    testSpectrum = log10(testSpectrum(1:65536/2));
    [testPeakY, testPeaks] = findpeaks(testSpectrum);

    goodIndexes = arrayfun(@(peak) any(abs(idealSpectralPeaks - peak) < 2), testPeaks);
    
    goodX = [goodX testPeaks(goodIndexes)' * 0 + freq];
    goodY = [goodY testPeakY(goodIndexes)'];
    goodColor = [goodColor testPeakY(goodIndexes)'];
    aliasX = [aliasX testPeaks(~goodIndexes)' * 0 + freq];
    aliasY = [aliasY testPeakY(~goodIndexes)'];
    aliasColor = [aliasColor testPeakY(~goodIndexes)'];
end

% combined = power(rescale([goodColor aliasColor]), 6);
% goodColor = combined(1:length(goodColor));
% aliasColor = combined(length(goodColor)+1:length(combined));
% s = scatter(goodX, goodY, 20, 'blue', 'filled');
% alpha(s, 0.2);
% hold on

s = scatter(aliasX, aliasY, 20, 'blue', 'filled');
alpha(s, 0.2);
hold on
