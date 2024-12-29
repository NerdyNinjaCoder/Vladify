#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <cstdlib>
#include <ctime>

using namespace std;
using namespace sf;

class MusicPlayer
{
private:
    RenderWindow window;
    Font font;
    Music music;

    vector<pair<string, string>> songs; // Плейлист с названием и путем к файлу
    vector<Text> playlistTexts;
    int currentSongIndex;
    bool isPlaying;
    bool isRandom;

    RectangleShape playlistBackground;
    RectangleShape currentSongBackground;
    RectangleShape progressBarBackground;
    RectangleShape progressBar;

    Text currentSongText;
    Text currentTimeText;
    Text totalTimeText;
    Text playButtonText;
    Text prevButtonText;
    Text nextButtonText;
    Text volumeUpText;
    Text volumeDownText;
    Text randomButtonText;

    RectangleShape playButton;
    RectangleShape prevButton;
    RectangleShape nextButton;
    RectangleShape randomButton;
    RectangleShape volumeUpButton;
    RectangleShape volumeDownButton;

    float volume;

    string formatTime(Time time)
    {
        int totalSeconds = static_cast<int>(time.asSeconds());
        int minutes = totalSeconds / 60;
        int seconds = totalSeconds % 60;
        ostringstream oss;
        oss << (minutes < 10 ? "0" : "") << minutes << ":"
            << (seconds < 10 ? "0" : "") << seconds;
        return oss.str();
    }

    void updateCurrentSong()
    {
        currentSongText.setString(songs[currentSongIndex].first);
        FloatRect textBounds = currentSongText.getLocalBounds();
        currentSongText.setPosition(currentSongBackground.getPosition().x + (currentSongBackground.getSize().x - textBounds.width) / 2, 75);
        totalTimeText.setString(formatTime(music.getDuration()));

        for (size_t i = 0; i < playlistTexts.size(); ++i)
        {
            if (i == currentSongIndex)
            {
                playlistTexts[i].setFillColor(Color::Green);
            }
            else
            {
                playlistTexts[i].setFillColor(Color::White);
            }
        }
    }

    void initializeInterface()
    {
        playlistBackground.setSize(Vector2f(250, 600));
        playlistBackground.setPosition(0, 0);
        playlistBackground.setFillColor(Color(40, 44, 52));
        playlistBackground.setOutlineThickness(2);
        playlistBackground.setOutlineColor(Color(60, 60, 60));

        for (size_t i = 0; i < songs.size(); ++i)
        {
            Text songText(songs[i].first, font, 20);
            songText.setPosition(20, 50 + i * 50);
            songText.setFillColor(Color::White);
            playlistTexts.push_back(songText);
        }

        currentSongText.setFont(font);
        currentSongText.setFillColor(Color::White);
        currentSongText.setCharacterSize(30);

        currentSongBackground.setSize(Vector2f(700, 100));
        currentSongBackground.setFillColor(Color(50, 50, 50));
        currentSongBackground.setOutlineThickness(3);
        currentSongBackground.setOutlineColor(Color(100, 100, 255));
        currentSongBackground.setPosition(260, 50);

        progressBarBackground.setSize(Vector2f(700, 10));
        progressBarBackground.setFillColor(Color(50, 50, 50));
        progressBarBackground.setPosition(260, 160);

        progressBar.setSize(Vector2f(0, 10));
        progressBar.setFillColor(Color(100, 200, 100));
        progressBar.setPosition(260, 160);

        currentTimeText.setFont(font);
        currentTimeText.setFillColor(Color::White);
        currentTimeText.setCharacterSize(20);
        currentTimeText.setPosition(260, 180);

        totalTimeText.setFont(font);
        totalTimeText.setFillColor(Color::White);
        totalTimeText.setCharacterSize(20);
        totalTimeText.setPosition(910, 180);

        playButton.setSize(Vector2f(100, 50));
        playButton.setFillColor(Color(70, 190, 80));
        playButton.setPosition(450, 500);

        playButtonText.setFont(font);
        playButtonText.setString("Play");
        playButtonText.setFillColor(Color::White);
        playButtonText.setCharacterSize(20);
        playButtonText.setPosition(475, 510);

        prevButton.setSize(Vector2f(100, 50));
        prevButton.setFillColor(Color(70, 140, 190));
        prevButton.setPosition(300, 500);

        prevButtonText.setFont(font);
        prevButtonText.setString("Prev");
        prevButtonText.setFillColor(Color::White);
        prevButtonText.setCharacterSize(20);
        prevButtonText.setPosition(325, 510);

        nextButton.setSize(Vector2f(100, 50));
        nextButton.setFillColor(Color(70, 140, 190));
        nextButton.setPosition(600, 500);

        nextButtonText.setFont(font);
        nextButtonText.setString("Next");
        nextButtonText.setFillColor(Color::White);
        nextButtonText.setCharacterSize(20);
        nextButtonText.setPosition(625, 510);

        randomButton.setSize(Vector2f(100, 50));
        randomButton.setFillColor(Color(70, 140, 190));
        randomButton.setPosition(750, 500);

        randomButtonText.setFont(font);
        randomButtonText.setString("Random");
        randomButtonText.setFillColor(Color::White);
        randomButtonText.setCharacterSize(20);
        randomButtonText.setPosition(770, 510);

        volumeUpButton.setSize(Vector2f(50, 50));
        volumeUpButton.setFillColor(Color(200, 200, 100));
        volumeUpButton.setPosition(900, 400);

        volumeUpText.setFont(font);
        volumeUpText.setString("+");
        volumeUpText.setFillColor(Color::Black);
        volumeUpText.setCharacterSize(20);
        volumeUpText.setPosition(920, 410);

        volumeDownButton.setSize(Vector2f(50, 50));
        volumeDownButton.setFillColor(Color(200, 200, 100));
        volumeDownButton.setPosition(900, 460);

        volumeDownText.setFont(font);
        volumeDownText.setString("-");
        volumeDownText.setFillColor(Color::Black);
        volumeDownText.setCharacterSize(20);
        volumeDownText.setPosition(920, 470);

        volume = 50.0f;
    }

    void handleEvents()
    {
        Event event;
        while (window.pollEvent(event))
        {
            if (event.type == Event::Closed)
            {
                window.close();
            }

            if (event.type == Event::KeyPressed)
            {
                if (event.key.code == Keyboard::Space)
                {
                    togglePlayPause();
                }
            }

            if (event.type == Event::MouseButtonPressed && event.mouseButton.button == Mouse::Left)
            {
                Vector2i mousePosition = Mouse::getPosition(window);

                // Обработка кликов по плейлисту
                for (size_t i = 0; i < playlistTexts.size(); ++i)
                {
                    if (playlistTexts[i].getGlobalBounds().contains(static_cast<float>(mousePosition.x), static_cast<float>(mousePosition.y)))
                    {
                        currentSongIndex = i;
                        loadAndPlaySong();
                    }
                }

                if (progressBarBackground.getGlobalBounds().contains(static_cast<float>(mousePosition.x), static_cast<float>(mousePosition.y)))
                {
                    float clickPosition = static_cast<float>(mousePosition.x) - progressBarBackground.getPosition().x;
                    float progress = clickPosition / progressBarBackground.getSize().x;
                    if (music.getDuration().asSeconds() > 0)
                    {
                        Time newPlayingOffset = music.getDuration() * progress;
                        music.setPlayingOffset(newPlayingOffset);
                        currentTimeText.setString(formatTime(music.getPlayingOffset()));
                    }
                }

                if (playButton.getGlobalBounds().contains(static_cast<float>(mousePosition.x), static_cast<float>(mousePosition.y)))
                {
                    togglePlayPause();
                }

                if (prevButton.getGlobalBounds().contains(static_cast<float>(mousePosition.x), static_cast<float>(mousePosition.y)))
                {
                    previousSong();
                }

                if (nextButton.getGlobalBounds().contains(static_cast<float>(mousePosition.x), static_cast<float>(mousePosition.y)))
                {
                    nextSong();
                }

                if (volumeUpButton.getGlobalBounds().contains(static_cast<float>(mousePosition.x), static_cast<float>(mousePosition.y)))
                {
                    changeVolume(10.0f);
                }

                if (volumeDownButton.getGlobalBounds().contains(static_cast<float>(mousePosition.x), static_cast<float>(mousePosition.y)))
                {
                    changeVolume(-10.0f);
                }

                if (randomButton.getGlobalBounds().contains(static_cast<float>(mousePosition.x), static_cast<float>(mousePosition.y)))
                {
                    toggleRandomMode();
                }
            }
        }
    }

    void loadAndPlaySong()
    {
        if (!music.openFromFile(songs[currentSongIndex].second))
        {
            cerr << "Ошибка загрузки музыкального файла: " << songs[currentSongIndex].second << endl;
            return;
        }
        music.setVolume(volume);
        music.play();
        playButtonText.setString("Pause");
        isPlaying = true;
        updateCurrentSong();
    }

    void togglePlayPause()
    {
        if (isPlaying)
        {
            music.pause();
            playButtonText.setString("Play");
        }
        else
        {
            music.play();
            playButtonText.setString("Pause");
        }
        isPlaying = !isPlaying;
    }

    void previousSong()
    {
        currentSongIndex = (currentSongIndex - 1 + songs.size()) % songs.size();
        loadAndPlaySong();
    }

    void nextSong()
    {
        if (isRandom)
        {
            currentSongIndex = rand() % songs.size();
        }
        else
        {
            currentSongIndex = (currentSongIndex + 1) % songs.size();
        }
        loadAndPlaySong();
    }

    void stopSong()
    {
        music.stop();
        playButtonText.setString("Play");
        isPlaying = false;
    }

    void toggleRandomMode()
    {
        isRandom = !isRandom;
        randomButton.setFillColor(isRandom ? Color(70, 190, 80) : Color(190, 70, 70)); // Зеленый для включенного, красный для выключенного
    }

    void changeVolume(float delta)
    {
        volume = max(0.0f, min(100.0f, volume + delta));
        music.setVolume(volume);
    }

    void updateProgress()
    {
        if (isPlaying)
        {
            // Обновление прогресса
            float progress = music.getPlayingOffset().asSeconds() / music.getDuration().asSeconds();
            progressBar.setSize(Vector2f(progress * progressBarBackground.getSize().x, progressBar.getSize().y));

            // Обновление текущего времени
            currentTimeText.setString(formatTime(music.getPlayingOffset()));

            if (music.getStatus() == Music::Status::Stopped)
            {
                nextSong();
            }
        }
    }

public:
    MusicPlayer() : window(VideoMode(1000, 600), "Vladify"), currentSongIndex(0), isPlaying(false), isRandom(false)
    {
        srand(time(0));  // Инициализация генератора случайных чисел
        if (!font.loadFromFile("C:/Users/kaurk/source/repos/Плеер/x64/Debug/arialmt.ttf"))
        {
            cerr << "Ошибка загрузки шрифта!" << endl;
            exit(-1);
        }

        songs =
        {
            {"ski-ba-bop-ba-dop-bop", "C:/Users/kaurk/source/repos/Плеер/x64/Debug/Scatman_John_-_Scatman_ski-ba-bop-ba-dop-bop_73330111.ogg"},
            {"Una mattina", "C:/Users/kaurk/source/repos/Плеер/x64/Debug/Ludovico_Einaudi_-_Una_Mattina_48077742.ogg"},
            {"TNT", "C:/Users/kaurk/source/repos/Плеер/x64/Debug/ACDC_-_TNT_39240727.ogg"},
            {"That s-Life", "C:/Users/kaurk/source/repos/Плеер/x64/Debug/Frank-Sinatra-That_s-Life.ogg"},
            {"Highway to hell", "C:/Users/kaurk/source/repos/Плеер/x64/Debug/ACDC_-_Highway_To_Hell_39240749.ogg"},
            {"Whole lottaRosie", "C:/Users/kaurk/source/repos/Плеер/x64/Debug/ACDC_-_Whole_LottaRosie_39240756.ogg"},
            {"Hit the road jack", "C:/Users/kaurk/source/repos/Плеер/x64/Debug/Ray_Charles_The_Raelets_-_Hit_the_Road_Jack_60310959.ogg"}
        };

        initializeInterface();
    }

    void run()
    {
        loadAndPlaySong(); // Начинаем воспроизведение первой песни при старте

        while (window.isOpen())
        {
            handleEvents();

            // Обновление состояния прогресса и времени
            updateProgress();

            window.clear(Color(30, 30, 30));
            window.draw(playlistBackground);

            for (const auto& text : playlistTexts)
            {
                window.draw(text);
            }

            window.draw(currentSongBackground);
            window.draw(currentSongText);

            window.draw(progressBarBackground);
            window.draw(progressBar);

            window.draw(currentTimeText);
            window.draw(totalTimeText);

            window.draw(playButton);
            window.draw(playButtonText);

            window.draw(prevButton);
            window.draw(prevButtonText);

            window.draw(nextButton);
            window.draw(nextButtonText);

            window.draw(randomButton);  // Отображаем кнопку random
            window.draw(randomButtonText);  // Отображаем текст на кнопке

            window.draw(volumeUpButton);
            window.draw(volumeUpText);

            window.draw(volumeDownButton);
            window.draw(volumeDownText);

            window.display();
        }
    }
};

int main()
{
    MusicPlayer player;
    player.run();
    return 0;
}

