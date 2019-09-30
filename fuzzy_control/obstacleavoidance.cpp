//File: ObstacleAvoidance.cpp
#include "fl/Headers.h"

int main(int argc, char* argv[]){
    using namespace fl;
    Engine* engine = FllImporter().fromFile("ObstacleAvoidance.fll");

    std::string status;
    if (not engine->isReady(&status))
        throw Exception("[engine error] engine is not ready:n" + status, FL_AT);

    InputVariable* obstacle = engine->getInputVariable("obstacle");
    InputVariable* distance = engine->getInputVariable("distance");
    OutputVariable* steer = engine->getOutputVariable("mSteer");

    for (int i = 0; i <= 50; ++i){
        scalar location = obstacle->getMinimum() + i * (obstacle->range() / 50);
        for(int j = 0; j <= 50; j++){
          scalar dist = distance->getMinimum() + j * (distance->range() / 50);
          distance->setValue(dist);
          engine->process();
          FL_LOG("obstacle.input = " << Op::str(location) << " distance.input = " << Op::str(dist) << " => " << "steer.output = " << Op::str(steer->getValue()));
        }

        obstacle->setValue(location);
        //engine->process();
        /*FL_LOG("obstacle.input = " << Op::str(location) <<
            " => " << "steer.output = " << Op::str(steer->getValue()));*/
    }
}
