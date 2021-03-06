#ifndef __FRAME_TRANSFORMER_HPP__
#define __FRAME_TRANSFORMER_HPP__

#include <map>
#include <vector>
#include <string>

#include <opencv2/core/core.hpp>

#include "converters.hpp"
#include "Timer.hpp"

typedef std::pair<std::size_t, double> PointNumberAndError;

typedef std::pair<std::size_t, std::size_t> Path;
typedef std::pair<Path, double> PathNumberAndError;

typedef std::map<std::string, std::string> Dictionary;

class FrameTransformer {
  public:
    FrameTransformer();
    virtual ~FrameTransformer();

    virtual void process(cv::Mat& frame) = 0;

    virtual void beforeFrame(cv::Mat& frame);
    virtual void afterFrame(cv::Mat& frame);

    virtual Dictionary getResults() const;

    virtual void handleFirstFrame(const cv::Mat& frame);
    virtual void handleMovieName(const std::string& movieName);

    virtual void afterInitialization() {};

    void collectAndDrawAverageTrack(
           const std::vector<cv::Point2f>& points,
           std::size_t meaningfulAmount,
           cv::Mat& frame);

    void collectAndDrawAverageTrack(
           const std::vector<cv::Point2d>& points,
           std::size_t meaningfulAmount,
           cv::Mat& frame);

    bool contains(const cv::Point2f& point) const;
    bool contains(const cv::Point2d& point) const;
    bool contains(const cv::Point& point) const;

  protected:
    common::Timer memoryMeasureOverheadTimer;
    common::Timer processingFrameTimer;
    common::Timer averagePointsCalculationTimer;

    double virtualMemoryAtStart;
    double residentSetAtStart;

    std::vector<double> memoryMeasureOverhead;
    std::vector<double> processedFramesTiming;

    std::vector<double> processedFramesVirtualMemoryUsage;
    std::vector<double> processedFramesResidentMemoryUsage;

    std::vector<double> averagePointsCalculationOverhead;

    std::vector<cv::Point2f> averagePoints;
    std::vector<cv::Point2d> doubleAveragePoints;

    cv::Size boundary;
    cv::Rect* boundingRectangle;

    double keypointRadius;
    std::vector<cv::Point> keypoints;
    std::vector<cv::Point> baseKeypoints;

    mutable std::vector<PointNumberAndError> collectedErrors;
    mutable std::vector<PathNumberAndError> collectedPathErrors;

  private:
    void calculateBoundingRectangle(const std::string& movieName);
    void readKeypointsFromFile(
          const std::string& fileName,
          std::vector<cv::Point>& points,
          std::vector<cv::Point>& baseKeypoints);
};

class ArgumentsAwareFrameTransformer : public FrameTransformer {
  public:
    virtual ~ArgumentsAwareFrameTransformer() {}
    virtual void fill(const std::vector<std::string>& arguments) = 0;
};

#endif