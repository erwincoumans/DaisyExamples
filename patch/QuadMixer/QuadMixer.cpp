#include "daisysp.h"
#include "daisy_patch.h"

using namespace daisy;
using namespace daisysp;

DaisyPatch patch;

float ctrlVal[4];
namespace test {
#include "printf.h"
#include "printf.c"
};
char buf[64];

static void AudioCallback(float **in, float **out, size_t size)
{
    for (int i = 0; i < 4; i++)
    {
	//Get the four control values
	ctrlVal[i] = patch.controls[i].Process();
        if (ctrlVal[i]<0.003)
           ctrlVal[i] = 0;
        if (ctrlVal[i]>0.97)
           ctrlVal[i]=1;
    }
    
    for (size_t i = 0; i < size; i++)
    {
	float output = 0.f;
	//sum all four inputs, attenuated by the control levels
	for (size_t chn = 0; chn < 4; chn++)
        {
	    output += ctrlVal[chn] * in[chn][i];
        }

	//attenuate by 1/4
	output *= .25f;

	//send the same thing to all 4 outputs
	for (size_t chn = 0; chn < 4; chn++)
        {
            out[chn][i] = output;
        }
    }
}

void UpdateOled()
{
    patch.display.Fill(false);

    //test::sprintf(buf,"%s", "QuadMixer");
    //patch.display.SetCursor(0,0);
    //patch.display.WriteString(buf, Font_6x8, true);

    for (int i=0;i<4;i++)
    {
      //two circuits
      patch.display.SetCursor(0, i*10);
      test::sprintf(buf, "ctrl%d:%1.5f", i, ctrlVal[i]);
      patch.display.WriteString(buf, Font_7x10, true);
    }

    patch.display.Update();
}


int main(void)
{
    patch.Init(); // Initialize hardware (daisy seed, and patch)

    //start audio
    patch.StartAdc();
    patch.StartAudio(AudioCallback);

    
    while(1)
    {
	//display the four control levels
	//patch.DisplayControls(false);
        UpdateOled();
    }
}
