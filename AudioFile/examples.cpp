#include <iostream>
#define _USE_MATH_DEFINES
#include <cmath>
#include "AudioFile.h"

//=======================================================================
namespace examples
{
    void writeSineWaveToAudioFile();
    void loadAudioFileAndPrintSummary(char *);
    void loadAudioFileAndProcessSamples(char *);
} // namespace examples

//=======================================================================
int main(int argc, char **argv)
{
    //---------------------------------------------------------------
    /** Writes a sine wave to an audio file */
    //examples::writeSineWaveToAudioFile();

    //进行AFL Persistent mode
    while (__AFL_LOOP(10000))
    {
        //---------------------------------------------------------------
        /** Loads an audio file and prints key details to the console*/
        examples::loadAudioFileAndPrintSummary(argv[1]);

        //---------------------------------------------------------------
        /** Loads an audio file and processess the samples */
        examples::loadAudioFileAndProcessSamples(argv[1]);
    }
    return 0;
}

//=======================================================================
namespace examples
{
    //=======================================================================
    void writeSineWaveToAudioFile()
    {
        //---------------------------------------------------------------
        std::cout << "**********************" << std::endl;
        std::cout << "Running Example: Write Sine Wave To Audio File" << std::endl;
        std::cout << "**********************" << std::endl
                  << std::endl;

        //---------------------------------------------------------------
        // 1. Let's setup our AudioFile instance

        AudioFile<float> a;
        a.setNumChannels(2);
        a.setNumSamplesPerChannel(44100);

        //---------------------------------------------------------------
        // 2. Create some variables to help us generate a sine wave

        const float sampleRate = 44100.f;
        const float frequencyInHz = 440.f;

        //---------------------------------------------------------------
        // 3. Write the samples to the AudioFile sample buffer

        for (int i = 0; i < a.getNumSamplesPerChannel(); i++)
        {
            for (int channel = 0; channel < a.getNumChannels(); channel++)
            {
                a.samples[channel][i] = sin((static_cast<float>(i) / sampleRate) * frequencyInHz * 2.f * M_PI);
            }
        }

        //---------------------------------------------------------------
        // 4. Save the AudioFile

        std::string filePath = "sine-wave.wav"; // change this to somewhere useful for you
        a.save("sine-wave.wav", AudioFileFormat::Wave);
    }

    //=======================================================================
    void loadAudioFileAndPrintSummary(char *file)
    {
        //---------------------------------------------------------------
        std::cout << "**********************" << std::endl;
        std::cout << "Running Example: Load Audio File and Print Summary" << std::endl;
        std::cout << "**********************" << std::endl
                  << std::endl;

        //---------------------------------------------------------------
        // 1. Set a file path to an audio file on your machine
        const std::string filePath = std::string(file);

        //---------------------------------------------------------------
        // 2. Create an AudioFile object and load the audio file

        AudioFile<float> a;
        bool loadedOK = a.load(filePath);

        /** If you hit this assert then the file path above
         probably doesn't refer to a valid audio file */
        assert(loadedOK);

        //---------------------------------------------------------------
        // 3. Let's print out some key details

        std::cout << "Bit Depth: " << a.getBitDepth() << std::endl;
        std::cout << "Sample Rate: " << a.getSampleRate() << std::endl;
        std::cout << "Num Channels: " << a.getNumChannels() << std::endl;
        std::cout << "Length in Seconds: " << a.getLengthInSeconds() << std::endl;
        std::cout << std::endl;
    }

    //=======================================================================
    void loadAudioFileAndProcessSamples(char *file)
    {
        //---------------------------------------------------------------
        std::cout << "**********************" << std::endl;
        std::cout << "Running Example: Load Audio File and Process Samples" << std::endl;
        std::cout << "**********************" << std::endl
                  << std::endl;

        //---------------------------------------------------------------
        // 1. Set a file path to an audio file on your machine
        const std::string inputFilePath = std::string(file);

        //---------------------------------------------------------------
        // 2. Create an AudioFile object and load the audio file

        AudioFile<float> a;
        bool loadedOK = a.load(inputFilePath);

        /** If you hit this assert then the file path above
         probably doesn't refer to a valid audio file */
        assert(loadedOK);

        //---------------------------------------------------------------
        // 3. Let's apply a gain to every audio sample

        float gain = 0.5f;

        for (int i = 0; i < a.getNumSamplesPerChannel(); i++)
        {
            for (int channel = 0; channel < a.getNumChannels(); channel++)
            {
                a.samples[channel][i] = a.samples[channel][i] * gain;
            }
        }

        //---------------------------------------------------------------
        // 4. Write audio file to disk

        //std::string outputFilePath = "quieter-audio-filer.wav"; // change this to somewhere useful for you
        //a.save(outputFilePath, AudioFileFormat::Aiff);
    }
} // namespace examples
