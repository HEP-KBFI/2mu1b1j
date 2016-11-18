MyRooFitSetting::MyRooFitSetting(
  double peak,
  double minPeakWidth,
  double maxPeakWidth,
  double xStart,
  double xEnd,
  double binning,
  double fitStart,
  double fitEnd,
  double fitBinning
  )
{
  this->peak         = peak;
  this->minPeakWidth = minPeakWidth;
  this->maxPeakWidth = maxPeakWidth;
  this->xStart       = xStart;
  this->xEnd         = xEnd;
  this->binning      = binning;
  this->fitStart     = fitStart;
  this->fitEnd       = fitEnd;
  this->fitBinning   = fitBinning;
}
