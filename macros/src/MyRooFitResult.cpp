#include "MyRooFitResult.hpp"

MyRooFitResult::MyRooFitResult()
{}

double MyRooFitResult::getPull() {
  double pull = this->signalEventsCount / this->fitError;

  return pull;
}

double MyRooFitResult::getPValue() {
  double pull   = this->getPull();
  double pValue = 0.5 * TMath::Erf(pull / TMath::Sqrt(2.0));

  return pValue;
}

string MyRooFitResult::getInfo() {
  string info = string("")
                + "\tx: " + to_string(x)
                + "\tsignalEventsCount: " + to_string(signalEventsCount)
                + "\tbackgroundEventsCount: " + to_string(backgroundEventsCount)
                + "\tfitError: " + to_string(fitError)
                + "\tbreitWignerMean: " + to_string(breitWignerMean)
                + "\tbreitWignerWidth: " + to_string(breitWignerWidth)
                + "\tgaussMean: " + to_string(gaussMean)
                + "\tgaussWidth: " + to_string(gaussWidth)
                + "\tbackgroundA: " + to_string(backgroundA)
                + "\tbackgroundB: " + to_string(backgroundB)
                + "\tbackgroundC: " + to_string(backgroundC)
                + "\tpull: " + to_string(this->getPull())
                + "\tpValue: " + to_string(this->getPValue());

  return info;
}
