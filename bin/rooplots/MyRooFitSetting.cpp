#include "MyRooFitSetting.hpp"

MyRooFitSetting::MyRooFitSetting(
  double peak,
  double minPeakWidth,
  double maxPeakWidth,
  double xStart,
  double xEnd,
  double binWidth,
  double fitStart,
  double fitEnd,
  double fitBinWidth
  )
{
  this->peak         = peak;
  this->minPeakWidth = minPeakWidth;
  this->maxPeakWidth = maxPeakWidth;
  this->xStart       = xStart;
  this->xEnd         = xEnd;
  this->binWidth     = binWidth;
  this->fitStart     = fitStart;
  this->fitEnd       = fitEnd;
  this->fitBinWidth  = fitBinWidth;
}
