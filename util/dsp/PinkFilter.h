
#pragma once

template <typename T>
class PinkFilter {
public:
    T process(T);
private:
    // Noise filter coefficients
    // From Paul Kellete. Way more poles than we need, but why not?

    // https://www.musicdsp.org/en/latest/Filters/76-pink-noise-filter.html
    // https://www.firstpr.com.au/dsp/pink-noise/
    // https://www.mit.edu/~gari/CODE/NOISE/pinkNoise.m
    const T _ka0 = .99886;
    const T _ka1 = 0.9933;
    const T _ka2 = 0.96900;
    const T _ka3 = 0.86650;
    const T _ka4 = 0.55000;
    const T _ka5 = -0.76162;
    const T _ka6 = 0.115926;

    const T _kb0 = 0.0555179;
    const T _kb1 = 0.0750759;
    const T _kb2 = 0.1538520;
    const T _kb3 = 0.3104856;
    const T _kb4 = 0.5329522;
    const T _kb5 = 0.0168980;

    // Noise filter state (delay memory)
    T b0 = 0;
    T b1 = 0;
    T b2 = 0;
    T b3 = 0;
    T b4 = 0;
    T b5 = 0;
    T b6 = 0;
};

template <typename T>
inline T PinkFilter<T>::process(T input) {
#if 1
        // everything off - flat above 100, some weird hump below that
        // just b0 - down 72 db at 125hz, back up to -60 db at 150
        // at high sr: -3db at 50hz, down to 
        b0 = _ka0 * b0 + input * _kb0;

       // down 40db at  900? smooth fall till end -45 db there
         b1 = _ka1 * b1 + input * _kb1;

       // 6db hump, falls to -12 at nyquist
         b2 = _ka2 * b2 + input * _kb2;

        // 6 db bump at 125, then falls of to about -5db at quist
         b3 = _ka3 * b3 + input * _kb3;

        // small bump at 250hz, then slowly falls off till about -6 db at nyquist
         b4 = _ka4 * b4 + input * _kb4;

        // pretty flat - very slight rise over full range.
        b5 = _ka5 * b5 + input * _kb5;
        T ret = (b0 + b1 + b2 + b3 + b4 + b5 + b6 + input * 0.5362) * 0.003;
        b6 = input * _ka6;
        ret *= 100;
    #else
        T ret = input;
    #endif
        return ret;
}