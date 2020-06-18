#include "alarm.h"

#include <algorithm>

alarm::alarm(QString string)
{
    std::srand(time(0));
    player = new QMediaPlayer;
    timer = new QTimer;
    should_play = true;
    set_working_time(45);
    set_pause_time(5);
    music_directory = 0;
    set_dir(string);
    player->setVolume(20);
    timer->setInterval(1000);
    QObject::connect(timer, &QTimer::timeout, this, &alarm::decrease_second);
    QObject::connect(player, &QMediaPlayer::mediaStatusChanged, this, &alarm::add_next);
    minute = 0;
    second = 0;
    state = status::work;
}

alarm::~alarm()
{
    delete player;
    delete timer;
}

void alarm::set_volume(int vol)
{
    player->setVolume(vol);
}

int alarm::get_volume()
{
    return player->volume();
}

void alarm::set_working_time(int time)
{
    working_time = time;
    if (state == status::work)
    {
        minute = time;
        second = 0;
        emit change_time(minute, second);
    }
}

void alarm::set_pause_time(int time)
{
    pause_time = time;
    if (state == status::pause)
    {
        minute = time;
        second = 0;
        emit change_time(minute, second);
    }
}

bool alarm::set_dir(QString dirname)
{
    if (music_directory != 0)
    {
        delete music_directory;
        play_list.clear();
    }
    music_directory = new QDir(dirname);
    QStringList filter;
    filter << "*.mp3";
    music_directory->setNameFilters(filter);
    play_list = music_directory->entryInfoList().toVector();
    if (play_list.length() == 0)
        return false;
    std::random_shuffle(play_list.begin(), play_list.end());
    count = 0;
    player->setMedia(QUrl::fromLocalFile(play_list[count].absoluteFilePath()));
    if (state == status::pause)
        play_music();
    return true;
}

void alarm::play_music()
{
    if (!should_play)
        return;
    if (play_list.length() == 0)
        return;
    if (player->state() == QMediaPlayer::PlayingState)
        player->pause();
    player->play();
}

void alarm::next()
{
    if (play_list.length() == 0)
        return;
    count = (count + 1) % play_list.size();
    player->setMedia(QUrl::fromLocalFile(play_list[count].absoluteFilePath()));
    if (player->state() == QMediaPlayer::PlayingState)
        play_music();
    else
        stop_music();
}

void alarm::add_next(QMediaPlayer::MediaStatus state)
{
    if (state == QMediaPlayer::EndOfMedia)
        next();
}

QString alarm::name()
{
    return play_list[count].fileName();
}

void alarm::stop_music()
{
    player->pause();
}

void alarm::toggle_music()
{
    if (player->state() == QMediaPlayer::PlayingState)
        stop_music();
    else
        play_music();
}

int alarm::get_working_time()
{
    return working_time;
}

int alarm::get_pause_time()
{
    return pause_time;
}

QMediaPlayer *alarm::get_media_player()
{
    return player;
}

QFileInfo *alarm::get_current()
{
    if (play_list.length() == 0)
        return nullptr;
    else
        return &(play_list[count]);
}

void alarm::decrease_second()
{
    if (second != 0)
        second--;
    else
    {
        second = 59;
        minute--;
    }
    if (minute == 0 && second == 0)
        change_state();
    emit change_time(minute, second);
}

void alarm::change_state()
{
    if (state == status::pause)
    {
        state = status::work;
        minute = working_time;
        second = 0;
        stop_music();
    }
    else
    {
        state = status::pause;
        minute = pause_time;
        second = 0;
        play_music();
    }
    emit change_to_state(state);
}

void alarm::start()
{
    if (minute == 0 && second == 0)
    {
        minute = working_time;
        second = 0;
        emit change_time(minute, second);
    }
    timer->start();
    if (state == status::pause)
        play_music();
}

void alarm::stop()
{
    timer->stop();
    stop_music();
}

void alarm::toggle()
{
    if (timer->isActive())
        stop();
    else
        start();
}

QString alarm::dir_name()
{
    return music_directory->absolutePath();
}

void alarm::reset()
{
    if (state == status::work)
    {
        minute = working_time;
        second = 0;
    }
    else if (state == status::pause)
    {
        minute = pause_time;
        second = 0;
    }
    emit change_time(minute, second);
}

void alarm::toggle_mute()
{
    if (player->isMuted())
        player->setMuted(false);
    else
        player->setMuted(true);
}

void alarm::toggle_song()
{
    should_play = !should_play;
    if (should_play && state == status::pause)
        play_music();
    if ((!should_play) && player->state() == QMediaPlayer::PlayingState)
        stop_music();
}

bool alarm::musicable()
{
    return should_play;
}
