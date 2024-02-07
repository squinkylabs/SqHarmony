#pragma once

// VCV usually uses a c++ "struct" as an object. We use the more common way of doing it, using
// a c++ "class". The two only differ in small ways.
class Filter4PButter {
public:
    //	void setParameters(Type type, float f, float Q, float V) {

    // This is where the caller would pass in the desired cutoff frequency, and
    // we then set up the biquads with the right coefficients.
    // Note the following:
    //  1) normalizeCutoff is the desired cutoff frequency, scaled to the sample rate, where 0 is
    //  zero frequency, and 1.0 is the sample rate. Of course in any DSP we can never do things above sample
    //  rate / 2, so this must never be greater than .5.
    //
    //  2) The VCV biquad lets you set it up by specifying a type of response, a frequency, a "Q", and a gain.
    //  we use 1 for the gain, and the caller specifies the cutoff. We tell VCV we want a LOWPASS filter, which
    //  it interprets as "two pole low pass". And we pass in these magic numbers for "Q". There are ways you
    //  could derive these numbers yourself, but they are difficult and error prone is you aren't good at math.
    //  It's much easier to look them up.
    //
    //  3) Here is the online calculator we use to get the Q numbers: https://www.earlevel.com/main/2016/09/29/cascading-filters/
    void setCutoffFreq(float normalizedCutoff) {
        assert(normalizedCutoff > 0 && normalizedCutoff < .5f);
        f[0].setParameters(rack::dsp::TBiquadFilter<float>::LOWPASS, normalizedCutoff, 0.54119610, 1);
        f[1].setParameters(rack::dsp::TBiquadFilter<float>::LOWPASS, normalizedCutoff, 1.3065630, 1);
      
    }

    // Process takes one sample of input, and generates one sample of output.
    float process(float x) {
        x = f[0].process(x);  // filter input through biquad #1
        x = f[1].process(x);  // filter the output of biquad #1 through biquad #2
        return x;             // return the output of biquad #3
    }

private:
    // A 6 pole filter requires three biquads in series,
    // one biquad for each pair of poles.
    //
    // TBiquadFilter is a type that comes with the VCV SDK.
    // It is a very reasonable implementation of a biquad, and
    // it may be templatized with float_4 for SIMD operation. Here
    // we are just using float.
    rack::dsp::TBiquadFilter<float> f[2];
};
