#ifndef MRVM_H
#define MRVM_H

#include "mltools_global.h"
#include <list>
#include <QString>
#include <QMap>
#include <QFile>
#include <QPoint>
#include <QPointF>
#include <QFileInfo>
#include <QVariant>
#include <QPolygon>
#include <QSet>
#include <arrayfire.h>
#include <QXmlStreamReader>
#include "gdal_priv.h"
#include "cpl_conv.h" // for CPLMalloc()
#include <QRegularExpression>

#ifndef NDEBUG
#   define ASSERT(condition, message) \
  do { \
  if (! (condition)) { \
  std::cerr << "Assertion `" #condition "` failed in " << __FILE__ \
  << " line " << __LINE__ << ": " << message << std::endl; \
  std::exit(EXIT_FAILURE); \
  } \
  } while (false)
#else
#   define ASSERT(condition, message) do { } while (false)
#endif


# ifndef NUM_THREADS
#define NUM_THREADS 4
#endif

class MLTOOLS_EXPORT MRVMItem
{
  
public:
  enum MRVMValueType
  {
    Real,
    Categorical
  };

  enum IOType
  {
    Input,
    Output
  };

public:
  MRVMItem(IOType type = IOType::Input, const QString& name = "");
  
  virtual ~MRVMItem();

  QString name() const;

  void setName(const QString& name);
  
  void clearAllValues();

  const QList<QString>& trainingValuesAsString() const;

  virtual af::array trainingValues(int row) = 0;

  virtual void setTrainingValuesAsString(const QList<QString>& trainingValues);

  const QList<QString>& forecastValuesAsString() const;

  virtual af::array forecastValues(int row) = 0;

  virtual void setForecastValuesAsString(const QList<QString>& forecastValues);

  virtual void setForecastValues(int row, const af::array& values, const af::array& uncertainty) = 0;

  const QList<QString>& forecastUncertaintyValuesAsString() const;

  virtual void setForecastUncertaintyValueAsString(const QList<QString>& forecastUncertaintyValuesAsString);

  virtual void readXML(QXmlStreamReader & xmlReader);

  virtual void writeXML(QXmlStreamWriter& xmlWriter);

  virtual int columnCount() = 0;
  
  virtual int numTrainingValues() const;

  virtual int numForecastValues() const;

  virtual int numRowsPerValue() const;

  const QMap<QString, QVariant>& properties() const;

  virtual void setProperties(const QMap<QString, QVariant>& properties);
  
  virtual MRVMValueType valueType() const = 0;
  
  virtual QString type() const = 0;

  virtual IOType ioType() const;

  virtual QString toString() const;

protected:
  IOType m_iotype;
  QString m_name;
  QMap<QString, QVariant> m_properties;
  QList<QString> m_trainingValuesAsString,  m_forecastValuesAsString, m_forecastUncertaintyValuesAsString;
  int m_numTrainingValues, m_numForecastValues;

};

class MLTOOLS_EXPORT RealMRVMItem : public MRVMItem
{
  
public:
  
  RealMRVMItem(IOType type = IOType::Input,const QString& name = "");
  
  ~RealMRVMItem();
  
  af::array trainingValues(int row) override;

  virtual void setTrainingValuesAsString(const QList<QString>& trainingValues ) override;

  af::array forecastValues(int row) override;

  virtual void setForecastValues(int row, const af::array& values, const af::array& uncertainty) override;

  virtual void readXML(QXmlStreamReader & xmlReader) override;

  virtual void writeXML(QXmlStreamWriter& xmlWriter) override;

  int columnCount() override;

  virtual int numTrainingValues() const override;

  virtual int numForecastValues() const override;

  MRVMValueType valueType() const override;

  QString type() const override;

private:
  void readWriteTrainingValuesFiles(bool read = true);

  void readWriteForecastValuesFiles(bool read = true);

  void readWriteForecastUncertaintyValuesFiles(bool read = true);

  void expandListTo(QList<float>& list, int index);

private:
  QList<float> m_trainingValues;
  QList<float> m_forecastValues;
  QList<float> m_forecastUncertaintyValues;

};

class MLTOOLS_EXPORT RealArrayMRVMItem : public MRVMItem
{

public:
  RealArrayMRVMItem(IOType type = IOType::Input, const QString& name="");

  ~RealArrayMRVMItem();

  virtual af::array trainingValues(int row) override;

  virtual void setTrainingValuesAsString(const QList<QString>& trainingValues) override;

  virtual af::array forecastValues(int row) override;

  virtual void setForecastValues(int row, const af::array& values, const af::array& uncertainty) override;

  virtual void readXML(QXmlStreamReader & xmlReader) override;

  virtual void writeXML(QXmlStreamWriter& xmlWriter) override;

  virtual int columnCount() override;

  virtual int numTrainingValues() const override;

  virtual int numForecastValues() const override;

  virtual MRVMValueType valueType() const override;

  virtual QString type() const override;

private:
  void readWriteTrainingValuesFiles(bool read = true);

  void readWriteForecastValuesFiles(bool read = true);

  void readWriteForecastUncertaintyValuesFiles(bool read = true);

  void expandListTo(QList<QList<float> >& list, int index);

private:
  QList<QList<float> > m_trainingValues;
  QList<QList<float> > m_forecastValues;
  QList<QList<float> > m_forecastUncertaintyValues;
  static QRegularExpression s_regex;

protected:
  int m_columnCount;

};

class MLTOOLS_EXPORT CategoricalMRVMItem : public MRVMItem
{

public:
  CategoricalMRVMItem(IOType type = IOType::Input, const QString& name = "");

  ~CategoricalMRVMItem();

  virtual QMap<QString,int> getCategories() const;

  void setCategories(const QMap<QString,int>& categories);

  virtual af::array trainingValues(int row) override;

  virtual af::array forecastValues(int row) override;

  virtual void setForecastValues(int row, const af::array& values,
                                 const af::array& uncertainty) override;

  virtual void readXML(QXmlStreamReader & xmlReader) override;

  virtual void writeXML(QXmlStreamWriter& xmlWriter) override;

  virtual int columnCount() override;


  virtual int numTrainingValues() const override;

  virtual int numForecastValues() const override;


  virtual MRVMValueType valueType() const override;

  virtual QString type() const override;

  virtual QString toString() const override;

  QMap<QString, int> categories() const;

private:
  void readWriteTrainingValuesFiles(bool read = true);

  void readWriteForecastValuesFiles(bool read = true);

  void readWriteForecastUncertaintyValuesFiles(bool read = true);

  void expandListTo(QList<int>& list, int index);

  void expandListTo(QList<float>& list, int index);

private:
  QList<int> m_trainingValues;
  QList<int> m_forecastValues;
  QList<float> m_forecastUncertaintyValues;

protected:
  QMap<QString,int> m_classbycategory;
  QMap<int,QString> m_categorybyclass;
  QMap<int,int> m_classbyindex;
  QMap<int,int> m_indexbyclass;\
  float maxCValue, minCValue;

};

class MLTOOLS_EXPORT RasterItem
{

public:
  RasterItem();

  virtual ~RasterItem();

  virtual QString getName() const = 0;

  bool contains(const QPointF& point);

  bool isValid(const QPoint& index);

  QPointF getCoordinates(const QPoint& indexes)  const;

  QPoint getCoordinateIndexes(const QPointF& coordinates) const;

  void setBootStrapPoints(const QList<QPoint>& centers, const QList<QList<QPoint> >& indexes);

  bool includeDistanceWithBootstrap() const;

  bool setIncludeDistanceWithBootstrap(bool includeDistanceWithBootstrap);

  virtual QPolygonF boundary() const;

protected:
  bool m_useRasterBootStrap;
  QList<QList<QPoint> > m_bootStrapSamplingPoints;
  QList<QPoint> m_bootStrapCenters;
  int m_xSize, m_ySize;
  int* m_validCell;
  float m_noData;
  GDALDriver* m_driver;
  double m_gcp[6];
  const char* m_wktproj;
  bool m_includeDistanceWhenBootstrapping;
};

class MLTOOLS_EXPORT RealRaster : public RealArrayMRVMItem, public RasterItem
{

public:

  RealRaster(IOType type = IOType::Input,const QString& name ="");

  ~RealRaster();

  QString getName() const override;

  virtual af::array trainingValues(int row) override;

  virtual void setTrainingValuesAsString(const QList<QString>& trainingValues) override;

  virtual af::array forecastValues(int row) override;

  virtual void setForecastValues(int row, const af::array& values, const af::array& uncertainty) override;

  virtual void setForecastValuesAsString(const QList<QString>& forecastValues) override;

  virtual void setForecastUncertaintyValueAsString(const QList<QString>& forecastUncertaintyValuesAsString) override;

  virtual void readXML(QXmlStreamReader & xmlReader) override;

  virtual int numRowsPerValue() const override;

  virtual QString type() const override;

protected:
  int m_numRowsPerValue;

private:

  void writeDataToRaster(const QString& filePath, const af::array& values);

  af::array readDataFromRaster(const QString& filePath);

  void readRasterProperties();

  void createOutputRasters();

};

class MLTOOLS_EXPORT CategoricalRaster : public CategoricalMRVMItem , public RasterItem
{

public:

  CategoricalRaster(IOType type = IOType::Input,const QString& name = "");

  ~CategoricalRaster();

  QString getName() const override;

  virtual af::array trainingValues(int row) override;

  virtual void setTrainingValuesAsString(const QList<QString>& trainingValues) override;

  virtual af::array forecastValues(int row) override;

  virtual void setForecastValues(int row, const af::array& values, const af::array& uncertainty) override;

  virtual void setForecastValuesAsString(const QList<QString>& forecastValues) override;

  virtual void setForecastUncertaintyValueAsString(const QList<QString>& forecastUncertaintyValuesAsString);

  virtual void readXML(QXmlStreamReader & xmlReader) override;

  virtual int numRowsPerValue() const override;

  virtual int columnCount() override;

  virtual QString type() const override;

private:

  void writeDataToRaster(const QString& filePathForecast, const af::array& values, const QString& filePathUncertain, const af::array& uncert);

  af::array readDataFromRaster(const QString& filePath);

  void readRasterProperties();

  void createOutputRasters();

private:

  int m_numRowsPerValue;
  int m_columnCount;

};

class MLTOOLS_EXPORT RasterBootstrap
{


public:
  RasterBootstrap();

  ~RasterBootstrap();

  int numSamplingWindows() const;

  void setNumSamplingWindows(int windowCount);

  int samplingWindowSize() const;

  void setSamplingWindowSize(int size);

  bool includeDistance() const;

  void setIncludeDistance(bool includedistance);

  QSet<RasterItem*> rasterItems() const;

  void addRasterItem(RasterItem* rasterItem);

  bool removeRasterItem(RasterItem* rasteriterm);

  QMap<QString,QList<QList<QPoint> > > sampleLocationIndexes() const;

  QMap<QString,QList<QPoint> > windowLocations() const;

  void sampleRasters();

  void setRasterItemLocations();

private:
  int m_numSampleWindows;
  int m_samplingWindowSize;
  QSet<RasterItem*> m_rasterItems;
  QList<QString> m_mrvmItems;
  QMap<QString,QList<QList<QPoint> > > m_mrvmItemLocations;
  QList<int> sampleSize;
  QMap<QString, QList<QPoint> > m_windowLocations;
  bool m_includeDistance;

};

class MLTOOLS_EXPORT Kernel
{
public:
  enum KernelType
  {
    Gaussian,
    Laplace,
    Polynomial,
    HomogeneousPolynomail,
    Spline,
    Cauchy,
    Cubic,
    Distance,
    ThinPlateSpline,
    Bubble
  };

public:
  Kernel(KernelType type = Gaussian , double lengthScale = 1000.0 );

  ~Kernel();
  
  KernelType type() const;
  
  void setKernelType(KernelType type);
  
  double lengthScale() const;
  
  void setLengthScale(double lengthScale);
  
  double polynomialPower() const;
  
  void setPolynomialPower(double polynomialPower);

  bool useBias() const;

  void setUseBias(bool m_useBias);
  
  af::array calculateKernel(const af::array& x1, const af::array& x2);

  //private:
public:

  af::array calculateGaussianKernel(const af::array& x1, const af::array& x2);

  af::array calculateLaplaceKernel(const af::array& x1, const af::array& x2);

  af::array calculatePolynomialKernel(const af::array& x1, const af::array& x2);
  
  af::array calculateHomogeneousPolynomialKernel(const af::array& x1, const af::array& x2);

  af::array calculateSplineKernel(const af::array& x1, const af::array& x2);

  af::array calculateCauchyKernel(const af::array& x1, const af::array& x2);

  af::array calculateCubicKernel(const af::array& x1, const af::array& x2);

  af::array calculateDistanceKernel(const af::array& x1, const af::array& x2);

  af::array calculateThinPlateSplineKernel(const af::array& x1, const af::array& x2);

  af::array calculateBubbleKernel(const af::array& x1, const af::array& x2);

  af::array distanceSquared(const af::array& x , const af::array& y);

private:
  KernelType m_kernelType;
  double m_lengthScale, m_polynomailPower;
  bool m_useBias;
};

class MLTOOLS_EXPORT MRVM : public QObject
{
public:
  enum Mode
  {
    TrainingAndRegression,
    Training,
    Regression
  };
  
  Q_OBJECT;
  
public:
  MRVM(const QFileInfo& file);
  
  ~MRVM();
  
  QString name() const;
  
  int maxNumberOfIterations() const;
  
  void setMaxNumberOfIterations(int niters);

  bool verbose() const;

  void setVerbose(bool verbose);

  int numberOfIterations() const ;
  
  MRVM::Mode mode() const;

  bool converged() const;

  const Kernel& kernel() const;
  
  const QMap<QString, MRVMItem*>& inputItems() const;
  
  void addInputItem(MRVMItem*& inputItem);
  
  bool removeInputItem(const QString& name);
  
  const QMap<QString, MRVMItem*>& outputItems() const;
  
  void addOutputItem(MRVMItem*& inputItem);
  
  bool removeOutputItem(const QString& name);
  
  QString matrixOutputFile() const;
  
  void setMatrixOutputFile(const QString& matrixOutputFile);
  
  const af::array& usedRelevantVectors() const;

  const af::array& alpha() const;
  
  const af::array& invSigma() const;

  const af::array& omega() const;

  const af::array& mu() const;
  
  void saveProject();

  void start();

  void performTraining();

  void mrvm();

  void fmrvm();

  void performRegression();

  static bool gdalRegistered();

#if Q_DEBUG
#else
private:
#endif

  void readProject();

  MRVMItem* readMRVMItem(MRVMItem::IOType type,  QXmlStreamReader& reader);

  void validateInputs();

  int ioValuesColumnCount(bool input = true) const;

  int ioValuesRowCount(int& maxrowsPerValue, bool input = true, bool training = true) const;

  af::array getInputMatrix(bool training = true);

  af::array getInputMatrix(int valueIndex, bool training = true);

  af::array getOutputMatrix(bool training = true);

  af::array getOutputMatrix(int valueIndex, bool training = true);

  void writeOutput(int valueIndex, const af::array& values , const af::array& uncertainty);

  QPair<int,double> calculateDeltaL(const af::array& mask, const af::array& alpha,
                                    const af::array& alphaNew, const af::array& sPrime,
                                    const af::array& qPrime, const af::array& s,
                                    const af::array& q);



  af::array corrcov(const af::array& cov);
  
private:
  QMap<QString,MRVMItem*> m_inputItems, m_outputItems;
  af::array m_inputMatrix, m_targetMatrix , m_used , m_alpha , m_invSigma, m_omega, m_Mu ;

  int m_maxNumberOfIterations = 1000 , m_numberOfIterations ;

  int m_numInputCols, m_numOutputCols, m_numInputTrainingRows, m_numOutputTrainingRows,
  m_numInputForecastRows, m_maxNumRowsPerInputValue, m_maxNumRowsPerOutputValue;

  float m_minChangeAlpha, m_maxChangeAlpha;
  QString m_matrixOutputFile;
  int N, V;
  QFileInfo m_file;
  QString m_name;
  MRVM::Mode m_mode;
  Kernel m_kernel;
  static bool s_gdalRegistered;
  bool m_converged;
  double m_tolerance = 0.01;
  int algmode = 0;
  bool m_verbose;
};



#endif // MRVM_H