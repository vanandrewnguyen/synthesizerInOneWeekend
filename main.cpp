#include <iostream>
#include "src/noiseMaker.h"
#include "src/synthEngine.h"

int main() {
    std::unique_ptr<SynthEngine> engine = std::make_unique<SynthEngine>();

    return 0;
}


