#include "AoC_2023.h"
#include "AoC_StreamBuff.h"
#include "AoC_Riddles.h"
#include "Test_Space.h"

#include <QMessagebox>
#include <QDebug>
#include <QPushbutton>
#include <iostream>
#include <string>
#include <tuple>
#include <filesystem>
#include <functional>
#include <format>
#include <ranges>

using namespace std::string_literals;
using namespace std::placeholders;

namespace fs = std::filesystem;

using structure_data = std::tuple<std::string, int, int, int, std::function<void (int, my_lines, bool)>>;

const std::string        strAoC{ "Advent of Code 2023" };
const std::array<int, 5> cols = { 50, 220, 400, 580, 750 };
const std::array<int, 5> rows = { 30, 150, 270, 390, 510 };
const int iWidth  = 161;
const int iHeight = 111;

void help(std::string const& strText, int part, my_lines file, bool) {
   std::cout << std::format("{} / Part {}\n{}\n", strText, part, file.theText);
   std::cout << "Unfortunately, there is no solution for this puzzle yet.\n"
             << "Perhaps you opened the door too early, and we haven't reached the day yet.\n";
   //QMessageBox::information(nullptr, strAoC.c_str(), QString::fromStdString(strMessage));
   }

std::vector<structure_data> advent_actions = {
   { structure_data{ "btnDay01"s, 0, 0,  1, aoc2023_day1::Riddle  } },
   { structure_data{ "btnDay02"s, 0, 1,  2, aoc2023_day2::Riddle  } },
   { structure_data{ "btnDay03"s, 0, 2,  3, aoc2023_day3::Riddle  } },
   { structure_data{ "btnDay04"s, 0, 3,  4, aoc2023_day4::Riddle  } },
   { structure_data{ "btnDay05"s, 0, 4,  5, aoc2023_day5::Riddle  } },
   { structure_data{ "btnDay06"s, 1, 0,  6, aoc2023_day6::Riddle  } },
   { structure_data{ "btnDay07"s, 1, 1,  7, aoc2023_day7::Riddle  } },
   { structure_data{ "btnDay08"s, 1, 2,  8, aoc2023_day8::Riddle  } },
   { structure_data{ "btnDay09"s, 1, 3,  9, aoc2023_day9::Riddle  } },
   { structure_data{ "btnDay10"s, 1, 4, 10, aoc2023_day10::Riddle } },
   { structure_data{ "btnDay11"s, 2, 0, 11, aoc2023_day11::Riddle } },
   { structure_data{ "btnDay12"s, 2, 1, 12, aoc2023_day12::Riddle } },
   { structure_data{ "btnDay13"s, 2, 2, 13, aoc2023_day13::Riddle } },
   { structure_data{ "btnDay14"s, 2, 3, 14, bind(help, "Day 14", _1, _2, _3)  } },
   { structure_data{ "btnDay15"s, 2, 4, 15, aoc2023_day15::Riddle } },
   { structure_data{ "btnDay16"s, 3, 0, 16, aoc2023_day16::Riddle } },
   { structure_data{ "btnDay17"s, 3, 1, 17, bind(help, "Day 17", _1, _2, _3)  } },
   { structure_data{ "btnDay18"s, 3, 2, 18, bind(help, "Day 18", _1, _2, _3)  } },
   { structure_data{ "btnDay19"s, 3, 3, 19, bind(help, "Day 19", _1, _2, _3)  } },
   { structure_data{ "btnDay20"s, 3, 4, 20, bind(help, "Day 20", _1, _2, _3)  } },
   { structure_data{ "btnDay21"s, 4, 0, 21, bind(help, "Day 21", _1, _2, _3)  } },
   { structure_data{ "btnDay22"s, 4, 1, 22, bind(help, "Day 22", _1, _2, _3)  } },
   { structure_data{ "btnDay23"s, 4, 2, 23, bind(help, "Day 23", _1, _2, _3)  } },
   { structure_data{ "btnDay24"s, 4, 3, 24, bind(help, "Day 24", _1, _2, _3)  } },
   { structure_data{ "btnDay25"s, 4, 4, 25, bind(help, "Day 25", _1, _2, _3)  } }
};


TStreamWrapper old_cout(std::cout), old_cerr(std::cerr), old_clog(std::clog);


void AoC_2023::CheckDay(int day) {
   if (day < 1 && day > 25)
      throw std::invalid_argument(std::format("argument part for CallRiddle is wrong, value was {}, "
                                              "expected values between 1 and 25", day));
   }


fs::path BuildInput(int day, bool test, std::string const& strApplication) {
   // build the input file name from the day and part, selectively for test or production data
   std::string strInput = std::format("../../../Input/{}/day{}.txt", (test ? "Test" : "Prod"), day);
   fs::path executableDir = fs::path(strApplication).parent_path();
   return fs::canonical(executableDir / fs::path(strInput));
   }

void AoC_2023::ShowData(int day) {
   ui.memOutput->clear();
   ui.memError->clear();
   CheckDay(day);
   bool test = ui.chkUseTestData->isChecked();
   auto file_path = BuildInput(day, test, strApplication);
   std::string strBuffer;
   for(auto const& line : my_lines(GetContent(file_path, strBuffer))) {
      std::cout << line << '\n';
      }
  }

void AoC_2023::CallRiddle(int day, std::function<void(int, my_lines, bool)> func) {
   try {
      // clear the user interface und reset it
      ui.memOutput->clear();
      ui.memError->clear();
      disconnect(ui.btnAction, &QPushButton::clicked, 0, 0);
      disconnect(ui.btnShow,   &QPushButton::clicked, 0, 0);

      ui.tabContrl->setCurrentIndex(1);

      // read informations from the user interface, set action to the button
      bool test    = ui.chkUseTestData->isChecked();
      int part     = ui.chkSecondPart->isChecked() ? 2 : 1;
      bool verbose = ui.chkVerbose->isChecked();

      CheckDay(day);
      std::string strDay;
      switch(day) {
         case 1:  strDay = std::format("{}st day", day); break;
         case 2:  strDay = std::format("{}nd day", day); break;
         case 3:  strDay = std::format("{}rd day", day); break;
         default: strDay = std::format("{}th day", day); break;
         }
      ui.btnAction->setText(QString::fromStdString(strDay));

      ui.btnAction->setEnabled(true);
      ui.btnShow->setEnabled(true);
      connect(ui.btnAction, &QPushButton::clicked, this, std::bind(&AoC_2023::CallRiddle, this, day, func));
      connect(ui.btnShow,   &QPushButton::clicked, this, std::bind(&AoC_2023::ShowData, this, day));

      auto file_path = BuildInput(day, test, strApplication);

      // write a log message
      std::clog << strDay << " / " << part << ". part of " << strThemes[0] << " with input file \"" << file_path.string() << '\n';

      // write a message with the theme and day to cout
      std::cout << strThemes[0];
      if (day < strThemes.size()) std::cout << " - " << strThemes[day] << "\n";
      else std::cout << "\n";

      // call function and read associated inputfile in a my_lines container
      std::string strBuffer;
      QCoreApplication::processEvents();
      auto func_start = std::chrono::high_resolution_clock::now();
      func(part, my_lines(GetContent(file_path, strBuffer)), verbose);
      auto func_ende = std::chrono::high_resolution_clock::now();
      auto time = std::chrono::duration_cast<std::chrono::milliseconds>(func_ende - func_start);
      std::clog << "Finished: " << strDay << " / " << part << ". part of " << strThemes[0];
      if (day < strThemes.size()) std::clog << " (" << strThemes[day] << ")";
      std::clog << " with input file \"" << file_path.string()
                << " in " << time.count() / 1000.0 << " sec\n";

      }
   catch(std::exception& ex) {
      std::cerr << "error in Caller, message: " << ex.what() << '\n';
      }
   }

AoC_2023::AoC_2023(std::string const& strAppl, QWidget *parent) : QMainWindow(parent) {
   strApplication = strAppl;
   ui.setupUi(this);
   output = ui.memError;
   ui.statusBar->setLayoutDirection(Qt::LayoutDirection::RightToLeft);
   statusLabel = new QLabel(this);
   statusLabel->setObjectName("sbMain");
   statusLabel->setText("sbMain");
   ui.statusBar->addPermanentWidget(statusLabel);

   ui.tabContrl->setTabText(0, "Advent Calendar");
   ui.tabContrl->setTabText(1, "daily Riddle");
   ui.tabContrl->setCurrentIndex(0);

   //ui.tabSolution->setSizeIncrement(QSize(1, 1));
   
   ui.chkUseTestData->setChecked(true);
   ui.chkSecondPart->setChecked(false);
   ui.btnAction->setEnabled(false);
   ui.btnAction->setText("");
   ui.btnShow->setEnabled(false);
   
   for (auto const& data : advent_actions) {
      QPushButton* btn = new QPushButton(QString::fromStdString(std::get<0>(data)), ui.lblCalendar); // centralWidget());
       btn->setText("");
       btn->setGeometry(cols[std::get<2>(data)], rows[std::get<1>(data)], iWidth, iHeight);
       QPalette palette = btn->palette();
       palette.setColor(QPalette::Button, Qt::transparent);
       btn->setPalette(palette);
       btn->setFlat(true);
       btn->setAutoFillBackground(true);
       btn->setStyleSheet("border: none;");
       btn->show();
       connect(btn, &QPushButton::clicked, this, std::bind(&AoC_2023::CallRiddle, this, std::get<3>(data), std::get<4>(data)));
       }

   connect(ui.btnTest, &QPushButton::clicked, this, [this]() { Test(); });
   connect(ui.btnClose, &QPushButton::clicked, this, [this]() { this->close(); });
   connect(ui.btnCalendar, &QPushButton::clicked, this, [this]() { this->ui.tabContrl->setCurrentIndex(0);
                                                                   this->ui.memOutput->clear();
                                                                   this->ui.memError->clear();
                                                                   this->ui.btnAction->setEnabled(false);
                                                                   this->ui.btnAction->setText("");
                                                                   this->ui.chkUseTestData->setChecked(true);
                                                                   this->ui.chkSecondPart->setChecked(false);
                                                                 });
   old_cout.Activate(ui.memOutput);
   old_cerr.Activate(ui.memError);
   old_clog.Activate(statusLabel);
   std::clog << "choose a day in calendar to solve the riddle." << std::endl;
   }

AoC_2023::~AoC_2023() {
   old_cout.Reset();
   old_cerr.Reset();
   }
