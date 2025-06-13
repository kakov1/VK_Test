#include "metrics.hpp"

Metric::Metric(const std::string &name) : name_(name) {}

std::string Metric::getName() const { return name_; }

//---------------------------------------------------------------------

CPUMetric::CPUMetric(const std::string &name) : Metric(name), value_(0.0) {}

void CPUMetric::update(double newValue) { value_ = newValue; }

double CPUMetric::getValue() const { return value_.load(); }

void CPUMetric::reset() { value_ = 0; }

//---------------------------------------------------------------------

HTTPMetric::HTTPMetric(const std::string &name) : Metric(name), value_(0) {}

void HTTPMetric::update(double newValue) { value_ = static_cast<int>(newValue); }

double HTTPMetric::getValue() const { return value_.load(); }

void HTTPMetric::reset() { value_ = 0; }

//---------------------------------------------------------------------

MetricsLogger::MetricsLogger(const std::string &filename,
                             std::chrono::milliseconds interval)
    : filename_(filename), interval_(interval), done_(false) {
  ofs_.open(filename_, std::ios::out | std::ios::app);
  loggerThread_ = std::thread(&MetricsLogger::run, this);
}

MetricsLogger::~MetricsLogger() {
  done_ = true;
  if (loggerThread_.joinable())
    loggerThread_.join();
  ofs_.close();
}

void MetricsLogger::addMetric(const std::shared_ptr<Metric> &metric) {
  std::lock_guard<std::mutex> lock(mutex_);
  metrics_.push_back(metric);
}

void MetricsLogger::run() {
  while (!done_) {
    std::this_thread::sleep_for(interval_);

    auto now = std::chrono::system_clock::now();
    std::time_t in_time = std::chrono::system_clock::to_time_t(now);
    std::tm *tinfo = std::localtime(&in_time);
    std::ostringstream ss;
    ss << std::put_time(tinfo, "%Y-%m-%d %X");

    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                  now.time_since_epoch())
                  .count() %
              1000;
    ss << '.' << std::setw(3) << std::setfill('0') << ms;

    {
      std::lock_guard<std::mutex> lock(mutex_);
      for (auto &&m : metrics_) {
        ss << ' ' << '"' << m->getName() << '"' << ' ' << m->getValue();
        m->reset();
      }
    }

    ofs_ << ss.str() << std::endl;
  }
}
