#ifndef MY_ROO_FIT_SETTING_H
#define MY_ROO_FIT_SETTING_H

class MyRooFitSetting {
public:

  double peak;
  double minPeakWidth;
  double maxPeakWidth;
  double xStart;
  double xEnd;
  double binWidth;
  double fitStart;
  double fitEnd;
  double fitBinWidth;

  MyRooFitSetting(
    double peak,
    double minPeakWidth,
    double maxPeakWidth,
    double xStart,
    double xEnd,
    double binWidth,
    double fitStart,
    double fitEnd,
    double fitBinWidth
    );
};

#endif // ifndef MY_ROO_FIT_SETTING_H
