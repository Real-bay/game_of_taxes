// The main program (provided by the course), TIE-20100/TIE-20106
//
// DO ****NOT**** EDIT THIS FILE!
// (Preferably do not edit this even temporarily. And if you still decide to do so
//  (for debugging, for example), DO NOT commit any changes to git, but revert all
//  changes later. Otherwise you won't be able to get any updates/fixes to this
//  file from git!)

#include "mainprogram.hh"

// Qt generated main window code

#ifdef GRAPHICAL_GUI
#include <QCoreApplication>
#include <QFileDialog>
#include <QDir>
#include <QFont>
#include <QGraphicsItem>
#include <QColor>
#include <QBrush>
#include <QPen>
#include <QGraphicsItem>
#include <QVariant>

#include <string>
using std::string;

#include <sstream>
using std::ostringstream;

#include <iostream>

#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <utility>
#include <tuple>

#include <cassert>

#include "mainwindow.hh"
#include "ui_mainwindow.h"

// Needed to be able to store TownID in QVariant (in QGraphicsItem)
Q_DECLARE_METATYPE(TownID)
// The same for AreaIDs (currently an int)
//struct AreaIDcont { AreaID id; }; // So that we have a different type than for Place IDs
//Q_DECLARE_METATYPE(AreaIDcont)
// The same for Coords (currently a pair of ints)
Q_DECLARE_METATYPE(Coord)

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    mainprg_.setui(this);

    // Execute line
    connect(ui->execute_button, &QPushButton::pressed, this, &MainWindow::execute_line);

    // Line edit
    connect(ui->lineEdit, &QLineEdit::returnPressed, this, &MainWindow::execute_line);

    // File selection
    connect(ui->file_button, &QPushButton::pressed, this, &MainWindow::select_file);

    // Command selection
    // !!!!! Sort commands in alphabetical order (should not be done here, but is)
    std::sort(mainprg_.cmds_.begin(), mainprg_.cmds_.end(), [](auto const& l, auto const& r){ return l.cmd < r.cmd; });
    for (auto& cmd : mainprg_.cmds_)
    {
        ui->cmd_select->addItem(QString::fromStdString(cmd.cmd));
    }
    connect(ui->cmd_select, static_cast<void(QComboBox::*)(int)>(&QComboBox::activated), this, &MainWindow::cmd_selected);

    // Number selection
    for (auto i = 0; i <= 20; ++i)
    {
        ui->number_select->addItem(QString("%1").arg(i));
    }
    connect(ui->number_select, static_cast<void(QComboBox::*)(QString const&)>(&QComboBox::currentTextChanged), this, &MainWindow::number_selected);

    // Output box
    QFont monofont = QFontDatabase::systemFont(QFontDatabase::FixedFont);
    monofont.setStyleHint(QFont::TypeWriter);
    ui->output->setFont(monofont);

    // Initialize graphics scene & view
    gscene_ = new QGraphicsScene(this);
    ui->graphics_view->setScene(gscene_);
    ui->graphics_view->resetTransform();
    ui->graphics_view->setBackgroundBrush(Qt::black);

    // Selecting graphics items by mouse
    connect(gscene_, &QGraphicsScene::selectionChanged, this, &MainWindow::scene_selection_change);
//    connect(this, &MainProgram::signal_clear_selection, this, &MainProgram::clear_selection);

    // Zoom slider changes graphics view scale
    connect(ui->zoom_plus, &QToolButton::clicked, [this]{ this->ui->graphics_view->scale(1.1, 1.1); });
    connect(ui->zoom_minus, &QToolButton::clicked, [this]{ this->ui->graphics_view->scale(1/1.1, 1/1.1); });
    connect(ui->zoom_1, &QToolButton::clicked, [this]{ this->ui->graphics_view->resetTransform(); });
    connect(ui->zoom_fit, &QToolButton::clicked, this, &MainWindow::fit_view);

    // Changing checkboxes updates view
    connect(ui->roads_checkbox, &QCheckBox::clicked, this, &MainWindow::update_view);
    connect(ui->towns_checkbox, &QCheckBox::clicked, this, &MainWindow::update_view);
    connect(ui->townnames_checkbox, &QCheckBox::clicked, this, &MainWindow::update_view);
    connect(ui->vassals_checkbox, &QCheckBox::clicked, this, &MainWindow::update_view);
//    connect(ui->regionnames_checkbox, &QCheckBox::clicked, this, &MainWindow::update_view);

    // Unchecking towns checkbox disables town names checkbox
    connect(ui->towns_checkbox, &QCheckBox::clicked,
            [this]{ this->ui->townnames_checkbox->setEnabled(this->ui->towns_checkbox->isChecked()); });

    // Unchecking regions checkbox disables region names checkbox
//    connect(ui->regions_checkbox, &QCheckBox::clicked,
//            [this]{ this->ui->regionnames_checkbox->setEnabled(this->ui->regions_checkbox->isChecked()); });

    // Changing font or points scale updates view
    connect(ui->fontscale, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &MainWindow::update_view);
    connect(ui->pointscale, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &MainWindow::update_view);

    // Clear input button
    connect(ui->clear_input_button, &QPushButton::clicked, this, &MainWindow::clear_input_line);

    // Stop button
    connect(ui->stop_button, &QPushButton::clicked, [this](){ this->stop_pressed_ = true; });

    clear_input_line();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::update_view()
{
    std::unordered_set<std::string> errorset;
    try
    {
        gscene_->clear();
        auto pointscale = ui->pointscale->value();
        auto fontscale = ui->fontscale->value();

        std::unordered_map<TownID, std::string> result_towns;
        auto& prev_result = mainprg_.prev_result.second;
        // Copy the stop id vector to the result set
        int i = 0;
        std::for_each(prev_result.begin(), prev_result.end(),
                      [&result_towns, &i](auto id){ result_towns[id] += MainProgram::convert_to_string(++i)+". "; });

        auto towns = mainprg_.ds_.all_towns();
        if (towns.size() == 1 && towns.front() == NO_TOWNID)
        {
            errorset.insert("all_towns() returned error {NO_TOWNID}");
        }

        for (auto& townid : towns)
        {
            auto res_place = std::find(prev_result.begin(), prev_result.end(), townid);

            QColor towncolor = Qt::white;
            QColor namecolor = Qt::cyan;
            QColor townborder = Qt::white;
            int townzvalue = 1;

            try
            {
                auto xy = mainprg_.ds_.get_town_coordinates(townid);
                auto [x,y] = xy;
                if (x == NO_VALUE || y == NO_VALUE)
                {
                    errorset.insert("get_coordinates() returned error NO_COORD/NO_VALUE");
                }

                if (x == NO_VALUE || y == NO_VALUE)
                {
                    x = 0; y = 0;
                    towncolor = Qt::magenta;
                    namecolor = Qt::magenta;
                    townzvalue = 30;
                }

                string prefix;
                auto res_town = result_towns.find(townid);
                if (res_town != result_towns.end())
                {
                    if (result_towns.size() > 1) { prefix = res_town->second; }
                    namecolor = Qt::red;
                    townborder = Qt::red;
                    townzvalue = 2;
                }

                if (ui->towns_checkbox->isChecked())
                {
                    auto groupitem = gscene_->createItemGroup({});
                    groupitem->setFlag(QGraphicsItem::ItemIsSelectable);
                    groupitem->setData(0, QVariant::fromValue(townid));

                    QPen placepen(townborder);
                    placepen.setWidth(0); // Cosmetic pen
                    auto dotitem = gscene_->addEllipse(-4*pointscale, -4*pointscale, 8*pointscale, 8*pointscale,
                                                       placepen, QBrush(towncolor));
                    dotitem->setFlag(QGraphicsItem::ItemIgnoresTransformations);
                    groupitem->addToGroup(dotitem);
                    //        dotitem->setFlag(QGraphicsItem::ItemIgnoresTransformations);
                    //        dotitem->setData(0, QVariant::fromValue(town));

                    // Draw town names
                    string label = prefix;
                    if (ui->townnames_checkbox->isChecked())
                    {
                        try
                        {
                            auto name = mainprg_.ds_.get_town_name(townid);
                            if (name == NO_NAME)
                            {
                                errorset.insert("get_town_name() returned error NO_NAME");
                            }

                            label += name;
                        }
                        catch (NotImplemented const& e)
                        {
                            errorset.insert(std::string("NotImplemented while updating graphics: ") + e.what());
                            std::cerr << std::endl << "NotImplemented while updating graphics: " << e.what() << std::endl;
                        }
                    }

                    if (!label.empty())
                    {
                        // Create extra item group to be able to set ItemIgnoresTransformations on the correct level (addSimpleText does not allow
                        // setting initial coordinates in item coordinates
                        auto textgroupitem = gscene_->createItemGroup({});
                        auto textitem = gscene_->addSimpleText(QString::fromStdString(label));
                        auto font = textitem->font();
                        font.setPointSizeF(font.pointSizeF()*fontscale);
                        textitem->setFont(font);
                        textitem->setBrush(QBrush(namecolor));
                        textitem->setPos(-textitem->boundingRect().width()/2, -4*pointscale - textitem->boundingRect().height());
                        textgroupitem->addToGroup(textitem);
                        textgroupitem->setFlag(QGraphicsItem::ItemIgnoresTransformations);
                        groupitem->addToGroup(textgroupitem);
                    }

                    groupitem->setPos(20*x, -20*y);
                    groupitem->setZValue(townzvalue);
                }

                // Draw vassal relations
                if (ui->vassals_checkbox->isChecked())
                {
                    try
                    {
                        auto vassals = mainprg_.ds_.get_town_vassals(townid);
                        for (auto& vassalid : vassals)
                        {
                            auto [vx,vy] = mainprg_.ds_.get_town_coordinates(vassalid);

                            QColor linecolor = Qt::gray;
                            int zvalue = -2;

                            if (mainprg_.prev_result.first == MainProgram::ResultType::HIERARCHY)
                            {
                                if (res_place < prev_result.end())
                                {
                                    if ((res_place != prev_result.begin() && *(res_place-1) == vassalid) ||
                                        (res_place+1 != prev_result.end() && *(res_place+1) == vassalid))
                                    {
                                        linecolor = Qt::red;
                                        zvalue = 10;
                                    }
                                }
                            }

                //            auto pen = QPen(linecolor);
                //            pen.setWidth(0); // "Cosmetic" pen
                //            auto lineitem = gscene_->addLine(20*x+4, -20*y+4, 20*vx+4, -20*vy+4, pen);
                            auto groupitem = gscene_->createItemGroup({});
                            auto pen = QPen(linecolor);
                            pen.setWidth(0); // "Cosmetic" pen

                            double const PI  = 3.141592653589793238463;
                            auto arrowSize = 10*pointscale;
                            QLineF line(QPointF(0, 0), QPointF(20*(vx-x), -20*(vy-y)));

                            double angle = std::atan2(-line.dy(), line.dx());

                            QPointF arrowP1 = line.p1() + QPointF(sin(angle + PI / 3) * arrowSize,
                                                            cos(angle + PI / 3) * arrowSize);
                            QPointF arrowP2 = line.p1() + QPointF(sin(angle + PI - PI / 3) * arrowSize,
                                                            cos(angle + PI - PI / 3) * arrowSize);

                            QPolygonF arrowHead;
                            arrowHead << line.p1() << arrowP1 << arrowP2;
                            auto lineitem = gscene_->addLine(line, pen);
                            groupitem->addToGroup(lineitem);
                            auto headitem = gscene_->addPolygon(arrowHead, pen, QBrush(pen.color()));
                            headitem->setFlag(QGraphicsItem::ItemIgnoresTransformations);
                            groupitem->addToGroup(headitem);
                            groupitem->setPos(20*x, -20*y);
                            groupitem->setZValue(zvalue);
                        }
                    }
                    catch (NotImplemented const& e)
                    {
                        errorset.insert(std::string("NotImplemented while updating graphics: ") + e.what());
                        std::cerr << std::endl << "NotImplemented while updating graphics: " << e.what() << std::endl;
                    }
                }

                // Draw roads
                if (ui->roads_checkbox->isChecked())
                {
                    try
                    {
                        auto roads = mainprg_.ds_.get_roads_from(townid);
                        for (auto& roadid : roads)
                        {
                            auto [rx,ry] = mainprg_.ds_.get_town_coordinates(roadid);

                            QColor linecolor = Qt::white;
                            int zvalue = -1;

                            if (mainprg_.prev_result.first == MainProgram::ResultType::ROUTE ||
                                mainprg_.prev_result.first == MainProgram::ResultType::CYCLE)
                            {
                                if (res_place < prev_result.end())
                                {
                                    if ((res_place != prev_result.begin() && *(res_place-1) == roadid) ||
                                        (res_place+1 != prev_result.end() && *(res_place+1) == roadid))
                                    {
                                        linecolor = Qt::red;
                                        zvalue = 10;
                                    }
                                }
                            }

                            auto pen = QPen(linecolor);
                            pen.setWidth(0); // "Cosmetic" pen
                            auto lineitem = gscene_->addLine(20*x, -20*y, 20*rx, -20*ry, pen);
                            lineitem->setZValue(zvalue);
                //            auto pen = QPen(linecolor);
                //            pen.setWidth(0); // "Cosmetic" pen
                //            auto arrow = new Arrow(QPointF(20*x+4, -20*y+4), QPointF(20*rx+4, -20*ry+4));
                //            arrow->setPen(pen);
                //            gscene_->addItem(arrow);
                //            arrow->setZValue(-1);
                        }
                    }
                    catch (NotImplemented const& e)
                    {
                        errorset.insert(std::string("NotImplemented while updating graphics: ") + e.what());
                        std::cerr << std::endl << "NotImplemented while updating graphics: " << e.what() << std::endl;
                    }
                }
            }
            catch (NotImplemented const& e)
            {
                errorset.insert(std::string("NotImplemented while updating graphics: ") + e.what());
                std::cerr << std::endl << "NotImplemented while updating graphics: " << e.what() << std::endl;
            }
        }
    }
    catch (NotImplemented const& e)
    {
        errorset.insert(std::string("NotImplemented while updating graphics: ") + e.what());
        std::cerr << std::endl << "NotImplemented while updating graphics: " << e.what() << std::endl;
    }

    if (!errorset.empty())
    {
        std::ostringstream errorstream;
        for (auto const& errormsg : errorset)
        {
            errorstream << "Error from GUI: " << errormsg << std::endl;
        }
        output_text(errorstream);
        output_text_end();
    }
}

void MainWindow::output_text(ostringstream& output)
{
    string outstr = output.str();
    if (!outstr.empty())
    {
        if (outstr.back() == '\n') { outstr.pop_back(); } // Remove trailing newline
        ui->output->appendPlainText(QString::fromStdString(outstr));
        ui->output->ensureCursorVisible();
        ui->output->repaint();
    }

    output.str(""); // Clear the stream, because it has already been output
}

void MainWindow::output_text_end()
{
    ui->output->moveCursor(QTextCursor::End);
    ui->output->ensureCursorVisible();
    ui->output->repaint();
}

bool MainWindow::check_stop_pressed() const
{
    QCoreApplication::processEvents();
    return stop_pressed_;
}

void MainWindow::execute_line()
{
    auto line = ui->lineEdit->text();
    clear_input_line();
    ui->output->appendPlainText(QString::fromStdString(MainProgram::PROMPT)+line);

    ui->execute_button->setEnabled(false);
    ui->stop_button->setEnabled(true);
    stop_pressed_ = false;

    ostringstream output;
    bool cont = mainprg_.command_parse_line(line.toStdString(), output);
    ui->lineEdit->clear();
    output_text(output);
    output_text_end();

    ui->stop_button->setEnabled(false);
    ui->execute_button->setEnabled(true);
    stop_pressed_ = false;

    ui->lineEdit->setFocus();

//    if (mainprg_.view_dirty)
//    {
//        update_view();
//        mainprg_.view_dirty = false;
//    }
    update_view();

    if (!cont)
    {
        close();
    }
}

void MainWindow::cmd_selected(int idx)
{
    ui->lineEdit->insert(QString::fromStdString(mainprg_.cmds_[idx].cmd+" "));
    ui->cmd_info_text->setText(QString::fromStdString(mainprg_.cmds_[idx].cmd+" "+mainprg_.cmds_[idx].info));

    ui->lineEdit->setFocus();
}

void MainWindow::number_selected(QString const& number)
{
    ui->lineEdit->insert(number+" ");

    ui->lineEdit->setFocus();
}

void MainWindow::select_file()
{
    QFileDialog dialog(this, "Select file", QDir::currentPath(), "Command list (*.txt)");
    dialog.setFileMode(QFileDialog::ExistingFiles);
    if (dialog.exec())
    {
        auto filenames = dialog.selectedFiles();
        for (auto& i : filenames)
        {
            auto filename = QDir("").relativeFilePath(i);
            ui->lineEdit->insert("\""+filename+"\" ");
        }
    }

    ui->lineEdit->setFocus();
}

void MainWindow::clear_input_line()
{
    ui->cmd_info_text->clear();
    ui->lineEdit->clear();
    ui->lineEdit->setFocus();
}

void MainWindow::fit_view()
{
    ui->graphics_view->fitInView(gscene_->itemsBoundingRect(), Qt::KeepAspectRatio);
}

void MainWindow::scene_selection_change()
{
    auto items = gscene_->selectedItems();
    if (!items.empty())
    {
        for (auto i : items)
        {
            auto data = i->data(0);
//            assert(stopid!=NO_ID && "NO_ID as stop in graphics view");
            if (!selection_clear_in_progress)
            {
                ostringstream output;
                output << "*click* ";
                std::string ui_str;
                if (data.canConvert<TownID>())
                {
                    auto townid = data.value<TownID>();
                    ui_str = mainprg_.print_town(townid, output);
                }
                else if (data.canConvert<Coord>())
                {
                    auto coord = data.value<Coord>();
                    ui_str = mainprg_.print_coord(coord, output);
                }
                if (!ui_str.empty()) { ui->lineEdit->insert(QString::fromStdString(ui_str+" ")); }
                output_text(output);
                output_text_end();
            }
            i->setSelected(false);
            selection_clear_in_progress = !selection_clear_in_progress;
        }
    }
}

void MainWindow::clear_selection()
{
    gscene_->clearSelection();
}



// Originally in main.cc
#include <QApplication>

int main(int argc, char *argv[])
{
    if (argc == 1)
    {
        QApplication a(argc, argv);
        MainWindow w;
        w.show();

        auto status = a.exec();
        std::cerr << "Program ended normally." << std::endl;
        return status;
    }
    else
    {
        return MainProgram::mainprogram(argc, argv);
    }
}

#else

int main(int argc, char *argv[])
{
    return MainProgram::mainprogram(argc, argv);
}
#endif
