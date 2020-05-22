#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "alarm.h"

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void on_playbutton_clicked();

    void on_working_time_box_valueChanged(int arg1);

    void on_pause_time_box_valueChanged(int arg1);

    void on_nextbutton_clicked();

    void update_music_name(QMediaContent media);

    void show_click(QSystemTrayIcon::ActivationReason reason);

    void change_to_state(status state);

    void update_time_display(int minute,int second);

    void changeEvent(QEvent *event) override;

    void on_dir_button_clicked();

    void close_all();

    void show_window();

    void on_reset_button_clicked();

private:
    Ui::MainWindow *ui;
    alarm* x;
    QSystemTrayIcon* tray;
    QMenu* tray_menu;
    QSettings* settings = nullptr;
    QString default_dir();
    QString fill_zero(int num);
};
#endif // MAINWINDOW_H
