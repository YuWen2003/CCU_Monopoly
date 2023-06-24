#define SFML_STATIC 1
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>

void putText(sf::Text &text, float x, float y) {

    sf::FloatRect textRect = text.getLocalBounds();
    text.setOrigin(textRect.left + textRect.width/2.0f,
               textRect.top  + textRect.height/2.0f);
    text.setPosition(sf::Vector2f(x, y));
}


void putButton(sf::Sprite &s, float x, float y) {

    sf::FloatRect textRect = s.getLocalBounds();
    s.setOrigin(textRect.left + textRect.width/2.0f,
               textRect.top  + textRect.height/2.0f);
    s.setPosition(sf::Vector2f(x, y));
}

int main()
{
    sf::RenderWindow window(sf::VideoMode(1280,720), "NTU", sf::Style::Fullscreen);

    sf::Texture bg;
    bg.loadFromFile("resource/pic/ntu_big_gate.jpg");

    sf::Sprite bg_s(bg);
    float SCRWIDTH = sf::VideoMode::getDesktopMode().width;
    float SCRHEIGHT = sf::VideoMode::getDesktopMode().height;
    sf::Vector2f targetSize( sf::VideoMode::getDesktopMode().width , sf::VideoMode::getDesktopMode().height); 
    bg_s.setScale(targetSize.x / bg_s.getLocalBounds().width, targetSize.y / bg_s.getLocalBounds().height);


    sf::Texture button;
    button.loadFromFile("resource/pic/button.png");
    sf::Sprite button_s(button);
    button_s.setScale(100 / button_s.getLocalBounds().width, 43 / button_s.getLocalBounds().height);
    

    sf::Font font;
    if(!font.loadFromFile("resource/font/mingliu.ttc")) { std::cout << "can't load font" << std::endl;}
    sf::Text title, op1, op2, op3, back;
    sf::Text rules[6];
    title.setFont(font); 
    title.setString("MAIN MENU");
    title.setCharacterSize(25);
    title.setFillColor(sf::Color::Black);

    op1.setFont(font); 
    op1.setString("PLAY");
    op1.setCharacterSize(25);
    op1.setFillColor(sf::Color::White);

    op2.setFont(font); 
    op2.setString("ROLE");
    op2.setCharacterSize(25);
    op2.setFillColor(sf::Color::White);

    op3.setFont(font); 
    op3.setString("QUIT");
    op3.setCharacterSize(25);
    op3.setFillColor(sf::Color::White);

    wchar_t rule_text[][100] =  {L"遊戲規則:初始金額10000元", L"玩法:1.停在無人之系館,選擇是否購買", L"2.停在他人之系館,支付過路費", L"3.停在自己之系館,選擇是否捐一棟系館", L"勝利條件:1.擁有1/5的系館", L"2.賺到20000元3.擁有5棟房子"};
    for(int x = 0; x < 6; x++) {
        rules[x].setFont(font); 
        rules[x].setString(rule_text[x]);
        rules[x].setCharacterSize(25);
        rules[x].setFillColor(sf::Color::White);
        putText(rules[x], SCRWIDTH/2.0f, SCRHEIGHT/2.0f + 80 * (x - 3));
    }

    back.setFont(font); 
    back.setString("Back");
    back.setCharacterSize(25);
    back.setFillColor(sf::Color::White);
    putText(back, SCRWIDTH/2.0f, SCRHEIGHT/2.0f + 80 * (7 - 3));


    sf::FloatRect textRect = title.getLocalBounds();
    title.setOrigin(textRect.left + textRect.width/2.0f,
               textRect.top  + textRect.height/2.0f);
    title.setPosition(sf::Vector2f(SCRWIDTH/2.0f,70.0f));

    putText(op1, SCRWIDTH/2.0f, 250.0f);
    putText(op2, SCRWIDTH/2.0f, 400.0f);
    putText(op3, SCRWIDTH/2.0f, 550.0f);

    sf::SoundBuffer buffer;
    buffer.loadFromFile("resource/sound/bgm.wav");
    sf::Sound sound;
    sound.setBuffer(buffer);
    sound.play();

    bool isInRule = 0;

    while (window.isOpen())
    {
        sf::Event event;
        sf::Vector2i cursorPos = sf::Mouse::getPosition(window);
        window.clear();

        if(isInRule) {
            while (window.pollEvent(event))
            {
                if (event.type == sf::Event::Closed)
                    window.close();
                else if (event.type == sf::Event::MouseButtonPressed)
                {
                    if (event.mouseButton.button == sf::Mouse::Left)
                    {
                        if (back.getGlobalBounds().contains(static_cast<sf::Vector2f>(cursorPos)))
                            isInRule = 0;
                    }
                }
            }
            if (back.getGlobalBounds().contains(static_cast<sf::Vector2f>(cursorPos)))
                back.setFillColor(sf::Color::Green);
            else
                back.setFillColor(sf::Color::White);
            for(int x = 0; x < 6; x++) {
                window.draw(rules[x]);
            }
            window.draw(back);
            window.display();
            continue;
        }

        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
            else if (event.type == sf::Event::MouseButtonPressed)
            {
                if (event.mouseButton.button == sf::Mouse::Left)
                {
                    if (op2.getGlobalBounds().contains(static_cast<sf::Vector2f>(cursorPos)))
                        isInRule = 1;
                    else if (op3.getGlobalBounds().contains(static_cast<sf::Vector2f>(cursorPos)))
                        window.close();
                }
            }
        }
        op1.setFillColor(sf::Color::White);
        op2.setFillColor(sf::Color::White);
        op3.setFillColor(sf::Color::White);
        if (op1.getGlobalBounds().contains(static_cast<sf::Vector2f>(cursorPos)))
            op1.setFillColor(sf::Color::Black);
        if (op2.getGlobalBounds().contains(static_cast<sf::Vector2f>(cursorPos)))
            op2.setFillColor(sf::Color::Black);
        if (op3.getGlobalBounds().contains(static_cast<sf::Vector2f>(cursorPos)))
            op3.setFillColor(sf::Color::Black);

        window.draw(bg_s);
        putButton(button_s, SCRWIDTH/2.0f, 250.0f);
        window.draw(button_s);
        putButton(button_s, SCRWIDTH/2.0f, 400.0f);
        window.draw(button_s);
        putButton(button_s, SCRWIDTH/2.0f, 550.0f);
        window.draw(button_s);
        window.draw(title);
        window.draw(op1);
        window.draw(op2);
        window.draw(op3);
        window.display();
    }

    return 0;
}