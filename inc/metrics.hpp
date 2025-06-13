#pragma once

#include <atomic>
#include <chrono>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <memory>
#include <mutex>
#include <sstream>
#include <thread>
#include <vector>

class Metric {
protected:
  std::string name_;

public:
  Metric(const std::string &name);
  virtual void update(double newValue) = 0;
  virtual double getValue() const = 0;
  virtual void reset() = 0;
  std::string getName() const;
  virtual ~Metric() = default;
};

class CPUMetric final : public Metric {
private:
  std::atomic<double> value_;

public:
  CPUMetric(const std::string &name);
  void update(double newValue) override;
  double getValue() const override;
  void reset() override;
};

class HTTPMetric final : public Metric {
  std::atomic<int> value_;

public:
  HTTPMetric(const std::string &name);
  void update(double newValue) override;
  double getValue() const override;
  void reset() override;
};

class MetricsLogger {
private:
  using pMetric = std::shared_ptr<Metric>;

private:
  std::string filename_;
  std::ofstream ofs_;
  std::vector<pMetric> metrics_;
  std::mutex mutex_;
  std::chrono::milliseconds interval_;
  std::thread loggerThread_;
  bool done_;

private:
  void run();

public:
  MetricsLogger(const std::string &filename,
                std::chrono::milliseconds interval);

  ~MetricsLogger();

  void addMetric(const pMetric &metric);
};
