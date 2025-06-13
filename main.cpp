#include "metrics.hpp"

const std::string cpuMetricName = "CPU";
const std::string HTTPMetricName = "HTTP requests RPS";

void validateInput() {
  if (!std::cin.good())
    throw std::runtime_error("Input error.");
}

int main() {
  MetricsLogger logger(LOG_PATH + std::string("/") + "metrics.log",
                       std::chrono::milliseconds(500));

  auto cpuMetric = std::make_shared<CPUMetric>(cpuMetricName);
  auto httpMetric = std::make_shared<HTTPMetric>(HTTPMetricName);

  std::vector<double> cpuMetricValues;
  std::vector<int> httpMetricValues;

  std::string metricName;
  double metricValue;

  while (std::cin >> metricName) {
    validateInput();

    std::cin >> metricValue;

    validateInput();

    if (metricName == cpuMetricName)
      cpuMetricValues.emplace_back(metricValue);

    else
      httpMetricValues.emplace_back(static_cast<int>(metricValue));
  }

  logger.addMetric(cpuMetric);
  logger.addMetric(httpMetric);

  std::thread t1([&]() {
    for (auto &&value : cpuMetricValues) {
      cpuMetric->update(value);
      std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
  });

  std::thread t2([&]() {
    for (auto &&value : httpMetricValues) {
      httpMetric->update(value);
      std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
  });

  t1.join();
  t2.join();

  std::this_thread::sleep_for(std::chrono::seconds(1));
  return 0;
}
