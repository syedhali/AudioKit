//
//  AKSquareWaveOscillatorDSPKernel.hpp
//  AudioKit
//
//  Autogenerated by scripts by Aurelius Prochazka. Do not edit directly.
//  Copyright (c) 2016 Aurelius Prochazka. All rights reserved.
//

#ifndef AKSquareWaveOscillatorDSPKernel_hpp
#define AKSquareWaveOscillatorDSPKernel_hpp

#import "AKDSPKernel.hpp"
#import "AKParameterRamper.hpp"

extern "C" {
#include "soundpipe.h"
}

enum {
    frequencyAddress = 0,
    amplitudeAddress = 1,
    pulseWidthAddress = 2
};

class AKSquareWaveOscillatorDSPKernel : public AKDSPKernel {
public:
    // MARK: Member Functions

    AKSquareWaveOscillatorDSPKernel() {}

    void init(int channelCount, double inSampleRate) {
        channels = channelCount;

        sampleRate = float(inSampleRate);

        sp_create(&sp);
        sp_blsquare_create(&blsquare);
        sp_blsquare_init(sp, blsquare);
        *blsquare->freq = 440;
        *blsquare->amp = 1.0;
        *blsquare->width = 0.5;
    }

    void start() {
        started = true;
    }

    void stop() {
        started = false;
    }

    void destroy() {
        sp_blsquare_destroy(&blsquare);
        sp_destroy(&sp);
    }

    void reset() {
    }

    void setParameter(AUParameterAddress address, AUValue value) {
        switch (address) {
            case frequencyAddress:
                frequencyRamper.set(clamp(value, (float)0, (float)20000));
                break;

            case amplitudeAddress:
                amplitudeRamper.set(clamp(value, (float)0, (float)10));
                break;

            case pulseWidthAddress:
                pulseWidthRamper.set(clamp(value, (float)0, (float)1));
                break;

        }
    }

    AUValue getParameter(AUParameterAddress address) {
        switch (address) {
            case frequencyAddress:
                return frequencyRamper.goal();

            case amplitudeAddress:
                return amplitudeRamper.goal();

            case pulseWidthAddress:
                return pulseWidthRamper.goal();

            default: return 0.0f;
        }
    }

    void startRamp(AUParameterAddress address, AUValue value, AUAudioFrameCount duration) override {
        switch (address) {
            case frequencyAddress:
                frequencyRamper.startRamp(clamp(value, (float)0, (float)20000), duration);
                break;

            case amplitudeAddress:
                amplitudeRamper.startRamp(clamp(value, (float)0, (float)10), duration);
                break;

            case pulseWidthAddress:
                pulseWidthRamper.startRamp(clamp(value, (float)0, (float)1), duration);
                break;

        }
    }

    void setBuffer(AudioBufferList *outBufferList) {
        outBufferListPtr = outBufferList;
    }

    void process(AUAudioFrameCount frameCount, AUAudioFrameCount bufferOffset) override {
        // For each sample.
        for (int frameIndex = 0; frameIndex < frameCount; ++frameIndex) {
            double frequency = double(frequencyRamper.getStep());
            double amplitude = double(amplitudeRamper.getStep());
            double pulseWidth = double(pulseWidthRamper.getStep());

            int frameOffset = int(frameIndex + bufferOffset);

            *blsquare->freq = (float)frequency;
            *blsquare->amp = (float)amplitude;
            *blsquare->width = (float)pulseWidth;

            float temp = 0;
            for (int channel = 0; channel < channels; ++channel) {
                float *out = (float *)outBufferListPtr->mBuffers[channel].mData + frameOffset;
                if (started) {
                    if (channel == 0) {
                        sp_blsquare_compute(sp, blsquare, nil, &temp);
                    }
                    *out = temp;
                } else {
                    *out = 0.0;
                }
            }
        }
    }

    // MARK: Member Variables

private:

    int channels = 2;
    float sampleRate = 44100.0;

    AudioBufferList *outBufferListPtr = nullptr;

    sp_data *sp;
    sp_blsquare *blsquare;

public:
    bool started = false;
    AKParameterRamper frequencyRamper = 440;
    AKParameterRamper amplitudeRamper = 1.0;
    AKParameterRamper pulseWidthRamper = 0.5;
};

#endif /* AKSquareWaveOscillatorDSPKernel_hpp */