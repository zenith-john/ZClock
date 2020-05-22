#ifndef ALARM_H
#define ALARM_H

#include <QtMultimedia>
#include <QtMultimediaWidgets>

enum status{
    work,
    pause
};

class alarm: public QObject
{
    Q_OBJECT

public:
    alarm(QString string);
    ~alarm();
    void play_music();
    void stop_music();
    QString name();
    void next();
    void toggle_music();
    void set_working_time(int time);
    void set_pause_time(int time);
    int get_working_time();
    int get_pause_time();
    void start();
    void stop();
    void toggle();
    void reset();
    bool set_dir(QString dirname);
    QMediaPlayer* get_media_player();
    QFileInfo* get_current();
    QString dir_name();

public slots:
    void decrease_second();
    void add_next(QMediaPlayer::MediaStatus);

signals:
    void change_to_state(status state);
    void change_time(int minute,int second);

private:
    QMediaPlayer* player;
    int working_time;
    int pause_time;
    int minute,second;
    status state;
    QDir* music_directory;
    QVector<QFileInfo> play_list;
    int count;
    QTimer* timer;
    void change_state();
};

#endif // ALARM_H
