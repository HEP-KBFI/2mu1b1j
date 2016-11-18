#ifndef MY_ROO_FIT_RESULT_H
#define MY_ROO_FIT_RESULT_H

class MyRooFitResult {
public:

  RooPlot *frame;
  double   x;
  double   signalEventsCount;
  double   fitError;
  double   backgroundEventsCount;
  double   breitWignerMean;
  double   breitWignerWidth;
  double   gaussMean;
  double   gaussWidth;
  double   backgroundA;
  double   backgroundB;
  double   backgroundC;

  MyRooFitResult();
  ~MyRooFitResult() {
    delete this->frame;
  }

  double getPull();
  double getPValue();
  string getInfo();
};

#endif // ifndef MY_ROO_FIT_RESULT_H
