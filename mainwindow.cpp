#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    settings = new QSettings(QCoreApplication::applicationDirPath() + "/dir.ini", QSettings::IniFormat);
    QString dir = settings->value("dir", default_dir()).toString();
    x = new alarm(dir);
    update_time_display(x->get_working_time(), 0);
    ui->working_time_box->setValue(x->get_working_time());
    ui->pause_time_box->setValue(x->get_pause_time());
    ui->vol_box->setValue(x->get_volume());
    update_music_name(QMediaContent());
    QObject::connect(x->get_media_player(), &QMediaPlayer::mediaChanged, this, &MainWindow::update_music_name);
    tray = new QSystemTrayIcon(QIcon(":/icon/ZClock.ico"));
    tray_menu = new QMenu;
    tray_menu->addAction("Show", this, &MainWindow::show_window);
    tray_menu->addAction("Quit", this, &MainWindow::close_all);
    tray->show();
    tray->setContextMenu(tray_menu);
    QObject::connect(tray, &QSystemTrayIcon::activated, this, &MainWindow::show_click);
    QObject::connect(x, &alarm::change_to_state, this, &MainWindow::change_to_state);
    QObject::connect(x, &alarm::change_time, this, &MainWindow::update_time_display);
    ui->dir_text->setText(x->dir_name());
}

QString MainWindow::default_dir()
{
    return QStandardPaths::standardLocations(QStandardPaths::MusicLocation).last();
}

MainWindow::~MainWindow()
{
    delete x;
    delete tray_menu;
    delete tray;
    delete ui;
}

void MainWindow::on_playbutton_clicked()
{
    x->toggle();
}

void MainWindow::on_working_time_box_valueChanged(int time)
{
    x->set_working_time(time);
}

void MainWindow::on_pause_time_box_valueChanged(int time)
{
    x->set_pause_time(time);
}

void MainWindow::on_nextbutton_clicked()
{
    x->next();
}

void MainWindow::update_music_name(QMediaContent media __attribute__((unused)))
{
    QFileInfo *ptr = x->get_current();
    QString str;
    if (ptr == nullptr)
        str = "No Song Available";
    else
        str = ptr->fileName();
    ui->musicname->setText(str);
}

void MainWindow::show_window()
{
    showNormal();
    QVariant q = settings->value("geometry", 0);
    if (q != 0)
    {
        setGeometry(q.toRect());
    }
    raise();
}

void MainWindow::show_click(QSystemTrayIcon::ActivationReason reason __attribute__((unused)))
{
    show_window();
}

void MainWindow::close_all()
{
    tray->hide();
    QMainWindow::close();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (tray->isVisible())
    {
        hide();
        event->ignore();
    }
}

void MainWindow::changeEvent(QEvent *event)
{
    QMainWindow::changeEvent(event);
    if (event->type() == QEvent::WindowStateChange)
    {
        if (isMinimized())
            this->hide();
    }
}

void MainWindow::change_to_state(status state)
{
    if (state == status::work)
        ui->label_status->setText("Work");
    else
        ui->label_status->setText("Pause");
}

void MainWindow::update_time_display(int minute, int second)
{
    ui->label_minute->display(fill_zero(minute));
    ui->label_second->display(fill_zero(second));
}

QString MainWindow::fill_zero(int num)
{
    if (num >= 100)
    {
        return QString::number(num % 100);
    }
    else if (num < 10)
    {
        return "0" + QString::number(num);
    }
    else
    {
        return QString::number(num);
    }
}

void MainWindow::on_dir_button_clicked()
{
    QString dir;
    while (true)
    {
        dir = QFileDialog::getExistingDirectory(this, tr("Choose Directory"), x->dir_name(),
                                                QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
        if (dir.isEmpty())
            return;
        if (x->set_dir(dir))
            break;
    }
    settings->setValue("dir", dir);
    settings->sync();
    ui->dir_text->setText(x->dir_name());
}

void MainWindow::on_reset_button_clicked()
{
    x->reset();
}

void MainWindow::on_song_button_clicked()
{
    x->toggle_song();
    if (x->musicable())
        ui->song_button->setText("Stop Song");
    else
        ui->song_button->setText("Play Song");
}

void MainWindow::on_vol_box_valueChanged(int vol)
{
    x->set_volume(vol);
}
