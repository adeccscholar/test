#pragma once

#include "AoC_lib.h"

#include <QtWidgets/QMainWindow>
#include <QString>
#include <string>
#include <functional>

#include "ui_AoC_2023.h"

class AoC_2023 : public QMainWindow
{
    Q_OBJECT

public:
    AoC_2023(std::string const& strAppl, QWidget *parent = nullptr);
    ~AoC_2023();

    static inline QTextEdit* output;

private:
   std::string strApplication;
    Ui::AoC_2023Class ui;
    QLabel* statusLabel;

    void ShowData(int day);
    void CallRiddle(int day, std::function<void(int, my_lines, bool)> func);

    void CheckDay(int day);
};
