## Describe
Two heap-buffer-overflow were discovered in AudioFile. The first one is being triggered in function decodeWaveFile() at AudioFile.h:502, the second one is being triggered in function determineAudioFileFormat() at AudioFile.h:1148.

## Reproduce
test program
    
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

    //__AFL_LOOP() used in AFL Persistent mode, if u don't use AFL compile, just comment 
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
    

    
Tested in parrot 4.9, 64bit.
Compile test program with address sanitizer with this command:

g++ -g -fsanitize=address -o asantry examples.cpp AudioFile.h 
You can get program here.

## ASan Reports
### The first one

./asantry ./out/default/crashes/id\:000005\,sig\:06\,src\:000006\,time\:84641\,op\:havoc\,rep\:2 
#### Get ASan reports

**********************
Running Example: Load Audio File and Print Summary
**********************

=================================================================
==23==ERROR: AddressSanitizer: heap-buffer-overflow on address 0x602000000738 at pc 0x55da0cb245e9 bp 0x7ffc6e244e90 sp 0x7ffc6e244e80
READ of size 1 at 0x602000000738 thread T0
    #0 0x55da0cb245e8 in void std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >::_S_copy_chars<__gnu_cxx::__normal_iterator<unsigned char*, std::vector<unsigned char, std::allocator<unsigned char> > > >(char*, __gnu_cxx::__normal_iterator<unsigned char*, std::vector<unsigned char, std::allocator<unsigned char> > >, __gnu_cxx::__normal_iterator<unsigned char*, std::vector<unsigned char, std::allocator<unsigned char> > >) /usr/include/c++/10/bits/basic_string.h:379
    #1 0x55da0cb226a7 in void std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >::_M_construct<__gnu_cxx::__normal_iterator<unsigned char*, std::vector<unsigned char, std::allocator<unsigned char> > > >(__gnu_cxx::__normal_iterator<unsigned char*, std::vector<unsigned char, std::allocator<unsigned char> > >, __gnu_cxx::__normal_iterator<unsigned char*, std::vector<unsigned char, std::allocator<unsigned char> > >, std::forward_iterator_tag) /usr/include/c++/10/bits/basic_string.tcc:225
    #2 0x55da0cb1fba7 in void std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >::_M_construct_aux<__gnu_cxx::__normal_iterator<unsigned char*, std::vector<unsigned char, std::allocator<unsigned char> > > >(__gnu_cxx::__normal_iterator<unsigned char*, std::vector<unsigned char, std::allocator<unsigned char> > >, __gnu_cxx::__normal_iterator<unsigned char*, std::vector<unsigned char, std::allocator<unsigned char> > >, std::__false_type) /usr/include/c++/10/bits/basic_string.h:247
    #3 0x55da0cb1cfd5 in void std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >::_M_construct<__gnu_cxx::__normal_iterator<unsigned char*, std::vector<unsigned char, std::allocator<unsigned char> > > >(__gnu_cxx::__normal_iterator<unsigned char*, std::vector<unsigned char, std::allocator<unsigned char> > >, __gnu_cxx::__normal_iterator<unsigned char*, std::vector<unsigned char, std::allocator<unsigned char> > >) /usr/include/c++/10/bits/basic_string.h:266
    #4 0x55da0cb19e45 in std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >::basic_string<__gnu_cxx::__normal_iterator<unsigned char*, std::vector<unsigned char, std::allocator<unsigned char> > >, void>(__gnu_cxx::__normal_iterator<unsigned char*, std::vector<unsigned char, std::allocator<unsigned char> > >, __gnu_cxx::__normal_iterator<unsigned char*, std::vector<unsigned char, std::allocator<unsigned char> > >, std::allocator<char> const&) /usr/include/c++/10/bits/basic_string.h:628
    #5 0x55da0cb11fcd in AudioFile<float>::decodeWaveFile(std::vector<unsigned char, std::allocator<unsigned char> >&) /src/AudioFile.h:502
    #6 0x55da0cb0d359 in AudioFile<float>::load(std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >) /src/AudioFile.h:481
    #7 0x55da0cb0554d in examples::loadAudioFileAndPrintSummary(char*) /src/examples.cpp:95
    #8 0x55da0cb04d0e in main /src/examples.cpp:26
    #9 0x7fede8fdb0b2 in __libc_start_main (/lib/x86_64-linux-gnu/libc.so.6+0x270b2)
    #10 0x55da0cb04c0d in _start (/src/asantry+0x4c0d)

0x602000000738 is located 2 bytes to the right of 6-byte region [0x602000000730,0x602000000736)
allocated by thread T0 here:
    #0 0x7fede95a2f17 in operator new(unsigned long) (/lib/x86_64-linux-gnu/libasan.so.6+0xb1f17)
    #1 0x55da0cb1da08 in __gnu_cxx::new_allocator<unsigned char>::allocate(unsigned long, void const*) /usr/include/c++/10/ext/new_allocator.h:115
    #2 0x55da0cb1ac79 in std::allocator_traits<std::allocator<unsigned char> >::allocate(std::allocator<unsigned char>&, unsigned long) /usr/include/c++/10/bits/alloc_traits.h:460
    #3 0x55da0cb16819 in std::_Vector_base<unsigned char, std::allocator<unsigned char> >::_M_allocate(unsigned long) /usr/include/c++/10/bits/stl_vector.h:346
    #4 0x55da0cb195b6 in std::vector<unsigned char, std::allocator<unsigned char> >::_M_default_append(unsigned long) /usr/include/c++/10/bits/vector.tcc:635
    #5 0x55da0cb11896 in std::vector<unsigned char, std::allocator<unsigned char> >::resize(unsigned long) /usr/include/c++/10/bits/stl_vector.h:940
    #6 0x55da0cb0d192 in AudioFile<float>::load(std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >) /src/AudioFile.h:465
    #7 0x55da0cb0554d in examples::loadAudioFileAndPrintSummary(char*) /src/examples.cpp:95
    #8 0x55da0cb04d0e in main /src/examples.cpp:26
    #9 0x7fede8fdb0b2 in __libc_start_main (/lib/x86_64-linux-gnu/libc.so.6+0x270b2)

SUMMARY: AddressSanitizer: heap-buffer-overflow /usr/include/c++/10/bits/basic_string.h:379 in void std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >::_S_copy_chars<__gnu_cxx::__normal_iterator<unsigned char*, std::vector<unsigned char, std::allocator<unsigned char> > > >(char*, __gnu_cxx::__normal_iterator<unsigned char*, std::vector<unsigned char, std::allocator<unsigned char> > >, __gnu_cxx::__normal_iterator<unsigned char*, std::vector<unsigned char, std::allocator<unsigned char> > >)
Shadow bytes around the buggy address:
  0x0c047fff8090: fa fa fd fd fa fa fd fd fa fa 00 02 fa fa 00 02
  0x0c047fff80a0: fa fa 00 02 fa fa 00 02 fa fa 00 02 fa fa 00 02
  0x0c047fff80b0: fa fa 00 02 fa fa 00 02 fa fa 00 02 fa fa 00 02
  0x0c047fff80c0: fa fa 00 02 fa fa 00 02 fa fa 00 02 fa fa 00 02
  0x0c047fff80d0: fa fa 00 02 fa fa 00 02 fa fa 00 02 fa fa 00 02
=>0x0c047fff80e0: fa fa 00 02 fa fa 06[fa]fa fa fa fa fa fa fa fa
  0x0c047fff80f0: fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa
  0x0c047fff8100: fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa
  0x0c047fff8110: fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa
  0x0c047fff8120: fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa
  0x0c047fff8130: fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa
Shadow byte legend (one shadow byte represents 8 application bytes):
  Addressable:           00
  Partially addressable: 01 02 03 04 05 06 07 
  Heap left redzone:       fa
  Freed heap region:       fd
  Stack left redzone:      f1
  Stack mid redzone:       f2
  Stack right redzone:     f3
  Stack after return:      f5
  Stack use after scope:   f8
  Global redzone:          f9
  Global init order:       f6
  Poisoned by user:        f7
  Container overflow:      fc
  Array cookie:            ac
  Intra object redzone:    bb
  ASan internal:           fe
  Left alloca redzone:     ca
  Right alloca redzone:    cb
  Shadow gap:              cc
==23==ABORTING

#### Poc
Poc file is here.

### the second one 
    
./asantry ./out/default/crashes/id\:000000\,sig\:06\,src\:000006\,time\:291\,op\:havoc\,rep\:16 
    
#### Get ASan reports

**********************
Running Example: Load Audio File and Print Summary
**********************

=================================================================
==13==ERROR: AddressSanitizer: heap-buffer-overflow on address 0x602000000731 at pc 0x561c434c55e9 bp 0x7ffeb0ea5a50 sp 0x7ffeb0ea5a40
READ of size 1 at 0x602000000731 thread T0
    #0 0x561c434c55e8 in void std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >::_S_copy_chars<__gnu_cxx::__normal_iterator<unsigned char*, std::vector<unsigned char, std::allocator<unsigned char> > > >(char*, __gnu_cxx::__normal_iterator<unsigned char*, std::vector<unsigned char, std::allocator<unsigned char> > >, __gnu_cxx::__normal_iterator<unsigned char*, std::vector<unsigned char, std::allocator<unsigned char> > >) /usr/include/c++/10/bits/basic_string.h:379
    #1 0x561c434c36a7 in void std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >::_M_construct<__gnu_cxx::__normal_iterator<unsigned char*, std::vector<unsigned char, std::allocator<unsigned char> > > >(__gnu_cxx::__normal_iterator<unsigned char*, std::vector<unsigned char, std::allocator<unsigned char> > >, __gnu_cxx::__normal_iterator<unsigned char*, std::vector<unsigned char, std::allocator<unsigned char> > >, std::forward_iterator_tag) /usr/include/c++/10/bits/basic_string.tcc:225
    #2 0x561c434c0ba7 in void std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >::_M_construct_aux<__gnu_cxx::__normal_iterator<unsigned char*, std::vector<unsigned char, std::allocator<unsigned char> > > >(__gnu_cxx::__normal_iterator<unsigned char*, std::vector<unsigned char, std::allocator<unsigned char> > >, __gnu_cxx::__normal_iterator<unsigned char*, std::vector<unsigned char, std::allocator<unsigned char> > >, std::__false_type) /usr/include/c++/10/bits/basic_string.h:247
    #3 0x561c434bdfd5 in void std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >::_M_construct<__gnu_cxx::__normal_iterator<unsigned char*, std::vector<unsigned char, std::allocator<unsigned char> > > >(__gnu_cxx::__normal_iterator<unsigned char*, std::vector<unsigned char, std::allocator<unsigned char> > >, __gnu_cxx::__normal_iterator<unsigned char*, std::vector<unsigned char, std::allocator<unsigned char> > >) /usr/include/c++/10/bits/basic_string.h:266
    #4 0x561c434bae45 in std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >::basic_string<__gnu_cxx::__normal_iterator<unsigned char*, std::vector<unsigned char, std::allocator<unsigned char> > >, void>(__gnu_cxx::__normal_iterator<unsigned char*, std::vector<unsigned char, std::allocator<unsigned char> > >, __gnu_cxx::__normal_iterator<unsigned char*, std::vector<unsigned char, std::allocator<unsigned char> > >, std::allocator<char> const&) /usr/include/c++/10/bits/basic_string.h:628
    #5 0x561c434b2a75 in AudioFile<float>::determineAudioFileFormat(std::vector<unsigned char, std::allocator<unsigned char> >&) /src/AudioFile.h:1148
    #6 0x561c434ae2ee in AudioFile<float>::load(std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >) /src/AudioFile.h:477
    #7 0x561c434a654d in examples::loadAudioFileAndPrintSummary(char*) /src/examples.cpp:95
    #8 0x561c434a5d0e in main /src/examples.cpp:26
    #9 0x7f99260b30b2 in __libc_start_main (/lib/x86_64-linux-gnu/libc.so.6+0x270b2)
    #10 0x561c434a5c0d in _start (/src/asantry+0x4c0d)

0x602000000731 is located 0 bytes to the right of 1-byte region [0x602000000730,0x602000000731)
allocated by thread T0 here:
    #0 0x7f992667af17 in operator new(unsigned long) (/lib/x86_64-linux-gnu/libasan.so.6+0xb1f17)
    #1 0x561c434bea08 in __gnu_cxx::new_allocator<unsigned char>::allocate(unsigned long, void const*) /usr/include/c++/10/ext/new_allocator.h:115
    #2 0x561c434bbc79 in std::allocator_traits<std::allocator<unsigned char> >::allocate(std::allocator<unsigned char>&, unsigned long) /usr/include/c++/10/bits/alloc_traits.h:460
    #3 0x561c434b7819 in std::_Vector_base<unsigned char, std::allocator<unsigned char> >::_M_allocate(unsigned long) /usr/include/c++/10/bits/stl_vector.h:346
    #4 0x561c434ba5b6 in std::vector<unsigned char, std::allocator<unsigned char> >::_M_default_append(unsigned long) /usr/include/c++/10/bits/vector.tcc:635
    #5 0x561c434b2896 in std::vector<unsigned char, std::allocator<unsigned char> >::resize(unsigned long) /usr/include/c++/10/bits/stl_vector.h:940
    #6 0x561c434ae192 in AudioFile<float>::load(std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >) /src/AudioFile.h:465
    #7 0x561c434a654d in examples::loadAudioFileAndPrintSummary(char*) /src/examples.cpp:95
    #8 0x561c434a5d0e in main /src/examples.cpp:26
    #9 0x7f99260b30b2 in __libc_start_main (/lib/x86_64-linux-gnu/libc.so.6+0x270b2)

SUMMARY: AddressSanitizer: heap-buffer-overflow /usr/include/c++/10/bits/basic_string.h:379 in void std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >::_S_copy_chars<__gnu_cxx::__normal_iterator<unsigned char*, std::vector<unsigned char, std::allocator<unsigned char> > > >(char*, __gnu_cxx::__normal_iterator<unsigned char*, std::vector<unsigned char, std::allocator<unsigned char> > >, __gnu_cxx::__normal_iterator<unsigned char*, std::vector<unsigned char, std::allocator<unsigned char> > >)
Shadow bytes around the buggy address:
  0x0c047fff8090: fa fa fd fd fa fa fd fd fa fa 00 02 fa fa 00 02
  0x0c047fff80a0: fa fa 00 02 fa fa 00 02 fa fa 00 02 fa fa 00 02
  0x0c047fff80b0: fa fa 00 02 fa fa 00 02 fa fa 00 02 fa fa 00 02
  0x0c047fff80c0: fa fa 00 02 fa fa 00 02 fa fa 00 02 fa fa 00 02
  0x0c047fff80d0: fa fa 00 02 fa fa 00 02 fa fa 00 02 fa fa 00 02
=>0x0c047fff80e0: fa fa 00 02 fa fa[01]fa fa fa fa fa fa fa fa fa
  0x0c047fff80f0: fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa
  0x0c047fff8100: fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa
  0x0c047fff8110: fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa
  0x0c047fff8120: fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa
  0x0c047fff8130: fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa
Shadow byte legend (one shadow byte represents 8 application bytes):
  Addressable:           00
  Partially addressable: 01 02 03 04 05 06 07 
  Heap left redzone:       fa
  Freed heap region:       fd
  Stack left redzone:      f1
  Stack mid redzone:       f2
  Stack right redzone:     f3
  Stack after return:      f5
  Stack use after scope:   f8
  Global redzone:          f9
  Global init order:       f6
  Poisoned by user:        f7
  Container overflow:      fc
  Array cookie:            ac
  Intra object redzone:    bb
  ASan internal:           fe
  Left alloca redzone:     ca
  Right alloca redzone:    cb
  Shadow gap:              cc
==13==ABORTING

#### Poc
Poc file is here.
    
## Fuzzer & Testcase
Fuzzer is AFLplusplus.
Testcase is in here.
I use your testcase in file tests/test-audio/ and I choose .wav files. Then I use afl-cmin to minisize these files.
