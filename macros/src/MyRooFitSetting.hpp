#ifndef MY_ROO_FIT_SETTING_H
#define MY_ROO_FIT_SETTING_H

class MyRooFitSetting {
public:

  double peak;
  double minPeakWidth;
  double maxPeakWidth;
  double xStart;
  double xEnd;
  double binning;
  double fitStart;
  double fitEnd;
  double fitBinning;

  MyRooFitSetting(
    double peak,
    double minPeakWidth,
    double maxPeakWidth,
    double xStart,
    double xEnd,
    double binning,
    double fitStart,
    double fitEnd,
    double fitBinning
    );
};

#endif // ifndef MY_ROO_FIT_SETTING_H
