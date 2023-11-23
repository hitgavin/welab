#include "core/logger/logger.h"
#include "core/core.h"
#include "core/logger/logger_settings.h"

#include <gtest/gtest.h>

TEST(LoggerSingleton, LoggerCreation) {
  EXPECT_TRUE(core::logger::Logger::instance() != nullptr);
  EXPECT_EQ(core::logger::Logger::instance(), logger());
}

TEST(LoggerSingleton, LoggerInit) {
  EXPECT_FALSE(logger()->is_init());
  auto settings = logger()->logger_settings();
  settings.sink_type_ = core::logger::CONSOLE | core::logger::FILE;
  settings.file_logger_settings_.file_path_ = std::string("logger.txt");
  settings.file_logger_settings_.max_file_size_mb_ = 50.0;
  settings.file_logger_settings_.max_files_ = 3;
  logger()->set_logger_settings(settings);
  EXPECT_TRUE(logger()->init());
  EXPECT_TRUE(logger()->is_init());
  logger()->reset_logger();
  EXPECT_FALSE(logger()->is_init());
}

TEST(TestConsoleLogger, NormalString) {
  auto settings = logger()->logger_settings();
  settings.sink_type_ = core::logger::CONSOLE | core::logger::FILE;
  settings.file_logger_settings_.file_path_ = std::string("logger.txt");
  settings.file_logger_settings_.max_file_size_mb_ = 50.0;
  settings.file_logger_settings_.max_files_ = 3;
  logger()->set_logger_settings(settings);
  logger()->init();
  logger()->info("test ok");
  logger()->info("test: {}, {}, {}", 1, 2.35, "testtt");
  logger()->reset_logger();
  EXPECT_FALSE(logger()->is_init());
}

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}