#include <vector>
void solveTwoUnknowsSystem(std::vector<float>&& coef, float& out1, float& out2){
    float md = (-coef[1] * coef[2] + coef[3] * coef[0]);
    out2 = (-coef[1] * coef[4] + coef[5] * coef[0]) / md;
    out1 = (-coef[5] * coef[2] + coef[4] * coef[3]) / md;

}