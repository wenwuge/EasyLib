// Copyright (c) 2011 The LevelDB Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#ifndef STORAGE_LEVELDB_UTIL_HISTOGRAM_H_
#define STORAGE_LEVELDB_UTIL_HISTOGRAM_H_

#include <string>
#include "ldd/util/slice.h"
#include "testutil.h"
#include <gflags/gflags.h>


using ::ldd::util::Slice;

void AppendWithSpace(std::string* str, Slice msg);



DECLARE_bool(histogram);
DECLARE_double(compression_ratio);

class Histogram {
public:
    Histogram() { }
    ~Histogram() { }

    void Clear();
    void Add(double value);
    void Merge(const Histogram& other);

    std::string ToString() const;

private:
    double min_;
    double max_;
    double num_;
    double sum_;
    double sum_squares_;

    enum { kNumBuckets = 154 };
    static const double kBucketLimit[kNumBuckets];
    double buckets_[kNumBuckets];

    double Median() const;
    double Percentile(double p) const;
    double Average() const;
    double StandardDeviation() const;
};



class Stats {
private:
    double start_;
    double finish_;
    double seconds_;
    int done_;
    int next_report_;
    int64_t bytes_;
    double last_op_finish_;
    Histogram hist_;
    std::string message_;

public:
    Stats() { Start(); }

    void Start() {
        next_report_ = 100;
        last_op_finish_ = start_;
        hist_.Clear();
        done_ = 0;
        bytes_ = 0;
        seconds_ = 0;
        start_ = NowMicros();
        finish_ = start_;
        message_.clear();
    }

    void Merge(const Stats& other) {
        hist_.Merge(other.hist_);
        done_ += other.done_;
        bytes_ += other.bytes_;
        //seconds_ += other.seconds_;
        if (other.start_ < start_) start_ = other.start_;
        if (other.finish_ > finish_) finish_ = other.finish_;

        // Just keep the messages from one thread
        if (message_.empty()) message_ = other.message_;
    }

    void Stop() {
        finish_ = NowMicros();
        seconds_ = (finish_ - start_) * 1e-6;
    }

    void AddMessage(Slice msg) {
        AppendWithSpace(&message_, msg);
    }

    void FinishedSingleOp() {
        if (FLAGS_histogram) {
            double now = NowMicros();
            double micros = now - last_op_finish_;
            hist_.Add(micros);
            if (micros > 20000) {
                fprintf(stderr, "long op: %.1f micros%30s\r", micros, "");
                fflush(stderr);
            }
            last_op_finish_ = now;
        }

        done_++;
        if (done_ >= next_report_) {
            if      (next_report_ < 1000)   next_report_ += 100;
            else if (next_report_ < 5000)   next_report_ += 500;
            else if (next_report_ < 10000)  next_report_ += 1000;
            else if (next_report_ < 50000)  next_report_ += 5000;
            else if (next_report_ < 100000) next_report_ += 10000;
            else if (next_report_ < 500000) next_report_ += 50000;
            else                            next_report_ += 100000;
            fprintf(stderr, "... finished %d ops%30s\r", done_, "");
            fflush(stderr);
        }
    }

    void AddBytes(int64_t n) {
        bytes_ += n;
    }

    void Report(const Slice& name) {
        // Pretend at least one op was done in case we are running a benchmark
        // that does not call FinishedSingleOp().
        if (done_ < 1) done_ = 1;

        std::string extra;
        if (bytes_ > 0) {
            // Rate is computed on actual elapsed time, not the sum of per-thread
            // elapsed times.
            double elapsed = (finish_ - start_) * 1e-6;
            char rate[100];
            snprintf(rate, sizeof(rate), "%6.1f MB/s",
                (bytes_ / 1048576.0) / elapsed);
            extra = rate;
        }
        AppendWithSpace(&extra, message_);

        fprintf(stdout, "%-12s : %11.3f micros/op; %11.3f op/second; %s%s\n",
            name.ToString().c_str(),
            seconds_ * 1e6 / done_, (done_*1e6)/(seconds_ * 1e6),
            (extra.empty() ? "" : " "),
            extra.c_str());
        if (FLAGS_histogram) {
            fprintf(stdout, "Microseconds per op:\n%s\n", hist_.ToString().c_str());
        }
        fflush(stdout);
    }
};


// Helper for quickly generating random data.
class RandomGenerator {
private:
    std::string data_;
    int pos_;

public:
    RandomGenerator() {
        // We use a limited amount of data over and over again and ensure
        // that it is larger than the compression window (32KB), and also
        // large enough to serve all typical value sizes we want to write.
        Random rnd(301);
        std::string piece;
        while (data_.size() < 1048576) {
            // Add a short fragment that is as compressible as specified
            // by FLAGS_compression_ratio.
            CompressibleString(&rnd, FLAGS_compression_ratio, 100, &piece);
            data_.append(piece);
        }
        pos_ = 0;
    }

    Slice Generate(int len) {
        if ((size_t)pos_ + len > data_.size()) {
            pos_ = 0;
            assert((size_t)len < data_.size());
        }
        pos_ += len;
        return Slice(data_.data() + pos_ - len, len);
    }
};


#endif  // STORAGE_LEVELDB_UTIL_HISTOGRAM_H_
