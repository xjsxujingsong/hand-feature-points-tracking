#include <opencv2/core/core.hpp>
#include <opencv2/core/core_c.h>

#include "memory-usage.hpp"
#include "FrameTransformers.hpp"

// Helpers.
template<typename T>
  void drawAverageTrack(const std::vector<T>& points, cv::Mat& frame) {
    const cv::Scalar DrawingColor = cv::Scalar(255, 255, 255);
    const double CircleRadius = 3.0;

    for (unsigned int i = 0; i < points.size(); ++i) {
      if (i > 0) {
        cv::line(frame, points[i - 1], points[i], DrawingColor);
      }

      cv::circle(frame, points[i], CircleRadius, DrawingColor, -1);
    }
  }

template<typename T, typename R>
  void calculateMean(const std::vector<T>& input, std::size_t meaningfulAmount, std::vector<T>& output) {
    if (input.size() > meaningfulAmount) {
      cv::Mat reducedMean;
      cv::reduce(input, reducedMean, CV_REDUCE_AVG, 1);

      if (!reducedMean.empty()) {
        T mean(reducedMean.at<R>(0,0), reducedMean.at<R>(0,1));
        output.push_back(mean);
      }
    }
  }

// Constructor.
FrameTransformer::FrameTransformer() {
  common::getMemoryUsage(virtualMemoryAtStart, residentSetAtStart);
}

// Public methods.
void FrameTransformer::collectAndDrawAverageTrack(
                          const std::vector<cv::Point2f>& points,
                          std::size_t meaningfulAmount,
                          cv::Mat& frame) {
  averagePointsCalculationTimer.start();

  calculateMean<cv::Point2f, float>(points, meaningfulAmount, averagePoints);
  drawAverageTrack<cv::Point2f>(averagePoints, frame);

  averagePointsCalculationTimer.stop();

  averagePointsCalculationOverhead.push_back(averagePointsCalculationTimer.getElapsedTimeInMilliseconds());
}

void FrameTransformer::collectAndDrawAverageTrack(
                          const std::vector<cv::Point2d>& points,
                          std::size_t meaningfulAmount,
                          cv::Mat& frame) {
  averagePointsCalculationTimer.start();

  calculateMean<cv::Point2d, double>(points, meaningfulAmount, doubleAveragePoints);
  drawAverageTrack<cv::Point2d>(doubleAveragePoints, frame);

  averagePointsCalculationTimer.stop();

  averagePointsCalculationOverhead.push_back(averagePointsCalculationTimer.getElapsedTimeInMilliseconds());
}

void FrameTransformer::beforeFrame(cv::Mat& frame) {
  processingFrameTimer.start();
}

void FrameTransformer::afterFrame(cv::Mat& frame) {
  processingFrameTimer.stop();
  processedFramesTiming.push_back(processingFrameTimer.getElapsedTimeInMilliseconds());

  memoryMeasureOverheadTimer.start();

  double virtualMemory, residentSet;
  common::getMemoryUsage(virtualMemory, residentSet);

  processedFramesVirtualMemoryUsage.push_back(virtualMemory - virtualMemoryAtStart);
  processedFramesResidentMemoryUsage.push_back(residentSet - residentSetAtStart);

  memoryMeasureOverheadTimer.stop();

  memoryMeasureOverhead.push_back(memoryMeasureOverheadTimer.getElapsedTimeInMilliseconds());
}

Dictionary FrameTransformer::getResults() const {
  Dictionary results;

  results.insert(std::make_pair("memoryMeasureOverheadTime", common::toString(memoryMeasureOverhead)));
  results.insert(std::make_pair("timesForProcessingEachFrame", common::toString(processedFramesTiming)));

  results.insert(std::make_pair("virtualMemoryUsage", common::toString(processedFramesVirtualMemoryUsage)));
  results.insert(std::make_pair("residentSetMemoryUsage", common::toString(processedFramesResidentMemoryUsage)));

  results.insert(std::make_pair("averagePointsCalculationOverheadTime",
                                common::toString(averagePointsCalculationOverhead)));

  if (averagePoints.size() > 0) {
    results.insert(std::make_pair("averagePoints", common::toString(averagePoints)));
  } else if (doubleAveragePoints.size() > 0) {
    results.insert(std::make_pair("averagePoints", common::toString(doubleAveragePoints)));
  }

  return results;
}