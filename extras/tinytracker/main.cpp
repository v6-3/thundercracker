#include <sifteo.h>
#include "assets.gen.h"
using namespace Sifteo;

static Metadata M = Metadata()
    .title("Tinytracker")
    .package("com.sifteo.extras.tinytracker", "1.0")
    .cubeRange(0);

void main()
{
    AudioTracker::play(Track);
    while (1)
	System::yield();
}

