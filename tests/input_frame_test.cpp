#include "input_frame.hpp"
#include "pitch_detector.hpp"
#include "tuning.hpp"
#include <array>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <limits>
#include <vector>

void expect(bool condition, const char* message) {
    if (!condition) { std::cerr << message << '\n'; std::exit(1); }
}
int main() {
    const std::array<float, 2> left{0.6f,0.0f};
    expect(std::abs(strings::selectInputFrame(left,0).sample-0.6f)<1e-6,"Silent right channel must not halve left signal");
    expect(strings::selectInputFrame(left,1).sample==0,"Right must remain silent when selected");
    const std::array<float, 2> opposite{0.4f,-0.4f};
    expect(strings::selectInputFrame(opposite,0).sample>0 && strings::selectInputFrame(opposite,1).sample<0,"Opposite phases must not cancel selected input");
    const std::array<float, 2> clipping{0.2f,1.0f};
    expect(!strings::selectInputFrame(clipping,0).clipped && strings::selectInputFrame(clipping,1).clipped,"Only selected channel should drive clip warning");
    expect(!strings::selectInputFrame(left,-1).valid && !strings::selectInputFrame(left,2).valid,"Reject invalid channel indices");
    expect(!strings::selectInputFrame({},0).valid,"Reject empty frames");
    const std::array<float, 2> bad{std::numeric_limits<float>::quiet_NaN(),0.2f};
    expect(!strings::selectInputFrame(bad,0).valid && strings::selectInputFrame(bad,0).clipped,"Nonfinite selected sample must be gated");
    expect(strings::selectInputFrame(bad,1).valid,"Bad unrelated channel must not poison selected signal");
    std::vector<float> selected;
    for(int i=0;i<4096;++i) {
        const auto sample=static_cast<float>(0.5*std::sin(2*3.14159265358979323846*82.406889*i/12000));
        const std::array<float, 2> stereo{sample,-sample};
        selected.push_back(strings::selectInputFrame(stereo,1).sample);
    }
    const auto pitch=strings::PitchDetector().estimate(selected,12000,25,700);
    expect(pitch.valid && std::abs(strings::centsBetween(pitch.hz,82.406889))<5,"Selected opposite-phase E2 must still be detected");
    std::cout << "Input channel tests passed\n";
}
