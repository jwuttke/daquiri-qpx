#include <gtest/gtest.h>
#include <consumers/add_ons/status.h>

using namespace DAQuiri;

TEST(Status, Init)
{
  Status s;
  EXPECT_FALSE(s.valid);
}

TEST(Status, ExtractDefault)
{
  Spill spill;

  Status s = Status::extract(spill);

  EXPECT_TRUE(s.valid);
  EXPECT_EQ(s.type, spill.type);
  EXPECT_EQ(s.producer_time, spill.time);
  EXPECT_GE(s.consumer_time, s.producer_time);
  EXPECT_TRUE(s.stats.empty());
  EXPECT_EQ(s.timebase, spill.event_model.timebase);
}

TEST(Status, ExtractWithTimes)
{
  Spill spill;
  spill.state.branches.add_a(Setting::integer("native_time", 1000));
  spill.state.branches.add_a(Setting::integer("live_time", 500));

  Status s = Status::extract(spill);

  EXPECT_TRUE(s.valid);
  EXPECT_TRUE(s.stats.count("native_time"));
  EXPECT_EQ(s.stats["native_time"].get_int(), 1000);
  EXPECT_TRUE(s.stats.count("live_time"));
  EXPECT_EQ(s.stats["live_time"].get_int(), 500);
}

TEST(Status, CalcDiffBothIllegal)
{
  Status s, s2;
  EXPECT_EQ(Status::calc_diff(s, s2, "sometime"), hr_duration_t());
}

TEST(Status, CalcDiffOneIllegal)
{
  Spill spill;
  spill.state.branches.add_a(Setting::integer("native_time", 1000));
  spill.state.branches.add_a(Setting::integer("live_time", 500));

  Status s = Status::extract(spill);
  Status s2;
  EXPECT_EQ(Status::calc_diff(s, s2, "sometime"), hr_duration_t());
  EXPECT_EQ(Status::calc_diff(s2, s, "sometime"), hr_duration_t());
}

TEST(Status, CalcDiffPositive)
{
  Spill spill;
  spill.state.branches.add_a(Setting::precise("native_time", 10000));
  spill.state.branches.add_a(Setting::precise("live_time", 5000));
  Status s = Status::extract(spill);

  Spill spill2;
  spill2.state.branches.add_a(Setting::precise("native_time", 20000));
  spill2.state.branches.add_a(Setting::precise("live_time", 8000));
  Status s2 = Status::extract(spill2);

  EXPECT_EQ(Status::calc_diff(s, s2, "native_time"), std::chrono::microseconds(10));
  EXPECT_EQ(Status::calc_diff(s, s2, "live_time"), std::chrono::microseconds(3));
}

TEST(Status, CalcDiffNegative)
{
  Spill spill;
  spill.state.branches.add_a(Setting::precise("native_time", 10000));
  spill.state.branches.add_a(Setting::precise("live_time", 5000));
  Status s = Status::extract(spill);

  Spill spill2;
  spill2.state.branches.add_a(Setting::precise("native_time", 20000));
  spill2.state.branches.add_a(Setting::precise("live_time", 8000));
  Status s2 = Status::extract(spill2);

  EXPECT_EQ(Status::calc_diff(s2, s, "native_time"), std::chrono::microseconds(-10));
  EXPECT_EQ(Status::calc_diff(s2, s, "live_time"), std::chrono::microseconds(-3));
}

TEST(Status, TotalElapsed)
{
  std::vector<Status> stats;

  Spill spill("", Spill::Type::start);
  spill.state.branches.add_a(Setting::precise("native_time", 0));
  stats.push_back(Status::extract(spill));
  EXPECT_EQ(Status::total_elapsed(stats, "native_time"), std::chrono::microseconds(0));

  spill.type = Spill::Type::running;
  spill.state.set(Setting::precise("native_time", 2000));
  stats.push_back(Status::extract(spill));
  EXPECT_EQ(Status::total_elapsed(stats, "native_time"), std::chrono::microseconds(2));

  spill.type = Spill::Type::stop;
  spill.state.set(Setting::precise("native_time", 4000));
  stats.push_back(Status::extract(spill));
  EXPECT_EQ(Status::total_elapsed(stats, "native_time"), std::chrono::microseconds(4));

  spill.type = Spill::Type::start;
  spill.state.set(Setting::precise("native_time", 7000));
  stats.push_back(Status::extract(spill));
  EXPECT_EQ(Status::total_elapsed(stats, "native_time"), std::chrono::microseconds(4));

  spill.type = Spill::Type::running;
  spill.state.set(Setting::precise("native_time", 9000));
  stats.push_back(Status::extract(spill));
  EXPECT_EQ(Status::total_elapsed(stats, "native_time"), std::chrono::microseconds(6));
}
